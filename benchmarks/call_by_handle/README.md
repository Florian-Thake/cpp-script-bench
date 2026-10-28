# call_by_handle

**Group:** host. **Measures:** what an application pays to call script functions through handles,
which it looked up once. The counterpart of `call_by_name`: the difference is the cost of the lookup.

## Task

The loop of `call_by_name` (`call_by_name/host.hpp`), with the same script and checksum:

```
fn<k>( a ) = a + k                 (script, k = 0 .. n-1)

look up the handle of every fn<k>  (in load, not measured)
sum = 0
for i = 0 .. 99,999:               (C++)
    k = i % n
    sum = sum + call fn<k>( i & 7 ) through its handle
return sum
```

- Series n = 10 / 1,000 / 10,000 functions, always 100,000 calls. Checksum: 800,000 / 50,300,000 /
  500,300,000.
- Like `host_to_script`: one engine per run; the handles are looked up in `load`.
- C++ is "n/a". The `c-api` variants of Lua and Python run it.
- TeaScript is "n/a": its default public API has no function handle (PLAN.md, decision log 2026-10-28).
  It has no script here, like in `cpp_objects`.

## Rules

- The handle is what the language's API gives for a script function, and the call is the one of
  `host_to_script`.

## Handle and call per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | "n/a"                                      |                                           |
| ChaiScript  | the `std::function<int( int )>` from `eval( name )` | |
| AngelScript | the `asIScriptFunction` from `GetFunctionByDecl`, called with `Prepare`, `SetArgDWord`, `Execute`, `GetReturnDWord` in one reused context | |
| Lua         | LuaBridge3: the `LuaRef` from `getGlobal`, `call<int>( a )` | `c-api`: a registry reference (`luaL_ref`), `lua_rawgeti`, `lua_pushinteger`, `lua_pcall`, `lua_tointegerx` |
| Python      | pybind11: the `py::object` of the function, `f( a ).cast<int>()` | `c-api`: `PyObject_CallOneArg` with `PyLong_FromLong` and `PyLong_AsLong` |

## Notes per language

- TeaScript: the counterpart of a handle would be the `FunctionPtr` from `GetVar()`; calling it needs
  the `Context`, which only a class derived from `Engine` reaches (the low-level API). A public call of
  a `FunctionPtr` may be added to TeaScript later; then it is used here.
- The `load` of n = 10,000 includes the lookup of 10,000 handles: ChaiScript ~1.1 s (its linear
  function table, see `call_by_name`), AngelScript ~0.29 s (MSVC).
- The time per call grows only a little with n (at most ~30% from 10 to 10,000 functions), while a
  call by name grows up to ~5x (ChaiScript).

## Measurements (per call of the host's loop, 100,000 calls, median)

| Language    | MSVC: n = 10 | 1,000   | 10,000   | GCC 11: n = 10 | 1,000    | 10,000   |
|-------------|--------------|---------|----------|----------------|----------|----------|
| ChaiScript `no-threads` | ~47.7 ms | ~50.5 ms | ~54.1 ms | ~20.4 ms | ~21.0 ms | ~23.6 ms |
| AngelScript `no-threads-no-line-cues` | ~4.9 ms | ~5.3 ms | ~5.4 ms | ~4.6 ms | ~4.7 ms | ~4.9 ms |
| Lua (LuaBridge3) | ~4.8 ms | ~4.9 ms  | ~5.1 ms  | ~3.4 ms        | ~3.5 ms  | ~3.6 ms  |
| Lua `c-api` | ~2.9 ms      | ~3.2 ms | ~3.7 ms  | ~2.3 ms        | ~2.4 ms  | ~2.6 ms  |
| Python (pybind11) | ~4.8 ms | ~5.1 ms | ~5.5 ms  | ~4.9 ms        | ~5.7 ms  | ~6.4 ms  |
| Python `c-api` | ~4.2 ms   | ~4.7 ms | ~4.9 ms  | ~4.5 ms        | ~5.3 ms  | ~5.9 ms  |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
