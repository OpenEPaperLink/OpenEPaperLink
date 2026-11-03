#!/usr/bin/env python3
"""Worst-case stack depth of a linked TC32 image, read off its listing, and the stack sizes the link
should reserve for it.

    python3 stackdepth.py out/OEPL_TLSR_AP.lst                     report both stacks
    python3 stackdepth.py pass1.lst --defsym out/stack.defsym      ... and write the link's sizes
    python3 stackdepth.py out/OEPL_TLSR_AP.lst --verify            fail unless the image reserves them

Input: `tc32-elf-objdump -x -D` of the .elf (the makefile's .lst, `-l -S` allowed): the section
headers, the symbol table (function sizes) and every section disassembled, from which the bytes of
jump tables and literal pools are read back as well. Output: each root's bound and deepest path;
with --defsym, the link's options (__STACK_MIN__, __IRQ_STACK__ and the two raw bounds); with
--verify, a check of the image's _stack_min_ and _irq_stk_lo_/_irq_stk_hi_ against the sizes it
needs. Standard library only.

tc32-elf-gcc 4.5.1 has no -fstack-usage. A function's stack is what it pushes plus what it subtracts
from sp, and a path's is the sum over the calls on it. This follows each function's control flow
from its entry, so the offset at every call is the one the code really has there, through every
branch and jump table, and the bound is the maximum over the whole call graph from the root.
Anything that would make that number a guess is an error rather than a warning, because the one
result that must never come out of this is a quiet under-estimate:

  - recursion, direct or through any chain;
  - an indirect call or jump whose targets cannot be named (`tjex rN` other than a return, `tmov pc`
    other than a bounded jump table, a call through a function pointer whose candidates cannot be
    enumerated);
  - sp written other than by push/pop and an immediate add/sub, unless it is a frame-pointer restore
    to an offset the analysis already knows (alloca, a variable adjustment, a mode switch);
  - control flow that leaves the function other than by a call to a function start, or runs into
    data, or reaches the same instruction with two different stack offsets;
  - an instruction the listing does not decode.

A call through a register is bounded when the register was loaded from a literal (one target), or
from a fixed variable that only known function addresses are ever stored to (those functions).
Otherwise it is bounded by the image's address-taken functions: in C a function's address only comes
from naming it, which leaves it as a constant in a literal pool or in data, so every word in the
image that is a function's start with the Thumb bit set is a candidate, and the call costs the
deepest of them. That is pessimistic and sound.

Interrupts are a second root on a stack of their own (`__irq`, which cstartup_8258.S runs in IRQ mode
on irq_stk); nothing reachable from it may re-enable interrupts for this to be the IRQ stack's bound,
which is true of irq_handler in src/tlsr/hal_tlsr.c and not something this tool can see. The main
root is main(): cstartup sets SP to _stack_end_ and calls it with nothing on the stack. A CPSR write
is taken as not changing the mode, and reported as a note.

The size is the bound times 4/3 (--margin), rounded up to 16 bytes. The margin is not for the
analysis's own approximations, which can only over-count; it is for what no listing shows, such as a
mode switch or a compiler idiom this has not met. 4/3 is the reciprocal of the firmware's warning
threshold: it warns once when a stack's high-water mark passes 3/4 of its size, which then means the
code went deeper than this bound while a quarter of the stack is still free. Every 16 bytes of margin
is arena.
"""
import re
import sys
from fractions import Fraction

REG_ALIAS = {'sb': 'r9', 'sl': 'r10', 'fp': 'r11', 'ip': 'r12', 'sp': 'r13', 'lr': 'r14', 'pc': 'r15'}
DATA_MNEM = ('.word', '.short', '.byte')
# Mnemonics whose first operand is read, not written
NO_DEST = ('tstorer', 'tstorerb', 'tstorerh', 'tcmp', 'tcmpn', 'tpush', 'tnop', 'tmcsr', 'tmssr', 'treti')
COND_BRANCH = re.compile(r'^tj(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le)$')


class StackError(Exception):
    pass


def reg(name):
    name = name.strip().lower()
    return REG_ALIAS.get(name, name)


def reglist(text):
    """'{r4, r5, lr}' or '{r0-r5}' -> ['r4', 'r5', 'r14']"""
    m = re.match(r'\s*\{(.*)\}', text)
    if not m:
        raise StackError('cannot read register list %r' % text)
    out = []
    for part in m.group(1).split(','):
        part = part.strip()
        if not part:
            continue
        if '-' in part:
            a, b = (reg(x) for x in part.split('-'))
            if not (a.startswith('r') and b.startswith('r')):
                raise StackError('cannot read register range %r' % part)
            out += ['r%d' % i for i in range(int(a[1:]), int(b[1:]) + 1)]
        else:
            out.append(reg(part))
    return out


class Insn:
    __slots__ = ('addr', 'size', 'mnem', 'ops', 'comment', 'section')

    def __init__(self, addr, size, mnem, ops, comment, section):
        self.addr, self.size, self.mnem, self.ops, self.comment, self.section = addr, size, mnem, ops, comment, section

    @property
    def end(self):
        return self.addr + self.size

    def operands(self):
        return [o.strip() for o in re.split(r',(?![^\[\{]*[\]\}])', self.ops)] if self.ops else []

    def target(self):
        m = re.match(r'([0-9a-f]+)\b', self.ops)
        if not m:
            raise StackError('0x%x: no branch target in %r' % (self.addr, self.ops))
        return int(m.group(1), 16)

    def literal(self):
        """the address a `tloadr rN, [pc, #n]` reads"""
        m = re.search(r'\(([0-9a-f]+)\b', self.comment or '')
        if not m:
            raise StackError('0x%x: pc-relative load without its address' % self.addr)
        return int(m.group(1), 16)

    def is_data(self):
        return self.mnem in DATA_MNEM or self.mnem.startswith('undefined')

    def __str__(self):
        return '0x%x: %s %s' % (self.addr, self.mnem, self.ops)


class Listing:
    def __init__(self, text):
        self.code_sections = set()
        self.symbols = []      # (addr, flags, section, size, name)
        self.insns = {}        # addr -> Insn, code sections only
        self.bytes = {}        # addr -> byte, every disassembled section
        self._parse(text)
        self.order = sorted(self.insns)
        self.index = {a: i for i, a in enumerate(self.order)}
        self._functions()

    # --- reading ---------------------------------------------------------------------------------
    def _parse(self, text):
        lines = text.splitlines()
        i, section, state = 0, None, None
        sec_re = re.compile(r'^\s*\d+\s+(\S+)\s+[0-9a-f]{8}\s+[0-9a-f]{8}\s+[0-9a-f]{8}\s+[0-9a-f]{8}\s+2\*\*\d+')
        sym_re = re.compile(r'^([0-9a-f]{8}) (.{7}) (\S+)\t([0-9a-f]{8}) (.*)$')
        ins_re = re.compile(r'^\s*([0-9a-f]+):\t((?:[0-9a-f]{2,8} ?)+)\s*\t?(.*)$')
        while i < len(lines):
            line = lines[i]
            if line.startswith('Sections:'):
                state = 'sections'
            elif line.startswith('SYMBOL TABLE:'):
                state = 'symbols'
            elif line.startswith('Disassembly of section '):
                state = 'dis'
                section = line[len('Disassembly of section '):].rstrip(':')
            elif state == 'sections':
                m = sec_re.match(line)
                if m and i + 1 < len(lines) and 'CODE' in lines[i + 1]:
                    self.code_sections.add(m.group(1))
            elif state == 'symbols':
                m = sym_re.match(line)
                if m:
                    self.symbols.append((int(m.group(1), 16), m.group(2), m.group(3), int(m.group(4), 16),
                                         m.group(5).strip()))
            elif state == 'dis':
                m = ins_re.match(line)
                if m:
                    self._insn(int(m.group(1), 16), m.group(2).split(), m.group(3), section)
            i += 1
        if not self.code_sections:
            raise StackError('no code sections in the listing: is it `objdump -x -D` output?')
        if not self.symbols:
            raise StackError('no symbol table in the listing: is it `objdump -x -D` output?')

    def _insn(self, addr, groups, rest, section):
        size = 0
        for g in groups:
            n = len(g) // 2
            v = int(g, 16)
            if n == 2 or n == 4 or n == 1:
                for k in range(n):
                    self.bytes[addr + size + k] = (v >> (8 * k)) & 0xFF
            else:
                raise StackError('0x%x: cannot read the bytes %r' % (addr, groups))
            size += n
        if section not in self.code_sections:
            return
        rest = rest.strip()
        comment = None
        if '\t;' in rest or rest.startswith(';'):
            rest, comment = rest.split(';', 1)
            rest = rest.rstrip()
        parts = rest.split('\t', 1) if '\t' in rest else rest.split(' ', 1)
        mnem = parts[0].strip()
        ops = parts[1].strip() if len(parts) > 1 else ''
        if mnem.startswith('undefined'):
            ops = ''
        self.insns[addr] = Insn(addr, size, mnem, ops, comment, section)

    def word(self, addr):
        try:
            return sum(self.bytes[addr + k] << (8 * k) for k in range(4))
        except KeyError:
            raise StackError('0x%x: no bytes for a word there in the listing' % addr)

    # --- functions -------------------------------------------------------------------------------
    def _functions(self):
        """Every code symbol is a function start; a sized STT_FUNC ends at its size, anything else
        (an assembler label such as __irq) at the next code symbol."""
        code = [s for s in self.symbols if s[2] in self.code_sections and not s[4].startswith('$')
                and 'd' not in s[1][5:7].lower() and s[1][6] != 'O']
        self.funcs = {}   # start -> (name, end)
        self.by_name = {}
        starts = sorted(set(s[0] for s in code))
        sec_end = {}
        for a in self.order:
            ins = self.insns[a]
            sec_end[ins.section] = max(sec_end.get(ins.section, 0), ins.end)
        for addr, flags, section, size, name in code:
            if flags[6] == 'F' and size:
                end = addr + size
            else:
                later = [s for s in starts if s > addr]
                end = min(later[0] if later else sec_end.get(section, addr), sec_end.get(section, addr))
            if addr in self.funcs:
                # an alias (two names, one address): keep the sized one
                if self.funcs[addr][1] >= end:
                    self.by_name[name] = addr
                    continue
            self.funcs[addr] = (name, end)
            self.by_name[name] = addr
        # Functions whose address is taken - the candidates of any call through a register: a word
        # holding a function's start with the Thumb bit, in a literal pool (a `.word` in code) or
        # anywhere in a data section; and any `tadd rN, pc, #n` (adr) that points at one.
        self.address_taken = set()
        self.fn_words = []   # (address of the word, function)
        data_words = set()
        for a in self.order:
            if self.insns[a].mnem == '.word':
                data_words.add(a)
        code_bytes = set()
        for a in self.order:
            code_bytes.update(range(a, self.insns[a].end))
        for a in sorted(set(b & ~3 for b in self.bytes)):
            if a in code_bytes and a not in data_words:
                continue
            if all((a + k) in self.bytes for k in range(4)):
                v = self.word(a)
                if v & 1 and (v & ~1) in self.funcs:
                    self.address_taken.add(v & ~1)
                    self.fn_words.append((a, v & ~1))
        self._code_literals = data_words
        for a in self.order:
            ins = self.insns[a]
            m = re.match(r'(r\d+), pc, #(\d+)$', ins.ops) if ins.mnem == 'tadd' else None
            if m:
                t = ((a + 4) & ~3) + int(m.group(2))
                if t in self.funcs:
                    self.address_taken.add(t)

    def word_is_code_literal(self, a):
        return a in self._code_literals

    def object_at(self, a):
        for addr, flags, sec, size, nm in self.symbols:
            if addr == a and flags[6] == 'O':
                return nm
        return '0x%x' % a

    def name(self, addr):
        return self.funcs[addr][0] if addr in self.funcs else '0x%x' % addr

    def func_insns(self, start):
        end = self.funcs[start][1]
        out = []
        i = self.index.get(start)
        if i is None:
            raise StackError('%s: no instruction at its start 0x%x' % (self.name(start), start))
        while i < len(self.order) and self.order[i] < end:
            out.append(self.insns[self.order[i]])
            i += 1
        return out


def writes(ins, r):
    """Does this instruction write register r?"""
    if ins.is_data():
        return False
    if ins.mnem in ('tpop',):
        return r in reglist(ins.ops)
    if ins.mnem in ('tloadm', 'tstorem'):
        base = reg(ins.operands()[0].rstrip('!'))
        return base == r or (ins.mnem == 'tloadm' and r in reglist(ins.ops.split(',', 1)[1]))
    if ins.mnem in NO_DEST or ins.mnem.startswith('tj'):
        return False
    ops = ins.operands()
    return bool(ops) and reg(ops[0]) == r


def uses(ins, r):
    """Does this instruction mention register r as a source?"""
    if ins.is_data():
        return False
    toks = [reg(t) for t in re.findall(r'\b(?:r\d+|sp|lr|pc|fp|sl|ip|sb)\b', ins.ops)]
    if ins.mnem in NO_DEST or ins.mnem.startswith('tj'):
        return r in toks
    if ins.mnem in ('tpop', 'tloadm'):
        return False
    srcs = toks[1:]
    # two-operand ALU forms read their destination too (tadds r3, #1; tors r3, r4)
    if toks and toks[0] == r and not ins.mnem.startswith(('tmov', 'tloadr')) and len(ins.operands()) == 2:
        return True
    return r in srcs


class Analysis:
    def __init__(self, listing):
        self.L = listing
        self.frames = {}     # start -> (max offset, [(site, offset, [callees], kind)])
        self.depth = {}
        self.unvisited = {}  # start -> instructions no path from the entry reaches (padding, veneers)
        self.notes = []

    # --- one function ----------------------------------------------------------------------------
    def function(self, start):
        if start in self.frames:
            return self.frames[start]
        L = self.L
        name, end = L.funcs[start]
        body = L.func_insns(start)
        at = {ins.addr: k for k, ins in enumerate(body)}
        targets = set()   # branch targets inside the function: where another path joins
        far = False
        for ins in body:
            if ins.mnem == 'tj' or COND_BRANCH.match(ins.mnem) or ins.mnem == 'tjl':
                t = ins.target()
                if start <= t < end:
                    targets.add(t)
                    if ins.mnem == 'tjl' and t != start and not self._veneer(body, at, t):
                        far = True
        calls = []
        seen = {}
        work = [(start, 0, None)]
        maxoff = 0

        def fail(ins, why):
            raise StackError('%s at 0x%x (%s %s): %s' % (name, ins.addr, ins.mnem, ins.ops, why))

        while work:
            a, off, fpo = work.pop()
            if a in seen:
                if seen[a][0] != off:
                    raise StackError('%s at 0x%x: reached with sp at -%d and at -%d' % (name, a, seen[a][0], off))
                if seen[a][1] != fpo and seen[a][1] is not None:
                    seen[a] = (off, None)
                    work.append((a, off, None))
                continue
            if a not in at:
                raise StackError('%s: control reaches 0x%x, which is not an instruction of it' % (name, a))
            seen[a] = (off, fpo)
            k = at[a]
            ins = body[k]
            nxt = ins.end
            mn, ops = ins.mnem, ins.operands()
            maxoff = max(maxoff, off)
            if ins.is_data():
                fail(ins, 'control runs into data or an undecoded instruction')
            if fpo is not None and writes(ins, 'r7') and not (mn == 'tadd' and ops[1:2] == ['sp']):
                fpo = None
            if mn == 'tpush':
                work.append((nxt, off + 4 * len(reglist(ins.ops)), fpo))
            elif mn == 'tpop':
                regs = reglist(ins.ops)
                off2 = off - 4 * len(regs)
                if off2 < 0:
                    fail(ins, 'pops more than the function pushed')
                if 'r15' in regs:
                    if off2:
                        fail(ins, 'returns with %d bytes still on the stack' % off2)
                else:
                    work.append((nxt, off2, None if 'r7' in regs else fpo))
            elif mn in ('tsub', 'tadd') and ops and reg(ops[0]) == 'r13':
                m = re.match(r'#(\d+)$', ops[1]) if len(ops) == 2 else None
                if not m:
                    fail(ins, 'sp adjusted by a register (alloca or a variable frame)')
                n = int(m.group(1))
                off2 = off + n if mn == 'tsub' else off - n
                if off2 < 0:
                    fail(ins, 'releases more stack than the function took')
                work.append((nxt, off2, fpo))
            elif mn == 'tadd' and len(ops) == 3 and reg(ops[0]) == 'r7' and reg(ops[1]) == 'r13':
                # frame pointer: r7 = sp + n, i.e. the offset off - n
                work.append((nxt, off, off - int(ops[2].lstrip('#'))))
            elif mn == 'tmov' and len(ops) == 2 and reg(ops[0]) == 'r7' and reg(ops[1]) == 'r13':
                work.append((nxt, off, off))
            elif mn == 'tmov' and len(ops) == 2 and reg(ops[0]) == 'r13':
                if reg(ops[1]) == 'r7' and fpo is not None and 0 <= fpo:
                    work.append((nxt, fpo, fpo))
                else:
                    fail(ins, 'sp loaded from a register the analysis cannot follow')
            elif ops and reg(ops[0]) == 'r13' and mn not in NO_DEST and not mn.startswith('tj'):
                fail(ins, 'sp written')
            elif mn == 'tjex':
                if reg(ops[0]) == 'r14':
                    if off:
                        fail(ins, 'returns with %d bytes still on the stack' % off)
                    if far:
                        fail(ins, 'returns through lr in a function that clobbers lr with a far branch')
                else:
                    fail(ins, 'indirect jump through %s' % ops[0])
            elif mn == 'treti':
                pass
            elif mn == 'tmov' and ops and reg(ops[0]) == 'r15':
                for t in self._jump_table(body, at, k, targets, fail):
                    work.append((t, off, fpo))
            elif ops and reg(ops[0]) == 'r15' and mn not in NO_DEST:
                fail(ins, 'pc written')
            elif mn == 'tj' or COND_BRANCH.match(mn):
                t = ins.target()
                if start <= t < end:
                    work.append((t, off, fpo))
                else:
                    if t not in L.funcs:
                        fail(ins, 'branches to 0x%x, inside %s rather than to a function start'
                             % (t, self._owner(t)))
                    calls.append((ins.addr, off, [t], 'tail'))
                if mn != 'tj':
                    work.append((nxt, off, fpo))
            elif mn == 'tjl':
                t = ins.target()
                if t == start:
                    calls.append((ins.addr, off, [t], 'call'))
                    work.append((nxt, off, fpo))
                elif start <= t < end:
                    v = self._veneer(body, at, t)
                    if v:
                        cands, how = self._indirect(body, at, k, v, targets, fail)
                        calls.append((ins.addr, off, cands, how))
                        work.append((nxt, off, fpo))
                    else:
                        work.append((t, off, fpo))     # a far branch: Thumb-1's long jump is a bl
                else:
                    if t not in L.funcs:
                        owner = self._owner(t)
                        v = L.insns.get(t)
                        if v and v.mnem == 'tjex':
                            fail(ins, 'calls through %s (a call-via veneer in %s): an indirect call'
                                 % (v.ops, owner))
                        fail(ins, 'calls 0x%x, inside %s rather than a function start' % (t, owner))
                    calls.append((ins.addr, off, [t], 'call'))
                    work.append((nxt, off, fpo))
            elif mn in ('tjlex', 'tserv'):
                fail(ins, 'not modelled')
            else:
                if mn == 'tmcsr':
                    # div masks interrupts around the hardware divider this way; a mode switch would
                    # change which SP is in use, which nothing reachable does
                    self.notes.append('%s writes the CPSR at 0x%x (taken as not changing the mode)' % (name, ins.addr))
                work.append((nxt, off, fpo))
        self.frames[start] = (maxoff, calls)
        self.unvisited[start] = [i for i in body if i.addr not in seen and not i.is_data()]
        return self.frames[start]

    def _owner(self, a):
        for s, (n, e) in self.L.funcs.items():
            if s <= a < e:
                return n
        return 'no function'

    @staticmethod
    def _veneer(body, at, t):
        """a `tjex rN` that a bl targets: GCC's _call_via_rN, the Thumb-1 indirect call"""
        k = at.get(t)
        if k is None:
            return None
        ins = body[k]
        if ins.mnem == 'tjex' and reg(ins.ops) != 'r14':
            return reg(ins.ops)
        return None

    @staticmethod
    def _def(body, k, r, targets):
        """the instruction that last wrote r before body[k] on the one path into it, or None: back
        through straight-line code and conditional branches (which write nothing and fall through),
        stopping where another path joins or at a call, which clobbers r0-r3"""
        j = k - 1
        while j >= 0:
            if body[j + 1].addr in targets:
                return None
            ins = body[j]
            if ins.is_data() or ins.mnem in ('tj', 'tjl', 'tjex', 'tjlex', 'treti') or (
                    ins.mnem == 'tpop' and 'r15' in reglist(ins.ops)):
                return None
            if writes(ins, r):
                return ins
            j -= 1
        return None

    def _literal_of(self, body, k, r, targets):
        """the constant r holds at body[k] when it was loaded from a literal pool, else None"""
        d = self._def(body, k, r, targets)
        if d is not None and d.mnem == 'tloadr' and re.match(r'r\d+, \[pc, #\d+\]$', d.ops):
            return self.L.word(d.literal())
        return None

    def _indirect(self, body, at, k, r, targets, fail):
        L = self.L
        d = self._def(body, k, r, targets)
        if d is None:
            fail(body[k], 'calls through %s, and what %s holds cannot be found' % (r, r))
        if d.mnem == 'tloadr' and re.match(r'r\d+, \[pc, #\d+\]$', d.ops):
            v = L.word(d.literal())
            if v & 1 and (v & ~1) in L.funcs:
                return [v & ~1], 'call via %s = %s' % (r, L.name(v & ~1))
            fail(body[k], 'calls through %s = 0x%x, which is not a function' % (r, v))
        self._points_to()
        m = re.match(r'(r\d+), \[(r\d+), #(\d+)\]$', d.ops) if d.mnem == 'tloadr' else None
        base = self._literal_of(body, body.index(d), reg(m.group(2)), targets) if m else None
        if base is None:
            cands = sorted(L.address_taken)
            if not cands:
                fail(body[k], 'calls through %s and the image has no address-taken function' % r)
            return cands, 'call via %s: any address-taken function' % r
        var = base + int(m.group(3))
        cands = sorted(self.flows.get(var, set()) | self.escaped)
        nm = L.object_at(var)
        if not cands:
            self.notes.append('%s calls through %s, which nothing ever sets: a null call, not a stack '
                              'question' % (L.name(body[0].addr), nm))
        return cands, 'call via %s = *%s: %s' % (r, nm, ', '.join(L.name(c) for c in cands) or 'never set')

    def _points_to(self):
        """Where each address-taken function's address can end up. A function whose address is
        loaded from a literal and only ever stored, as a value, to a fixed address (`f -> V`) can be
        the target of a call through V and of no other pointer call; one whose address is used in any
        other way - passed, returned, kept in data, stored through a pointer the analysis cannot
        name, or still live where the flow is not followed (a tail call, a jump table) - has escaped
        and is a candidate for every call through a pointer."""
        if hasattr(self, 'flows'):
            return
        L = self.L
        self.flows, self.escaped = {}, set()
        loaded = {}   # literal-pool address -> the instructions loading it
        for a in L.order:
            ins = L.insns[a]
            if ins.mnem == 'tloadr' and re.match(r'r\d+, \[pc, #\d+\]$', ins.ops):
                loaded.setdefault(ins.literal(), []).append(ins)
        for w, f in L.fn_words:
            if L.word_is_code_literal(w) and w in loaded:
                for ins in loaded[w]:
                    if not self._follow(ins, f):
                        self.escaped.add(f)
            else:
                self.escaped.add(f)
        for f in L.address_taken - set(f for w, f in L.fn_words):
            self.escaped.add(f)   # an `adr`: not followed

    def _follow(self, load, f):
        """Follow the register `load` puts f's address in, forward through its function. True if the
        only uses are stores of it to fixed addresses (recorded in self.flows)."""
        L = self.L
        start = max(s for s in L.funcs if s <= load.addr and load.addr < L.funcs[s][1]) \
            if any(s <= load.addr < L.funcs[s][1] for s in L.funcs) else None
        if start is None:
            return False
        body = L.func_insns(start)
        at = {ins.addr: k for k, ins in enumerate(body)}
        end = L.funcs[start][1]
        targets = set()
        for ins in body:
            if ins.mnem in ('tj', 'tjl') or COND_BRANCH.match(ins.mnem):
                t = ins.target()
                if start <= t < end:
                    targets.add(t)
        r = reg(load.operands()[0])
        work, seen = [at[load.addr] + 1], set()
        while work:
            j = work.pop()
            if j in seen:
                continue
            seen.add(j)
            if j >= len(body):
                return False
            ins = body[j]
            if ins.is_data():
                return False
            mn = ins.mnem
            if uses(ins, r):
                m = re.match(r'(r\d+), \[(r\d+), #(\d+)\]$', ins.ops)
                if mn != 'tstorer' or not m or reg(m.group(1)) != r or reg(m.group(2)) == r:
                    return False
                base = self._literal_of(body, j, reg(m.group(2)), targets)
                if base is None:
                    return False
                self.flows.setdefault(base + int(m.group(3)), set()).add(f)
            if writes(ins, r):
                continue
            nxt = j + 1
            if mn == 'tjl':
                t = ins.target()
                if start <= t < end and not self._veneer(body, at, t):
                    work.append(at[t]) if t in at else None
                    continue
                if r in ('r0', 'r1', 'r2', 'r3'):
                    return False      # it could be an argument
                work.append(nxt)
            elif mn == 'tj' or COND_BRANCH.match(mn):
                t = ins.target()
                if not (start <= t < end) or t not in at:
                    return False
                work.append(at[t])
                if mn != 'tj':
                    work.append(nxt)
            elif (mn == 'tjex' and reg(ins.ops) == 'r14') or mn == 'treti' or (
                    mn == 'tpop' and 'r15' in reglist(ins.ops)):
                if r in ('r0', 'r1'):
                    return False      # returned
            elif mn in ('tjex', 'tjlex', 'tserv') or (ins.operands() and reg(ins.operands()[0]) == 'r15'):
                return False
            else:
                work.append(nxt)
        return True

    def _jump_table(self, body, at, k, targets, fail):
        """`tshftls rI, rI, #2; tloadr rB, [pc..]; tloadr rT, [rB, rI]; tmov pc, rT`, guarded by
        `tcmp rI, #K` and an unsigned branch: K + 1 entries, each an instruction of this function."""
        L = self.L
        ins = body[k]
        rt = reg(ins.operands()[1])
        if k < 3:
            fail(ins, 'indirect jump that is not a jump table')
        ld, lit, sh = body[k - 1], body[k - 2], body[k - 3]
        m = re.match(r'(r\d+), \[(r\d+), (r\d+)\]$', ld.ops) if ld.mnem == 'tloadr' else None
        lm = re.match(r'(r\d+), \[pc, #\d+\]$', lit.ops) if lit.mnem == 'tloadr' else None
        sm = re.match(r'(r\d+), (r\d+), #2$', sh.ops) if sh.mnem == 'tshftls' else None
        if not (m and lm and sm and reg(m.group(1)) == rt):
            fail(ins, 'indirect jump that is not a jump table')
        rb = reg(lm.group(1))
        pair = {reg(m.group(2)), reg(m.group(3))}
        ri = reg(sm.group(1))
        if rb not in pair or ri not in pair or rb == ri:
            fail(ins, 'indirect jump that is not a jump table')
        rs = reg(sm.group(2))
        table = L.word(lit.literal())
        d = sh.addr
        if any(t in targets for t in (lit.addr, ld.addr, ins.addr)):
            fail(ins, 'a jump table whose dispatch has another way in')

        def cmp_k(j):
            c = body[j] if j >= 0 else None
            cm = re.match(r'(r\d+), #(\d+)$', c.ops) if c is not None and c.mnem == 'tcmp' else None
            if not cm or reg(cm.group(1)) != rs:
                fail(ins, 'jump table at 0x%x with no bound on %s' % (table, rs))
            return int(cm.group(2))

        bounds = []
        kd = at[d]
        prev = body[kd - 1] if kd else None
        far = prev is not None and prev.mnem == 'tjl' and body[0].addr <= prev.target() < body[-1].end \
            and not self._veneer(body, at, prev.target())   # a far branch: no fall-through either
        ends = prev is None or prev.is_data() or far or prev.mnem in ('tj', 'tjex', 'treti') or (
            prev.mnem == 'tpop' and 'r15' in reglist(prev.ops)) or (
            prev.mnem == 'tmov' and reg(prev.operands()[0]) == 'r15')   # another table's dispatch
        if not ends:
            if prev.mnem != 'tjhi':
                fail(ins, 'jump table entered without a bound check')
            bounds.append(cmp_k(kd - 2))
        for j, b in enumerate(body):
            if (b.mnem == 'tj' or COND_BRANCH.match(b.mnem) or b.mnem == 'tjl') and b.target() == d:
                if b.mnem == 'tjls':
                    bounds.append(cmp_k(j - 1))
                elif b.mnem == 'tj' and j >= 1 and body[j - 1].mnem == 'tjhi':
                    bounds.append(cmp_k(j - 2))
                else:
                    fail(ins, 'jump table entered from 0x%x without a bound check' % b.addr)
        if not bounds:
            fail(ins, 'jump table at 0x%x is never entered with a bound' % table)
        n = max(bounds) + 1
        out = []
        for e in range(n):
            t = L.word(table + 4 * e) & ~1
            if t not in at:
                fail(ins, 'jump table entry %d (0x%x) is not an instruction of this function' % (e, t))
            out.append(t)
            targets.add(t)
        return out

    # --- the call graph --------------------------------------------------------------------------
    def bound(self, root_name):
        L = self.L
        if root_name not in L.by_name:
            raise StackError('no function %s in the image' % root_name)
        root = L.by_name[root_name]
        state = {}
        stack = []

        def visit(f):
            if state.get(f) == 'done':
                return self.depth[f]
            if state.get(f) == 'open':
                cyc = stack[stack.index(f):] + [f]
                raise StackError('recursion: ' + ' -> '.join(L.name(x) for x in cyc))
            state[f] = 'open'
            stack.append(f)
            maxoff, calls = self.function(f)
            best = (maxoff, None, None, None)
            for site, off, cands, how in calls:
                for c in cands:
                    d = off + visit(c)[0]
                    if d > best[0]:
                        best = (d, c, site, (off, how))
            stack.pop()
            state[f] = 'done'
            self.depth[f] = best
            return best

        total = visit(root)[0]
        path, f = [], root
        while f is not None:
            d, c, site, info = self.depth[f]
            path.append((f, self.frames[f][0], site, info))
            f = c
        return total, path

    def report(self, root_name, title):
        total, path = self.bound(root_name)
        lines = ['%s: %d bytes, deepest path:' % (title, total)]
        for f, frame, site, info in path:
            if site is None:
                lines.append('  %-28s frame %4d' % (self.L.name(f), frame))
            else:
                off, how = info
                lines.append('  %-28s frame %4d, calls at -%d (0x%x%s)' % (
                    self.L.name(f), frame, off, site, '' if how in ('call', 'tail') else ', ' + how))
        return total, '\n'.join(lines)


def required(bound, margin, align):
    need = -(-bound * margin.numerator // margin.denominator)
    return -(-need // align) * align


def main(argv):
    import argparse
    p = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    p.add_argument('listing')
    p.add_argument('--root', default='main', help='the main stack\'s root (main)')
    p.add_argument('--irq', default='__irq', help='the interrupt stack\'s root (__irq)')
    p.add_argument('--margin', default='4/3', help='size = bound x margin (4/3)')
    p.add_argument('--align', type=int, default=16, help='sizes rounded up to this (16)')
    p.add_argument('--defsym', help='write the link\'s --defsym options to this file')
    p.add_argument('--verify', action='store_true',
                   help='fail unless the image reserves at least the sizes it needs')
    p.add_argument('--quiet', action='store_true', help='no paths, just the outcome')
    a = p.parse_args(argv)
    margin = Fraction(a.margin)
    try:
        with open(a.listing) as f:
            L = Listing(f.read())
        an = Analysis(L)
        main_b, main_txt = an.report(a.root, 'main stack (from %s)' % a.root)
        irq_b, irq_txt = an.report(a.irq, 'IRQ stack (from %s)' % a.irq)
    except StackError as e:
        print('stackdepth: %s: %s' % (a.listing, e), file=sys.stderr)
        print('stackdepth: no stack bound - fix the code or the analysis; the stacks are not sized from a guess',
              file=sys.stderr)
        return 1
    main_n, irq_n = required(main_b, margin, a.align), required(irq_b, margin, a.align)
    if not a.quiet:
        print(main_txt)
        print(irq_txt)
        for n in sorted(set(an.notes)):
            print('  note: ' + n)
        print('sizes at %s x the bound, rounded to %d: main stack %d, IRQ stack %d'
              % (a.margin, a.align, main_n, irq_n))
    if a.defsym:
        with open(a.defsym, 'w') as f:
            f.write('--defsym=__STACK_MIN__=%d --defsym=__IRQ_STACK__=%d '
                    '--defsym=__STACK_BOUND__=%d --defsym=__IRQ_STACK_BOUND__=%d\n' % (main_n, irq_n, main_b, irq_b))
    if a.verify:
        syms = {s[4]: s[0] for s in L.symbols}
        try:
            have_main = syms['_stack_min_']
            have_irq = syms['_irq_stk_hi_'] - syms['_irq_stk_lo_']
        except KeyError as e:
            print('stackdepth: %s: no symbol %s to verify against' % (a.listing, e), file=sys.stderr)
            return 1
        bad = []
        if have_main < main_n:
            bad.append('main stack: the image reserves %d, it needs %d (bound %d)' % (have_main, main_n, main_b))
        if have_irq < irq_n:
            bad.append('IRQ stack: the image reserves %d, it needs %d (bound %d)' % (have_irq, irq_n, irq_b))
        for k, b in (('_stack_bound_', main_b), ('_irq_stk_bound_', irq_b)):
            if syms.get(k) and syms[k] != b:
                print('stackdepth: note: %s was %d when the stacks were sized, %d in this image' % (k, syms[k], b))
        if bad:
            for b in bad:
                print('stackdepth: %s: %s' % (a.listing, b), file=sys.stderr)
            return 1
        print('STACK verified: main %d bytes for a bound of %d, IRQ %d for %d'
              % (have_main, main_b, have_irq, irq_b))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
