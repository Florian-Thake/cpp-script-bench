# array_exchange

**Group:** interop. **Measures:** arrays crossing the boundary in both directions: a C++ array
converted into the script's array, passed to a script function, its result converted back into a
C++ array. The series shows the cost per call against the cost per element.

## Task

The script defines `f( a )`, which returns a new array with every element + 1. The host calls it in a
C++ loop (`host.hpp`, the same for every runner and the C++ reference):

```
f( a ):                                            (script)
    b = new array, with the capacity of a where the language can reserve
    for i = 0 .. size(a)-1:
        append a[i] + 1 to b
    return b

a = std::vector<int> of n elements, a[i] = i & 7   (C++)
sum = 0
repeat 40,000 / n times:
    b = f( a )                                     (a converted into the script's array, b back into a std::vector<int>)
    sum = sum + all elements of b
return sum
```

- Series n = 10 / 1,000 / 40,000: 4,000 / 40 / 1 round trips, so every call moves 40,000 elements
  in each direction. The checksums are 156,000 / 180,000 / 180,000.
- The script has no `bench`. The measured unit (one call of `call_bench( n )`) is the host's loop.

## Rules

- The arrays are the natural containers of the languages, the same as in the Data benchmarks:
  TeaScript Tuple, ChaiScript Vector, AngelScript `array<int>`, Lua table as a sequence, Python list.
- The host converts with the language's normal API in both directions (below). The script builds the
  result by appending in a loop, with a reserved capacity where the language has one (PLAN.md,
  decision log 2026-10-18), and reads `a` by index, like `byte_buffer` and `maps`.

## Conversion per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | a `Tuple` with `Reserve` and `AppendValue` (shared mutable values, like `_tuple_append`), `CallFuncEx( "f", tuple )`; the result with `GetValueByIdx( i ).GetAsInteger()` | |
| ChaiScript  | a `std::vector<Boxed_Value>` (ChaiScript's Vector) with one `Boxed_Value` per element, through the `std::function`; the result with `boxed_cast<int>` per element | |
| AngelScript | `CScriptArray::Create` for `array<int>`, the elements copied as a block into `GetBuffer()` (they are stored contiguously), `SetArgObject`; the result copied out of its `GetBuffer()` | |
| Lua         | LuaBridge3: `LuaRef::call<std::vector<int>>( a )`, which converts both ways (`LuaBridge/Vector.h`) | `c-api`: `lua_createtable` and `lua_rawseti`; the result with `lua_rawlen` and `lua_rawgeti` |
| Python      | pybind11: `f( a ).cast<std::vector<int>>()` (`pybind11/stl.h` converts to and from a list) | `c-api`: `PyList_New` and `PyList_SET_ITEM` with `PyLong_FromLong`; the result with `PyList_GET_ITEM` and `PyLong_AsLong` |

## Notes per language

- C++: the reference calls a normal C++ function, which builds the new `std::vector` with `reserve`
  and `push_back`.
- TeaScript: `_tuple_append( b, a[i] + 1 )` appends a new value, so no copy is needed (unlike `sort`).
  The Tuple has no script-level reserve.
- ChaiScript: `b.reserve(size)`, then `push_back`, like `sieve`; no `return`, see
  `fib_recursive/README.md`. ChaiScript's `push_back` for a Vector is a ChaiScript function, which
  makes most of its time.
- AngelScript: `array<int>@ f( const array<int>@ a )`, the usual handle form for arrays; `reserve`, then
  `insertLast`, like `sieve`.
- Lua: `table.create(size)`, then `b[i] = a[i] + 1` (appending with the next index), like `sieve`;
  `f` is global, so the host can find it.
- Python: `b.append(a[i] + 1)`, like `sieve` (a list has no reserve).

## Measurements (per call of the host's loop, n = 10 / 1,000 / 40,000)

| Language    | MSVC: headline | MSVC: `c-api` | GCC 11: headline | GCC 11: `c-api` |
|-------------|----------------|---------------|------------------|-----------------|
| TeaScript `vm-O1` | 36.6 / 24.7 / 27.9 ms |  | 23.3 / 16.0 / 21.0 ms |       |
| ChaiScript `no-threads` | 396 / 351 / 354 ms | | 228 / 198 / 200 ms |        |
| AngelScript `no-threads-no-line-cues` | 3.7 / 1.8 / 2.0 ms | | 3.7 / 2.3 / 2.4 ms | |
| Lua         | 3.0 / 1.2 / 1.2 ms (LuaBridge3) | 2.5 / 0.9 / 0.8 ms | 2.1 / 1.0 / 1.1 ms (LuaBridge3) | 1.7 / 0.8 / 0.9 ms |
| Python      | 2.8 / 1.6 / 1.8 ms (pybind11) | 2.1 / 1.3 / 1.6 ms | 3.6 / 2.5 / 2.7 ms (pybind11) | 3.0 / 2.2 / 2.3 ms |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
