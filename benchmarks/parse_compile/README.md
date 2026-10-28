# parse_compile

**Group:** host. **Measures:** what an application pays to make a source runnable: parse it, compile it
where the language compiles, and run its top level, which defines the functions.

## Task

The script of each language is a template. Before anything is measured, the runner generates a source
with n copies of the marked function, numbered `fn0` .. `fn<n-1>`:

```
g( x ) = 2 * x + 1                       (header)

fn<k>( a ):                              (repeated, k = 0 .. n-1)
    sum = 0
    for i = 0 .. a-1:
        r = i % 3
        if r == 0:
            sum = sum + g( i )
        else:
            sum = sum - i * r
    return sum + k

bench( n ) = fn0( n ) + fn<n-1>( n )     (footer)
```

- Series n = 100 / 1,000 / 10,000 functions. With 10,000 functions the sources have 110,000 (Python)
  to 140,000 (AngelScript) lines, 1.96 to 2.42 MB.
- One sample: a fresh engine (not measured), then the `load` of the generated source (measured), then
  `bench( n )` (not measured) as the check, then the engine is destroyed (not measured). The engine is
  fresh for every sample, since TeaScript and ChaiScript reject a redefinition.
- The time of a sample is the sum of the sub-phases of `load`, the same as the `load` column of the
  other benchmarks. The lookup of `bench` after loading is not part of it.
- Checksum: the result of `bench( n )`, -2,935 / -329,335 / -33,293,335. All values fit into 32 bit
  signed integers.
- C++ is "n/a". The `c-api` variants of Lua and Python are "n/a": their `load` is the same as the
  headline's.

## Rules

- The template format (`ScriptKind::generated`, `harness/include/csb/benchmark.hpp`): the lines between
  a line with `csb:repeat` and a line with `csb:end` are repeated n times, with `__I__` replaced by
  0 .. n-1; `__LAST__` is replaced by n-1. The marker lines are comments, which the runner removes.
- The function has a parameter, two locals, a counting loop, an if/else, arithmetic and a call of a
  helper, in each language's natural form (like `int_arith` and `branching`).
- The functions are named `fn<k>`, not `f<k>`: `f64` is a type name in TeaScript.
- The headline is the total, "source -> runnable". The sub-phases are the ones of `load` (PLAN.md,
  section 3), as far as the language separates them.

## Sub-phases per language

| Language    | Sub-phases                                                  |
|-------------|-------------------------------------------------------------|
| TeaScript   | `parse` (`Parser::Parse`), `compile` (`StackVM::Compiler`, with the optimization level of the variant), `top_level` (`ExecuteProgram`); the `ast` variant: `parse`, `top_level` (the AST is evaluated) |
| ChaiScript  | `parse` (`parse()` of the engine), `top_level` (`eval()` of the AST, which defines the functions); ChaiScript does not compile |
| AngelScript | `parse_compile` (`GetModule`, `AddScriptSection` and `Build`, one step) |
| Lua         | `parse_compile` (`luaL_loadbufferx`), `top_level` (`lua_pcall` of the chunk) |
| Python      | `parse_compile` (`Py_CompileString`), `top_level` (`PyEval_EvalCode` in a fresh namespace) |

## Notes per language

- ChaiScript: no `return`, see `fib_recursive/README.md`. Its `top_level` grows quadratically with n
  (MSVC: ~0.56 ms / ~14 ms / ~0.93 s): ChaiScript keeps the functions of the engine in a
  `QuickFlatMap`, which is searched linearly (`std::find_if`), so every `def` costs time proportional
  to the number of functions already defined.
- Lua: the generated functions are global. `local function`, the form of the other Lua scripts, would
  exceed Lua's limit of 200 local variables per function (here the main chunk). The helper `g` is a
  `local function`.
- Python: parsing and compiling is one step, `Py_CompileString`, the normal path of the C API. A split
  into `ast.parse` and `compile()` would build Python objects for the AST, which the normal path never
  does (PLAN.md, decision log 2026-10-28). `sum` is called `total`, so it doesn't shadow the built-in.

## Measurements (per sample, median)

| Language    | MSVC: n = 100 | 1,000    | 10,000   | GCC 11: n = 100 | 1,000    | 10,000   |
|-------------|---------------|----------|----------|-----------------|----------|----------|
| TeaScript `vm-O1` | ~3.0 ms | ~29 ms   | ~291 ms  | ~1.6 ms         | ~17 ms   | ~203 ms  |
| ChaiScript `no-threads` | ~4.9 ms | ~56 ms | ~1.36 s | ~2.8 ms     | ~33 ms   | ~876 ms  |
| AngelScript `no-threads-no-line-cues` | ~4.1 ms | ~38 ms | ~570 ms | ~3.1 ms | ~31 ms | ~528 ms |
| Lua         | ~0.50 ms      | ~4.5 ms  | ~44 ms   | ~0.31 ms        | ~3.0 ms  | ~30 ms   |
| Python      | ~3.9 ms       | ~41 ms   | ~439 ms  | ~2.4 ms         | ~29 ms   | ~311 ms  |

Sub-phases with n = 10,000 (MSVC): TeaScript `parse` ~176 ms, `compile` ~111 ms, `top_level`
~3.4 ms; ChaiScript `parse` ~428 ms, `top_level` ~929 ms; Lua `parse_compile` ~43 ms, `top_level`
~1.1 ms; Python `parse_compile` ~438 ms, `top_level` ~1.2 ms.

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
