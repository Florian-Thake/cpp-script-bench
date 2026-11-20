# engine_lifecycle

**Group:** host. **Measures:** what an application pays to create an engine with its standard library
and to destroy it again.

## Task

The runner creates and destroys engines in a C++ loop (`host.hpp`, the same for every runner) and
measures both steps:

```
for i = 0 .. n-1:
    create an engine with its standard library     (sub-phase "create")
    destroy it                                      (sub-phase "destroy")
return n
```

- Default `n`: 10 engines per sample. The time of a sample is the sum of both sub-phases, which are
  reported as well.
- No script. The checksum is the number of engines. Every API reports a failure: an exception, or a
  null pointer, which the runner turns into one.
- C++ is "n/a": it has no engine.

## Rules

- The engine is the one of all other benchmarks: the runner's `create_engine()` and
  `destroy_engine()`. So these are the startup and the teardown of the other benchmarks, which are
  measured only once per run there, with samples (PLAN.md, decision log 2026-10-28).
- Each sample is one call of `run_host()` (PLAN.md, section 4), like every host API benchmark.

## The engine per language

| Language    | create                                     | destroy                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `teascript::Engine` with `config::no_modules( config::full() )` (the full Core Library, without importing modules; the option has no effect until TeaScript implements modules) in the mode of the variant (TeaStackVM or AST eval), no tsb path | the destructor |
| ChaiScript  | `chaiscript::ChaiScript` (with `Std_Lib`) and the options `No_Load_Modules` and `No_External_Scripts`, then the math module of ChaiScript_Extras | the destructor |
| AngelScript | `asCreateScriptEngine()`, the message callback, the line cue property of the variant, the add-ons for strings, arrays, dictionaries and math, one context | `Release()` of the context, `ShutDownAndRelease()` |
| Lua         | `luaL_newstate()`, `luaL_openselectedlibs()` without `package`, then `dofile` and `loadfile` removed | `lua_close()` |
| Python      | the embedded interpreter (pybind11) with the isolated configuration, without `site`, then the check that the JIT is off | finalizing the interpreter |

The `c-api` variants of Lua and Python are "n/a": their engine is the same as the headline's.

## Notes per language

- Python: there is one interpreter at a time, which is initialized and finalized again for every
  engine; this works for the standard library (PLAN.md, decision log 2026-10-06). Its creation reads
  the modules of the standard library from disk, after the first time from the cache of the OS (PLAN.md,
  "No IO").

## Measurements (per engine: the median of a sample / 10)

| Language    | MSVC: create | MSVC: destroy | GCC 11: create | GCC 11: destroy |
|-------------|--------------|---------------|----------------|-----------------|
| TeaScript `vm-O1` | ~1.0 ms | ~45 us        | ~0.64 ms       | ~33 us          |
| ChaiScript `no-threads` | ~3.8 ms | ~0.28 ms | ~2.0 ms       | ~0.17 ms        |
| AngelScript `no-threads-no-line-cues` | ~0.44 ms | ~42 us | ~0.34 ms | ~22 us     |
| Lua         | ~24 us       | ~5.8 us       | ~17 us         | ~2.6 us         |
| Python      | ~5.4 ms      | ~1.1 ms       | ~2.1 ms        | ~0.48 ms        |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
