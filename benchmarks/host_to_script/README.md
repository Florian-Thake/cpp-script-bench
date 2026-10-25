# host_to_script

**Group:** interop. **Measures:** the overhead of calls of script functions from C++, with 0, 1 and
3 arguments: the call API of the language, the conversion of the arguments and of the result.

## Task

The mirror of `script_to_host`. The script defines the three functions of `calls`, and the host calls
them in a C++ loop (`host.hpp`, the same for every runner and the C++ reference):

```
f0()          = 1                  (script)
f1( a )       = a & 7              (script)
f3( a, b, c ) = (a + b + c) & 7    (script)

sum = 0
for i = 0 .. n-1:                  (C++)
    sum = sum + f0() + f1( i ) + f3( i, 1, 2 )
return sum
```

- Default `n`: 100,000, i.e. 300,000 calls, like `calls` and `script_to_host`, with the same checksum
  (800,000 for n = 100,000).
- The script has no `bench`. The measured unit (one call of `call_bench( n )`) is the host's loop.
- All values fit into 32 bit signed integers for n <= 260,000,000.

## Rules

- The script functions are normal named functions, defined like in `calls`, so the host can find them
  (global in Lua).
- The host calls them through the language's normal C++ API. Where the API has a function handle, the
  runner looks it up once in the `load` phase, like `bench`, and calls through it (PLAN.md, decision
  log 2026-10-25). The arguments and the results are integers.

## Call path per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `CallFuncEx( "f1", a )` by name, the result with `GetAsInteger()` (the API has no function handle) | |
| ChaiScript  | `std::function<int( int )>` from `eval( "f1" )` | |
| AngelScript | the `asIScriptFunction` from `GetFunctionByDecl`, called with `Prepare`, `SetArgDWord`, `Execute`, `GetReturnDWord` in one reused context | |
| Lua         | LuaBridge3: `LuaRef` from `getGlobal`, `call<int>( a )` | `c-api`: a registry reference (`luaL_ref`), `lua_rawgeti`, `lua_pushinteger`, `lua_pcall`, `lua_tointegerx` |
| Python      | pybind11: the `py::object` of the function, `f1( a ).cast<int>()` | `c-api`: `PyObject_CallNoArgs`, `PyObject_CallOneArg`, `PyObject_Vectorcall` with `PyLong_FromLong` and `PyLong_AsLong` |

## Notes per language

- C++: the reference runs the same loop with normal C++ functions, which the compiler inlines.
- TeaScript: `CallFuncEx` looks up the function by its name on every call; TeaScript's public API has
  no function handle (PLAN.md, decision log 2026-10-09).
- ChaiScript: no `return`, see `fib_recursive/README.md`.
- Lua: the functions are global (not `local function` as in `calls`), so the host can find them. The
  `c-api` variant calls through registry references, the C API's handle (also what a LuaBridge3
  `LuaRef` holds). `lua_getglobal` on every call, the canonical form for single calls (and how the
  runner calls `bench`), was ~17% slower in a scratch test with MSVC.

## Measurements (per call of the host's loop, n = 100,000)

| Language    | MSVC: headline | MSVC: `c-api` | GCC 11: headline | GCC 11: `c-api` |
|-------------|----------------|---------------|------------------|-----------------|
| TeaScript `vm-O1` | ~215 ms  |               | ~152 ms          |                 |
| ChaiScript `no-threads` | ~180 ms |         | ~72 ms           |                 |
| AngelScript `no-threads-no-line-cues` | ~17 ms | | ~16 ms        |                 |
| Lua         | ~17 ms (LuaBridge3) | ~10.6 ms | ~11.7 ms (LuaBridge3) | ~8.0 ms     |
| Python      | ~19.4 ms (pybind11) | ~16.0 ms | ~21.1 ms (pybind11) | ~18.5 ms     |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
