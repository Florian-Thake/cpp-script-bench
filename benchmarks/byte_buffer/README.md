# byte_buffer

**Group:** data. **Measures:** writes and reads of single bytes by index in a byte buffer, the
form of binary data in the script (e.g. a packet or a file in memory).

## Task

`bench( n )` creates a buffer of n zero bytes, fills it with the low bytes of the generator of
`sort`, and computes an Adler-32 style checksum over it:

```
buf = n zero bytes
x = 1
for i = 0 .. n-1:
    x = (x * 75 + 74) % 65537
    buf[i] = x & 255
a = 1
b = 0
for i = 0 .. n-1:
    a = (a + buf[i]) % 65521
    b = (b + a) % 65521
return a + b
```

- Default `n`: 100,000. The checksum is a + b (106,756 for n = 100,000).
- `a` and `b` stay below 65,521, so all values fit into 32 bit signed integers for every n.
- Not the same as `buffer` in the interop group (Phase 4c), where the host owns the buffer.

## Rules

- The buffer is the language's type for bytes, created at its full size: unlike the arrays of
  `sieve` and `sort`, a byte buffer usually has a fixed size. Where the language has no byte type,
  its natural array is used instead (see Lua and ChaiScript), built by appending in the write loop
  with a capacity reserved where possible, and the values are masked to 0..255 as in the spec.
- One byte per access, by index. No multi-byte access (e.g. TeaScript's `_buf_get_u32`), which only
  some languages can do in place; it would fit the idiomatic track (PLAN.md, section 7).
- Both loops are the language's natural counting loop, also for the reads: the benchmark measures
  index reads in every language, not element iteration (see Python).
- Plain assignments, no compound assignment. All variables are local variables of `bench`.

## Notes per language

**TeaScript**
- A Buffer: `_buf( n )` creates an empty Buffer with the capacity n, `_buf_resize( buf, n )` makes it
  n zero bytes. A Buffer can't grow past its capacity.
- Reads and writes use the subscript operator `buf[i]`. A Buffer only accepts `u8` values, so the
  write converts with `as u8`.

**ChaiScript**
- ChaiScript has no byte type. The buffer is a Vector of `int` values in 0..255, with `reserve( n )`,
  built with `push_back` in the write loop.
- The size n = 100,000 is limited by ChaiScript: its headline needs ~0.47 s per call with MSVC.

**AngelScript**
- `array<uint8>( n )` creates n zero bytes. The write converts explicitly with `uint8( x & 255 )`.

**Lua**
- Lua has no byte type (its strings are immutable). The buffer is a table of integers in 0..255,
  created with `table.create( n )` and filled with `buf[i] = x & 255` in the write loop.
- Lua indexes from 1: the byte with the index i of the spec has the index i + 1. Both loops run over
  1 .. n.

**Python**
- `bytearray( n )` creates n zero bytes; `buf[i] = x & 255` writes a byte, `buf[i]` reads one as an
  `int`.
- The read loop uses `buf[i]` like the other languages. The idiomatic Python form for a pass over all
  bytes would be `for byte in buf:`, ~9% faster for the whole call with Python 3.14 (MSVC). It is not
  used, because the other languages would have to switch to element iteration as well, and the
  benchmark measures access by index.
