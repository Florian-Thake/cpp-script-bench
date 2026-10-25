# script_to_host

**Group:** interop. **Measures:** the overhead of calls of C++ functions from the script, with 0, 1 and
3 arguments: the binding layer, the conversion of the arguments and of the result.

## Task

The host registers three C++ functions (`host.hpp`). `bench( n )` runs the loop of the `calls`
benchmark with them:

```
f0()          = 1                  (C++)
f1( a )       = a & 7              (C++)
f3( a, b, c ) = (a + b + c) & 7    (C++)

sum = 0
for i = 0 .. n-1:
    sum = sum + f0() + f1( i ) + f3( i, 1, 2 )
return sum
```

- Default `n`: 100,000, i.e. 300,000 calls, like `calls`. The checksum is the same (800,000 for
  n = 100,000), so the results compare directly: a call of a script function (`calls`) and a call of
  a C++ function (`script_to_host`).
- All values fit into 32 bit signed integers for n <= 260,000,000.

## Rules

- The host registers the functions in the `bind` phase, before the script is loaded, with the
  language's normal way of binding a C++ function. The parameters and the result are `int`.
- The script calls them by their names, like the script functions in `calls`. Where they live in a
  module or table, the script may copy them into its namespace or into locals once in its top level,
  in the idiomatic fast form of the language for library functions (like `math_builtins`).
- Use the language's natural counting loop, like in `loop`.
- Write the body exactly as `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )`, as in `calls`.

## Binding per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `RegisterUserCallback`: a callback reads its parameters from the Context | |
| ChaiScript  | `chaiscript::fun( &f )`                    | |
| AngelScript | `RegisterGlobalFunction` with the native calling convention | |
| Lua         | LuaBridge3 `addFunction` in the namespace (table) `host` | `c-api`: `lua_CFunction`s in the table `host` (`luaL_newlib`) |
| Python      | pybind11 `m.def` in the embedded module `host` | `c-api`: `PyMethodDef`s (`METH_NOARGS`, `METH_O`, `METH_FASTCALL`) in a module `host` |

Lua and Python use their C++ binding libraries for the headline (PLAN.md, decision log 2026-10-25);
the variants `c-api` show the same with the plain C API.

## Notes per language

- C++: the reference calls the functions of `host.hpp` directly; the compiler inlines them, as it
  would in any C++ program.
- TeaScript: the callbacks are written like the examples of the TeaScript demo: check the number of
  parameters, take them with `ConsumeParam()` and convert them with `GetAsInteger()`.
- ChaiScript: no `return`, see `fib_recursive/README.md`.
- AngelScript: the native calling convention is AngelScript's default on the supported platforms. The
  generic convention (`asCALL_GENERIC`, a wrapper which reads the arguments from an
  `asIScriptGeneric`) was measured as an alternative: with GCC 11 it was ~37% faster (7.5 ms instead of
  11.8 ms per call of `bench`), with MSVC ~15% slower (8.2 ms instead of 7.1 ms). The script is the same
  for both; the native convention stays, since the generic one is not faster with both toolchains.
- Lua: `local f0, f1, f3 = host.f0, host.f1, host.f3` in the top level, like `math.sqrt` in
  `math_builtins`. `bench` is global, so the host can find it.
- Python: `from host import f0, f1, f3` in the top level, like `math` in `math_builtins`. The module
  `host` is imported in the `bind` phase (pybind11: an embedded module; `c-api`: a module created by
  the host), so the import of the script only finds it in `sys.modules`.

## Measurements (MSVC, per call of `bench`, n = 100,000)

The cost of the binding layer, compared with `calls` (script functions):

| Language    | `calls` | headline | `c-api` |
|-------------|---------|----------|---------|
| TeaScript `vm-O1` | 162 ms | 92 ms |  |
| ChaiScript `no-threads` | 206 ms | 147 ms |  |
| AngelScript `no-threads-no-line-cues` | 4.5 ms | 6.9 ms |  |
| Lua         | 4.9 ms  | 9.9 ms (LuaBridge3) | 5.1 ms |
| Python 3.14 | 12.0 ms | 48.7 ms (pybind11) | 8.4 ms |

With GCC 11 (WSL; Python 3.10): Lua 6.4 ms (LuaBridge3) and 4.2 ms (`c-api`), Python 20.2 ms (pybind11)
and 10.4 ms (`c-api`).
