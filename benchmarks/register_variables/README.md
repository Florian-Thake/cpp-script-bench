# register_variables

**Group:** host. **Measures:** what an application pays to register host variables in an engine, in
each API's normal form of a global variable.

## Task

```
create a fresh engine                         (not measured)
for k = 0 .. n-1:                             (measured)
    register the host int variable v<k> (value k) under the name v<k>
check: bench( n ) = v0 + v<n-1>               (not measured)
```

- The loop of `register_functions` (`register_functions/host.hpp`) with variables instead of functions.
  The names and the values are created before the measurement.
- Series n = 10 / 100 / 1,000. Checksum: n-1 (9 / 99 / 999).
- C++ is "n/a". The `c-api` variants of Lua and Python run it.

## Rules

- Each API's normal form of a global variable (PLAN.md, decision log 2026-10-28): bound to the
  variable of the host (by reference) where the API binds C++ variables, a copy where it only copies
  values. The variables are read-write.
- The same places as in `register_functions`: global variables in TeaScript, ChaiScript and
  AngelScript (ChaiScript: `add_global`, so functions see them), the table `host` in Lua, the module
  `host` in Python.

## Registration per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `AddVar( name, value )`: a script variable with a copy of the value | |
| ChaiScript  | `add_global( chaiscript::var( std::ref( value ) ), name )`: bound by reference | |
| AngelScript | `RegisterGlobalProperty( "int v<k>", &value )`: bound by reference, AngelScript's only form | |
| Lua         | LuaBridge3: `beginNamespace( "host" )`, then `addProperty( name, &value, &value )` n times: read-write properties bound by reference | `c-api`: `lua_createtable`, then `lua_pushinteger` and `lua_setfield` n times (copies), `lua_setglobal` |
| Python      | pybind11: `m.attr( name ) = value` on the module `host` (a copy) | `c-api`: `PyModule_AddIntConstant` (a copy) |

## Notes per language

- TeaScript, Python and Lua's C API can only copy a value into the engine (TeaScript can share a
  value between host and script, but not bind a C++ variable).
- ChaiScript can copy as well (`chaiscript::var( value )`); its binding by reference is used, like
  AngelScript's, since it is the form for a variable of the host.
- Lua: LuaBridge3's properties of a namespace are read with a metamethod; that only matters for the
  check here.

## Results

The measured numbers of every language and variant, with both toolchains, are in the result sets:
[results/](../../results/README.md).
The samples of n = 10 take only a few microseconds, so their spread is larger.
