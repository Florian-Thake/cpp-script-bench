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

## Measurements (per sample, median)

MSVC:

| Language    | n = 10   | 100      | 1,000    |
|-------------|----------|----------|----------|
| TeaScript `vm-O1` | ~2.5 us | ~39 us | ~0.29 ms |
| ChaiScript `no-threads` | ~48 us | ~0.53 ms | ~12.7 ms |
| AngelScript `no-threads-no-line-cues` | ~16 us | ~0.18 ms | ~2.8 ms |
| Lua (LuaBridge3) | ~3.8 us | ~21 us | ~0.23 ms |
| Lua `c-api` | ~1.8 us  | ~9.9 us  | ~0.11 ms |
| Python (pybind11) | ~38 us | ~0.25 ms | ~2.4 ms |
| Python `c-api` | ~2.5 us | ~19 us | ~0.21 ms |

GCC 11:

| Language    | n = 10   | 100      | 1,000    |
|-------------|----------|----------|----------|
| TeaScript `vm-O1` | ~6.0 us | ~29 us | ~0.13 ms |
| ChaiScript `no-threads` | ~25 us | ~0.29 ms | ~8.0 ms |
| AngelScript `no-threads-no-line-cues` | ~13 us | ~0.14 ms | ~2.1 ms |
| Lua (LuaBridge3) | ~2.0 us | ~16 us | ~0.15 ms |
| Lua `c-api` | ~0.7 us  | ~7.1 us  | ~71 us   |
| Python (pybind11) | ~29 us | ~0.24 ms | ~2.3 ms |
| Python `c-api` | ~2.0 us | ~14 us | ~0.19 ms |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL). The samples of n = 10 are only a few microseconds,
so their spread is larger (up to ~15%).
