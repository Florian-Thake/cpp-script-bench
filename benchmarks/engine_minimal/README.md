# engine_minimal

**Group:** host. **Measures:** the floor of each engine API: create and destroy the smallest engine
the API allows, without a standard library.

## Task

The loop of `engine_lifecycle` (`engine_lifecycle/host.hpp`) with the smallest engine:

```
for i = 0 .. n-1:
    create the smallest engine     (sub-phase "create")
    destroy it                     (sub-phase "destroy")
return n
```

- Default `n`: 10 engines per sample, the checksum is the number of engines, like `engine_lifecycle`.
- C++ is "n/a": it has no engine.

## Rules

- The smallest engine of each language's public API, with the same settings as in `engine_lifecycle`
  where they don't add a library (PLAN.md, decision log 2026-10-28).
- The engines are not equally usable: the number is the cost of the API's floor, and the difference to
  `engine_lifecycle` is what the standard library costs.

## The engine per language

| Language    | create                                     | destroy                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | `teascript::Engine` with `config::minimal()`, the lowest level of the Core Library: only the types and the version variables | the destructor |
| ChaiScript  | `chaiscript::ChaiScript` with `Library_Options::No_Stdlib` (an empty module instead of `Std_Lib`) and the options of `engine_lifecycle`: only the parser and the functions of the eval system (`eval`, `print_string`, ...) | the destructor |
| AngelScript | `asCreateScriptEngine()`, without add-ons, message callback and context | `ShutDownAndRelease()` |
| Lua         | `luaL_newstate()` without libraries        | `lua_close()`                             |
| Python      | "n/a"                                      |                                           |

## Notes per language

- TeaScript: `config::minimal()` is the public way. The protected constructor of `Engine` which skips
  the Core Library is not used: it is part of the low-level API, which is not used for `call_by_handle`
  either (PLAN.md, decision log 2026-10-28).
- ChaiScript: `Std_Lib` also registers the basic types and their operators (`Bootstrap::bootstrap`).
  `No_Stdlib` is ChaiScript's own option for an engine without it.
- Lua: a state without libraries still runs the whole language, only without the library functions.
- Python: "n/a". The interpreter always loads its core modules (`builtins`, `sys`, the import system,
  the codecs); the isolated configuration of `engine_lifecycle` is already its minimum.

## Results

The measured numbers of every language and variant, with both toolchains, are in the result sets:
[results/](../../results/README.md).
`engine_lifecycle` creates the engines with their standard libraries. The sub-phases `create` and
`destroy` of every sample are in the raw data of the sets (`results.json`).
