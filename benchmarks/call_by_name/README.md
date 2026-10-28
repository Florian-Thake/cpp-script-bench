# call_by_name

**Group:** host. **Measures:** what an application pays to call script functions by their names: the
lookup of the function by name in every call, plus the call itself. With n functions in the engine,
it shows whether the lookup grows with their number.

## Task

The script defines n functions, the host calls them in a C++ loop (`host.hpp`, the same for every
runner and the C++ reference):

```
fn<k>( a ) = a + k                 (script, k = 0 .. n-1)

sum = 0
for i = 0 .. 99,999:               (C++)
    k = i % n
    sum = sum + call fn<k>( i & 7 ) by its name
return sum
```

- Series n = 10 / 1,000 / 10,000 functions, always 100,000 calls. Checksum: 800,000 / 50,300,000 /
  500,300,000.
- The script is a template (`ScriptKind::generated`, see `parse_compile`); it has no `bench`.
- Like `host_to_script`: one engine per run, the script is loaded once; the measured unit (one call
  of `call_bench( n )`) is the host's loop. The names are created in `load`, before the measurement.
- C++ is "n/a". The `c-api` variants of Lua and Python run it.
- `call_by_handle` is the same loop with the functions looked up once.

## Rules

- Every call looks the function up by its name with the language's normal API for it, then calls it
  like `host_to_script` does. The argument and the result are integers.
- The functions are named `fn<k>`, not `f<k>`: `f64` is a type name in TeaScript.

## Lookup and call per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `CallFuncEx( name, a )`, the result with `GetAsInteger()` | |
| ChaiScript  | `eval<std::function<int( int )>>( name )`, then the call | |
| AngelScript | `GetFunctionByName( name )` of the module, then `Prepare`, `SetArgDWord`, `Execute`, `GetReturnDWord` in one reused context | |
| Lua         | LuaBridge3: `getGlobal( L, name )` gives a `LuaRef`, `call<int>( a )` | `c-api`: `lua_getglobal`, `lua_pushinteger`, `lua_pcall`, `lua_tointegerx` |
| Python      | pybind11: `globals[name]` (the namespace of the script), the call, `cast<int>()` | `c-api`: `PyDict_GetItemString`, then `PyObject_CallOneArg` with `PyLong_FromLong` and `PyLong_AsLong` |

## Notes per language

- TeaScript: `CallFuncEx` is the way TeaScript calls a script function from C++ (PLAN.md, decision log
  2026-10-09), the same as in `host_to_script`. It looks the function up by its name in every call;
  TeaScript has no `call_by_handle` (decision log 2026-10-28).
- ChaiScript: `eval()` of the name is ChaiScript's documented way to get a script function as a
  `std::function`. It parses the name, then looks it up in the function table of the engine, which is
  searched linearly (`QuickFlatMap`, see `parse_compile`). So the cost of a call grows with n
  (MSVC: ~2.8 / ~4.0 / ~14.7 us per call). With n = 10,000 a call of `call_bench` needs ~1.5 s
  (above the usual 0.1-0.5 s, within the time budget); the 100,000 calls stay, so the numbers compare
  with `call_by_handle` and `host_to_script`.
- Lua: the functions are global, so the host finds them (`local function` would also exceed Lua's
  limit of 200 local variables per function).
- Python: the script runs in a fresh namespace dict (PLAN.md, decision log 2026-10-06), in which the
  functions are looked up.

## Measurements (per call of the host's loop, 100,000 calls, median)

| Language    | MSVC: n = 10 | 1,000   | 10,000   | GCC 11: n = 10 | 1,000    | 10,000   |
|-------------|--------------|---------|----------|----------------|----------|----------|
| TeaScript `vm-O1` | ~65 ms | ~67 ms | ~74 ms   | ~45 ms         | ~46 ms   | ~53 ms   |
| ChaiScript `no-threads` | ~282 ms | ~404 ms | ~1.47 s | ~161 ms  | ~236 ms  | ~850 ms  |
| AngelScript `no-threads-no-line-cues` | ~12.2 ms | ~18.6 ms | ~23.4 ms | ~11.6 ms | ~16.5 ms | ~19.8 ms |
| Lua (LuaBridge3) | ~11.1 ms | ~12.7 ms | ~14.3 ms | ~9.8 ms     | ~11.5 ms | ~12.9 ms |
| Lua `c-api` | ~3.6 ms      | ~5.8 ms | ~8.0 ms  | ~2.9 ms        | ~5.2 ms  | ~7.2 ms  |
| Python (pybind11) | ~10.9 ms | ~11.4 ms | ~12.8 ms | ~9.7 ms    | ~10.8 ms | ~12.1 ms |
| Python `c-api` | ~9.4 ms   | ~10.3 ms | ~11.9 ms | ~9.5 ms       | ~11.0 ms | ~11.9 ms |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
