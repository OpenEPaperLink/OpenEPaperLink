import ctypes

"""
helpers to interact with c-struct based data, which we transfer with the AP
"""


def c_format(value):
    """
    internal formatting helper for ctypes-values.
    most importantly renders array-contents as lists or hex (if the data is in bytes)
    """
    if isinstance(value, ctypes.Array):
        if value._type_ == ctypes.c_byte or value._type_ == ctypes.c_ubyte:
            return bytes(value).hex()
        else:
            return [str(v) for v in value]
    else:
        return str(value)


def c_pretty(inst):
    """
    decorator for ctypes.Structure classes, which adds automatic pretty-printing
    """
    def __repr__(self) -> str:
        values = ", ".join(f"{name}={c_format(value)}" for name, value in self._asdict().items())
        return f"<{self.__class__.__name__}: {values}>"

    def _asdict(self) -> dict:
        return {field[0]: getattr(self, field[0]) for field in self._fields_}

    inst.__repr__ = __repr__
    inst._asdict = _asdict
    return inst


def hex_reverse_bytes(val, delimiter=":"):
    """
    reverses (by copy) the given bytearray and encodes it as colon-delimited hex.
    this is mostly used to pretty-print tag MAC addresses, which are transferred in reverse
    """
    reversed = bytes(val)[::-1]
    return reversed.hex() if delimiter is None else reversed.hex(delimiter)


def hex_bytes(val, delimiter=None):
    """
    encodes the given bytes / bytes-convertable into a (optionally delimited) hex-string
    """
    return bytes(val).hex() if delimiter is None else bytes(val).hex(delimiter)


def try_decode(buffer, prefix, typ):
    """
    tries to decode the given c-struct (typ) from the given buffer,
    if the buffer starts with the given prefix.
    also returns an indicator, whether the buffer might be decodable to this
    type, if the buffer gets more contents / longer.
    """
    if len(buffer) < len(prefix):
        may_match = buffer == prefix[:len(buffer)]
        return may_match, None
    may_match = buffer[:len(prefix)] == prefix
    if may_match and len(buffer) >= len(prefix) + ctypes.sizeof(typ):
        val = typ.from_buffer_copy(buffer, len(prefix))
        buffer[:ctypes.sizeof(typ) + len(prefix)] = b''
        return True, val
    return may_match, None
