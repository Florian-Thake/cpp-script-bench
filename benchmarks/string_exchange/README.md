# string_exchange

**Group:** interop. **Measures:** strings crossing the boundary in both directions: a C++ string as
the argument of a script function, the script's string as the result, converted back to a C++ string.

## Task

The script defines `f( s )`, which returns its argument. The host calls it in a C++ loop (`host.hpp`,
the same for every runner and the C++ reference):

```
f( s ) = s                                         (script)

sum = 0
for i = 0 .. n-1:                                  (C++)
    s = "item" + i                                 (std::string)
    r = f( s )                                     (converted to the script's string and back)
    sum = sum + length( r ) + last digit of r
return sum
```

- Default `n`: 250,000. The checksum is 3,513,890 for n = 250,000.
- The last digit makes a result of the same length but with another content change the checksum.
- The script has no `bench`. The measured unit (one call of `call_bench( n )`) is the host's loop.
- Building `"item" + i` is C++ code in the measured loop, the same for every language (~5 ms with
  MSVC, most of the C++ reference).

## Rules

- `f` returns its argument unchanged. The conversions between the C++ string and the script's string
  type are the work: each language's normal way to pass a string to a script function and to get a
  string back, through the function handle looked up once (like `host_to_script`).
- The host converts the result into a `std::string` before it uses it.

## Call path per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `CallFuncEx( "f", s )` with the `std::string`, the result with `GetAsString()` | |
| ChaiScript  | `std::function<std::string( std::string const & )>` from `eval( "f" )` | |
| AngelScript | `string f( const string &in s )`: `SetArgObject` with the `std::string`, the result from `GetReturnObject()` | |
| Lua         | LuaBridge3: `LuaRef::call<std::string>( s )` | `c-api`: a registry reference, `lua_pushlstring`, `lua_pcall`, `lua_tolstring` |
| Python      | pybind11: `f( s ).cast<std::string>()`    | `c-api`: `PyUnicode_FromStringAndSize`, `PyObject_CallOneArg`, `PyUnicode_AsUTF8AndSize` |

## Notes per language

- C++: the reference calls a normal C++ function, which returns a copy of its argument.
- TeaScript: like `host_to_script`, the function is called by its name (`CallFuncEx`).
- ChaiScript: no `return`, see `fib_recursive/README.md`.
- AngelScript: `const string &in` is AngelScript's usual form of a string parameter; the context keeps
  the returned string until the next call, the host copies it.
- Lua: `f` is global, so the host can find it. Lua strings are interned: every new string is hashed
  and looked up when it is pushed.
- Python: the C++ string is UTF-8, a Python `str` is converted from and to UTF-8 (the strings are
  ASCII).

## Measurements (per call of the host's loop, n = 250,000)

| Language    | MSVC: headline | MSVC: `c-api` | GCC 11: headline | GCC 11: `c-api` |
|-------------|----------------|---------------|------------------|-----------------|
| TeaScript `vm-O1` | ~155 ms  |               | ~103 ms          |                 |
| ChaiScript `no-threads` | ~77 ms |          | ~40 ms           |                 |
| AngelScript `no-threads-no-line-cues` | ~21.5 ms | | ~27 ms       |                 |
| Lua         | ~36 ms (LuaBridge3) | ~30 ms   | ~25 ms (LuaBridge3) | ~21 ms      |
| Python      | ~20 ms (pybind11) | ~16 ms     | ~21 ms (pybind11) | ~16.5 ms      |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
