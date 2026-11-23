# buffer

**Group:** interop. **Measures:** writing into memory owned by the host: an RGBA image of 32 bit
pixels, filled pixel by pixel by the script (from the BufferOverhead test of the old benchmark
project).

## Task

The host owns an image of n x n pixels of 32 bit (`host.hpp`). Before every call of `bench( n )` it
makes the image n x n pixels, all 0. The script writes every pixel, then the host computes its
checksum over the image:

```
for y = 0 .. n-1:
    for x = 0 .. n-1:
        image[y * n + x] = (x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536
return n * n                                   (the number of pixels, checked by the host)

checksum = sum of image[i] * ((i & 7) + 1)     (C++, 64 bit)
```

- Default `n`: 400 (160,000 pixels). The checksum is 6,097,724,102,400 for n = 400; the host computes
  it in 64 bit. The values in the script fit into 32 bit signed integers (pixels below 2^24, indices
  below n * n).
- The weight `(i & 7) + 1` makes a pixel at a wrong place change the checksum.
- Resizing and clearing the image and the checksum are C++ code of the host in the measured call,
  the same for every language (~60 us with MSVC, the C++ reference including its own fill).

## Rules

- The image is memory of the host, not a container of the script. The script writes into it through
  the binding, pixel by pixel, in the language's natural form for host memory (below).
- No bulk operations (no fill of a whole row or the whole image in one call).
- Use the language's natural counting loops, like in `var_nested`.

## Access per language

| Language    | Form                                       |
|-------------|--------------------------------------------|
| TeaScript   | `_buf_set_u32( image, (y * n + x) * 4, v )` on a Buffer created by the host (a Buffer cannot refer to memory of the host); the host resizes it before every call and reads the pixels from it |
| ChaiScript  | `image.set( y * n + x, v )`: the image of the host as a user type, bound by reference, with a C++ function `set` |
| AngelScript | `image[y * n + x] = v`: a registered type (`asOBJ_REF`, `asOBJ_NOCOUNT`) with `uint &opIndex( uint )`, a global property |
| Lua         | `image:set( y * n + x + 1, v )`: LuaBridge3 class with the method `set`; `c-api`: a userdata whose metatable's `__index` holds `set` |
| Python      | `buf = memoryview( image )` once per call, then `buf[y * n + x] = v`: the image is an object with the buffer protocol (pybind11 `def_buffer`; `c-api`: the slot `Py_bf_getbuffer`), so the memoryview (format `I`) writes directly into the memory of the host |

The design round planned the index operator for ChaiScript, AngelScript, Lua and Python. A method
`set( index, value )` was measured as the alternative (n = 256), and the faster form is used where it
is faster with both toolchains:

| Language    | MSVC: `image[i] = v` | MSVC: `set( i, v )` | GCC 11: `image[i] = v` | GCC 11: `set( i, v )` | Used |
|-------------|----------------------|---------------------|------------------------|-----------------------|------|
| ChaiScript `no-threads` | 137-143 ms (`vector_type`, `[]`) | 121 ms | 63.5 ms | 47 ms | `set` |
| AngelScript `no-threads-no-line-cues` | 1.74 ms | 2.85 ms | 1.25 ms | 3.95 ms | `[]` |
| Lua (LuaBridge3) | 31 ms (`addNewIndexMetaMethod`) | 9.7 ms | 24.6 ms | 7.5 ms | `set` |
| Lua `c-api` | 3.8 ms (`__newindex`) | 4.0 ms | 3.4 ms | 3.4 ms | (the script of the headline) |

- Python keeps the memoryview, Python's form for writing into a buffer of the host. A method would
  cost at least the call of a bound function (~160 ns with pybind11, see `script_to_host`), more than
  a whole iteration of the memoryview loop (~115 ns with MSVC).
- LuaBridge3's `__newindex` converts the key with `lua_tostring()` before it calls the fallback of the
  class, so an integer key arrives as a numeric string; that and the `LuaRef` arguments make the
  index form slow.

## Notes per language

- C++: the reference fills the image of `host.hpp` directly.
- TeaScript: the pixel value is a `u64`, the type which `_buf_set_u32` takes. An operation with a `u64`
  operand gives a `u64`, so the script uses `u64` literals (`x bit_and 255u64`) and needs no cast per
  pixel (~7-8% faster than `as u64`; PLAN.md, decision log 2026-10-25). The loop counters are `i64`
  (`_seq` takes only `i64`). The position is in bytes: `(y * n + x) * 4`.
- ChaiScript: no `return`, see `fib_recursive/README.md`. The image is a global: top-level variables
  are not visible in functions.
- AngelScript: the index operator checks the range and raises a script exception, like the array
  add-on.
- Lua: the index runs from 1, like in a Lua array: `y * n + x + 1`.
- Python: the memoryview is taken in `bench`, because the host resizes the image before every call.

## Results

The measured numbers of every language and variant, with both toolchains, are in the result sets:
[results/](../../results/README.md).
The `c-api` variants show the cost of the binding libraries.
