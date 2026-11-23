# register_functions

**Group:** host. **Measures:** what an application pays to register its C++ functions in an engine,
with each language's normal API.

## Task

```
create a fresh engine                         (not measured)
for k = 0 .. n-1:                             (measured)
    register the C++ function f1 under the name fn<k>
check: bench( n ) = fn0( n ) + fn<n-1>( n + 3 )    (not measured)
```

- `f1( a ) = a & 7` is the C++ function of `script_to_host` (`script_to_host/host.hpp`); all n names
  refer to it. The names are created before the measurement.
- Series n = 10 / 100 / 1,000.
- One sample: a fresh engine, then the registrations (measured), then the generated script, whose
  `bench( n )` calls the first and the last registered function, as the check (not measured).
  Checksum: `f1( n ) + f1( n + 3 )`, 7 / 11 / 3.
- The script is a template (`ScriptKind::generated`, see `parse_compile`): the runner replaces
  `__LAST__` with n-1.
- C++ is "n/a". The `c-api` variants of Lua and Python run it: the binding layer is what is measured.

## Rules

- Each language registers with its normal API for C++ functions, like in `script_to_host`, and into the
  same place: TeaScript, ChaiScript and AngelScript as global functions, Lua into the table `host`,
  Python into the module `host`.
- Only the registrations are measured. Creating the place where they go is part of the measurement
  where the API does it in the same step (Lua: the table `host`); a module which exists before is not
  (Python: importing pybind11's embedded module `host`, creating the module with the C API).
- The functions are named `fn<k>`, not `f<k>`: `f64` is a type name in TeaScript.

## Registration per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `RegisterUserCallback( name, callback )`, the callback of `script_to_host` | |
| ChaiScript  | `add( chaiscript::fun( &f1 ), name )`      |                                           |
| AngelScript | `RegisterGlobalFunction( "int fn<k>( int a )", asFUNCTION( f1 ), asCALL_CDECL )`, the declarations created before the measurement | |
| Lua         | LuaBridge3: `beginNamespace( "host" )`, then `addFunction( name, &f1 )` n times | `c-api`: `lua_createtable`, `luaL_setfuncs` with a `luaL_Reg` list (created before the measurement), `lua_setglobal` |
| Python      | pybind11: `m.def( name, &f1 )` on the module `host` | `c-api`: `PyModule_AddFunctions` with a `PyMethodDef` list (created before the measurement) |

## Notes per language

- ChaiScript: the time grows quadratically with n. ChaiScript keeps the functions of the engine in a
  `QuickFlatMap`, which is searched linearly (`std::find_if`), see `parse_compile`.
- AngelScript: the time grows faster than linearly as well (~16x from 100 to 1,000 functions).

## Results

The measured numbers of every language and variant, with both toolchains, are in the result sets:
[results/](../../results/README.md).
The samples of n = 10 take only a few microseconds, so their spread is larger.
