#!/usr/bin/env python3
"""Tests of tools/stackdepth.py on small listings written here, in the shape `tc32-elf-objdump -x -D`
prints: a known depth through pushes, sp adjustments, branches and a jump table, and every case the
tool must refuse rather than under-count - recursion, an indirect call or jump it cannot bound, sp
moved by a register, a jump table without its bound, control running into data.

    make -C test        # with the rest
"""
import contextlib
import io
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import stackdepth  # noqa: E402


def listing(funcs, data=(), objects=()):
    """funcs: [(name, addr, [(mnemonic, operands[, comment]), ...])], every instruction 2 bytes but
    `tjl` (4) and `.word` (4); data: [(addr, word)] in .rodata; objects: [(name, addr)] in .bss"""
    syms, dis = [], []
    end = 0
    for name, addr, body in funcs:
        a = addr
        lines = ['', '%08x <%s>:' % (addr, name)]
        for ins in body:
            mn, ops = ins[0], ins[1]
            comment = ins[2] if len(ins) > 2 else None
            if mn == '.word':
                hexcol = '%08x ' % int(ops, 0)
                size = 4
            elif mn == 'tjl':
                hexcol, size = '9000 9000 ', 4
            else:
                hexcol, size = '0000      ', 2
            line = '%8x:\t%s\t%s\t%s' % (a, hexcol, mn, ops)
            if comment:
                line += '\t; ' + comment
            lines.append(line)
            a += size
        syms.append('%08x g     F .text\t%08x %s' % (addr, a - addr, name))
        dis += lines
        end = max(end, a)
    rodata = ['', '00009000 <table>:'] + ['%8x:\t%08x \t.word\t0x%08x' % (a, w, w) for a, w in data]
    for name, addr in objects:
        syms.append('%08x g     O .bss\t00000004 %s' % (addr, name))
    return '\n'.join([
        'x.elf:     file format elf32-littletc32', '',
        'Sections:',
        'Idx Name          Size      VMA       LMA       File off  Algn',
        '  0 .text         %08x  00000100  00000100  00008000  2**4' % (end - 0x100),
        '                  CONTENTS, ALLOC, LOAD, READONLY, CODE',
        '  1 .rodata       00000100  00009000  00009000  00011000  2**2',
        '                  CONTENTS, ALLOC, LOAD, READONLY, DATA',
        'SYMBOL TABLE:'] + syms + ['', '', 'Disassembly of section .text:'] + dis +
        ['', 'Disassembly of section .rodata:'] + rodata) + '\n'


IRQ = ('__irq', 0x800, [('tpush', '{r0, r1, r2, r3, lr}'), ('tjl', '880 <irqleaf>'),
                        ('tpop', '{r0, r1, r2, r3}'), ('treti', '{pc}')])
IRQLEAF = ('irqleaf', 0x880, [('tpush', '{r4, lr}'), ('tsub', 'sp, #8'), ('tadd', 'sp, #8'),
                              ('tpop', '{r4, pc}')])
# main: 8 + 16, then f (16), then g (4 + 8): 52, and through a conditional branch that skips the call
MAIN = ('main', 0x100, [('tpush', '{r4, lr}'), ('tsub', 'sp, #16'), ('tcmp', 'r0, #0'),
                        ('tjeq', '10c <main+0xc>'), ('tjl', '200 <f>'), ('tadd', 'sp, #16'),
                        ('tpop', '{r4, pc}')])
F = ('f', 0x200, [('tpush', '{r4, r5, r6, lr}'), ('tjl', '300 <g>'), ('tpop', '{r4, r5, r6, pc}')])
G = ('g', 0x300, [('tpush', '{lr}'), ('tsub', 'sp, #8'), ('tadd', 'sp, #8'), ('tpop', '{pc}')])


def run(text, *args):
    with tempfile.TemporaryDirectory() as d:
        p = os.path.join(d, 'x.lst')
        with open(p, 'w') as f:
            f.write(text)
        out, err = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(err):
            rc = stackdepth.main([p] + list(args))
        return rc, out.getvalue(), err.getvalue()


TESTS = []


def test(fn):
    TESTS.append(fn)
    return fn


@test
def a_known_depth_and_the_sizes_from_it():
    rc, out, err = run(listing([MAIN, F, G, IRQ, IRQLEAF]))
    assert rc == 0, err
    assert 'main stack (from main): 52 bytes' in out, out
    assert 'IRQ stack (from __irq): 36 bytes' in out, out  # 20 + 8 + 8
    # 52 * 4/3 = 69.3 -> 70 -> 80 at 16; 36 * 4/3 = 48
    assert 'main stack 80, IRQ stack 48' in out, out


@test
def the_deeper_of_two_callees_counts_at_its_own_call_site():
    # h pushes 8 before calling g and 40 before calling f: 40 + 16 + 12, not 8 + ...
    h = ('main', 0x100, [('tpush', '{r4, lr}'), ('tjl', '300 <g>'), ('tpush', '{r0, r1, r2, r3, r4, r5, r6, r7}'),
                         ('tjl', '200 <f>'), ('tpop', '{r0, r1, r2, r3, r4, r5, r6, r7}'), ('tpop', '{r4, pc}')])
    rc, out, err = run(listing([h, F, G, IRQ, IRQLEAF]))
    assert rc == 0 and 'main stack (from main): 68 bytes' in out, (out, err)


@test
def a_bounded_jump_table_reaches_every_entry():
    # main: cmp r0,#1; bls dispatch; ...; dispatch: two entries, the second calls g
    body = [('tpush', '{lr}'),                                  # 100
            ('tcmp', 'r0, #1'),                                 # 102
            ('tjls', '108 <main+0x8>'),                         # 104
            ('tpop', '{pc}'),                                   # 106
            ('tshftls', 'r0, r0, #2'),                          # 108
            ('tloadr', 'r3, [pc, #12]', '(118 <main+0x18>)'),  # 10a
            ('tloadr', 'r3, [r3, r0]'),                         # 10c
            ('tmov', 'pc, r3'),                                 # 10e
            ('tpop', '{pc}'),                                   # 110: entry 0
            ('tjl', '300 <g>'),                                 # 112: entry 1
            ('tpop', '{pc}'),                                   # 116
            ('.word', '0x9000')]                                # 118
    rc, out, err = run(listing([('main', 0x100, body), G, IRQ, IRQLEAF], data=[(0x9000, 0x110), (0x9004, 0x112)]))
    assert rc == 0 and 'main stack (from main): 16 bytes' in out, (out, err)
    # ... and without its bound it is refused
    body[2] = ('tj', '108 <main+0x8>')
    rc, out, err = run(listing([('main', 0x100, body), G, IRQ, IRQLEAF], data=[(0x9000, 0x110), (0x9004, 0x112)]))
    assert rc == 1 and 'without a bound check' in err, (out, err)


@test
def a_call_through_a_pointer_set_from_constants_costs_its_targets():
    # init stores g's address into V; main calls through V via the call-via-r3 veneer at its end
    init = ('init', 0x400, [('tloadr', 'r2, [pc, #4]', '(408 <init+0x8>)'),
                            ('tloadr', 'r3, [pc, #8]', '(40c <init+0xc>)'),
                            ('tstorer', 'r2, [r3, #0]'), ('tjex', 'lr'),
                            ('.word', '0x301'), ('.word', '0x00841000')])
    main = ('main', 0x100, [('tpush', '{r4, lr}'), ('tjl', '400 <init>'),
                            ('tloadr', 'r3, [pc, #8]', '(110 <main+0x10>)'), ('tloadr', 'r3, [r3, #0]'),
                            ('tjl', '114 <main+0x14>'), ('tpop', '{r4, pc}'),
                            ('.word', '0x00841000'), ('tjex', 'r3'), ('tnop', '')])
    rc, out, err = run(listing([main, init, G, IRQ, IRQLEAF], objects=[('V', 0x841000)]))
    assert rc == 0 and 'main stack (from main): 20 bytes' in out, (out, err)  # 8 + g's 12
    assert '*V: g' in out, out


@test
def recursion_is_refused():
    g = ('g', 0x300, [('tpush', '{lr}'), ('tjl', '200 <f>'), ('tpop', '{pc}')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 1 and 'recursion: ' in err and 'f -> g -> f' in err, err


@test
def an_indirect_call_it_cannot_bound_is_refused():
    # r3 comes from a register nobody can name, and no function's address is taken
    f = ('f', 0x200, [('tpush', '{r4, lr}'), ('tloadr', 'r3, [r4, #0]'), ('tjl', '20a <f+0xa>'),
                      ('tpop', '{r4, pc}'), ('tjex', 'r3'), ('tnop', '')])
    rc, out, err = run(listing([MAIN, f, G, IRQ, IRQLEAF]))
    assert rc == 1 and 'no address-taken function' in err, err
    # ... and an indirect jump in the flow, which is not a return
    f = ('f', 0x200, [('tpush', '{r4, lr}'), ('tpop', '{r4}'), ('tpop', '{r3}'), ('tjex', 'r3')])
    rc, out, err = run(listing([MAIN, f, G, IRQ, IRQLEAF]))
    assert rc == 1 and 'indirect jump' in err, err


@test
def sp_moved_by_a_register_is_refused():
    g = ('g', 0x300, [('tpush', '{lr}'), ('tmov', 'sp, r3'), ('tpop', '{pc}')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 1 and 'sp loaded from a register' in err, err
    # ... but a frame pointer put back where it was taken is followed
    g = ('g', 0x300, [('tpush', '{r7, lr}'), ('tsub', 'sp, #4'), ('tadd', 'r7, sp, #0'), ('tmov', 'sp, r7'),
                      ('tadd', 'sp, #4'), ('tpop', '{r7, pc}')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 0 and 'main stack (from main): 52 bytes' in out, (out, err)


@test
def control_running_into_data_or_unbalanced_returns_are_refused():
    g = ('g', 0x300, [('tpush', '{lr}'), ('tcmp', 'r0, #0'), ('.word', '0x12345678')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 1 and 'runs into data' in err, err
    g = ('g', 0x300, [('tpush', '{r4, lr}'), ('tjex', 'lr')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 1 and 'still on the stack' in err, err
    g = ('g', 0x300, [('tpush', '{lr}'), ('tj', '1000 <elsewhere>')])
    rc, out, err = run(listing([MAIN, F, g, IRQ, IRQLEAF]))
    assert rc == 1 and 'rather than to a function start' in err, err


@test
def verify_refuses_an_image_that_reserves_less():
    text = listing([MAIN, F, G, IRQ, IRQLEAF])
    abs_syms = ('00000050 g       *ABS*\t00000000 _stack_min_\n'
                '00841000 g       .bss\t00000000 _irq_stk_lo_\n'
                '00841030 g       .bss\t00000000 _irq_stk_hi_\n')
    text = text.replace('SYMBOL TABLE:\n', 'SYMBOL TABLE:\n' + abs_syms)
    rc, out, err = run(text, '--verify', '--quiet')  # 80 and 48: exactly enough
    assert rc == 0 and 'STACK verified: main 80' in out, (out, err)
    rc, out, err = run(text.replace('00000050 g       *ABS*', '0000004c g       *ABS*'), '--verify')
    assert rc == 1 and 'reserves 76, it needs 80' in err, (out, err)


def main():
    failed = 0
    for t in TESTS:
        try:
            t()
            print('ok   %s' % t.__name__)
        except Exception as e:  # noqa: BLE001
            failed += 1
            print('FAIL %s: %r' % (t.__name__, e))
    print('%d of %d passed' % (len(TESTS) - failed, len(TESTS)))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
