# cpp-script-bench

Fair benchmarks of script languages embedded in C++.

Each benchmark discipline has an equivalent script for every language. A small C++ host runs it
through the language's own C++ embedding API and does all the measuring. Only CPU and RAM work is
measured, no IO.

> **Status:** early development. The harness, the C++ reference and the TeaScript runner work, with
> one benchmark so far. See [PLAN.md](PLAN.md) for the full plan and the fairness rules.

## Languages

| Language    | Embedded via               | Status    |
|-------------|----------------------------|-----------|
| C++         | native reference           | available |
| TeaScript   | native C++ API             | available (variants: TeaStackVM O0/O1/O2, AST eval) |
| ChaiScript  | native C++ API             | planned   |
| AngelScript | native C++ API + add-ons   | planned   |
| Lua         | LuaBridge3                 | planned   |
| Python      | pybind11 (embedded)        | planned   |

## Fairness

All languages solve the same task with the same algorithm, and every result is verified against the
C++ reference. The scripts use comparable language features, and all libraries built from source
use the same compiler and flags. The complete rules are in [PLAN.md](PLAN.md#2-fairness-rules).

This project is maintained by the author of TeaScript. For that reason, all scripts, settings and
raw results are public. Reviews and pull requests from all language communities are very welcome,
especially if a script does not use its language well.

## Building

Requirements:
- CMake 3.22 or newer
- a C++20 compiler: MSVC 2022 or GCC
- the script language libraries you want to test (see below)

You provide the script language libraries yourself and tell CMake where they are in a
`CMakeUserPresets.json` next to `CMakePresets.json`. That file is ignored by git. Example:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "msvc-local",
      "inherits": "msvc",
      "cacheVariables": {
        "CSB_WITH_TEASCRIPT": "ON",
        "CSB_TEASCRIPT_DIR": "D:/code/repos/TeaScriptLibrary"
      }
    }
  ],
  "buildPresets": [
    { "name": "msvc-local-release", "configurePreset": "msvc-local", "configuration": "Release" }
  ]
}
```

Then:

```
cmake --preset msvc-local
cmake --build --preset msvc-local-release
```

On Linux, inherit from the `gcc` preset instead (Ninja, Release).

| Option                          | Meaning |
|---------------------------------|---------|
| `CSB_WITH_TEASCRIPT`            | build the TeaScript runner |
| `CSB_TEASCRIPT_DIR`             | root of the TeaScript C++ Library (contains `include/teascript`) |
| `CSB_TEASCRIPT_DEFAULT_VARIANT` | variant used when no `--variant` is given (default `vm-O1`) |
| `CSB_FMT_DIR`                   | optional: root of the fmt library, used by TeaScript (needed for GCC < 13, which has no `std::format`) |
| `CSB_ARCH`                      | `x86-64-v3` (AVX2, default) or `default` |

## Running

There is one runner executable per language: `csb_cpp`, `csb_teascript`, ...

```
csb_teascript --list                        # benchmarks and variants
csb_teascript                               # all benchmarks, default variant
csb_teascript --variant all                 # TeaStackVM O0, O1, O2 and AST eval
csb_teascript --bench fib_recursive --n 30 --runs 20 --json results.json
```

Columns in the output:

| Column  | Meaning |
|---------|---------|
| startup | creating the engine including its standard library |
| load    | making the script runnable: parse, compile, run its top-level code |
| first   | the very first call of `bench( n )` |
| median, min, stddev | the measured calls of `bench( n )` after warm-up; the headline numbers |

## Layout

```
harness/     shared runner code: command line, measuring, verification, statistics, JSON report
benchmarks/  one directory per discipline: spec (README.md), C++ reference, one script per language
adapters/    one runner executable per language
cmake/       CMake helpers
```

## License

MIT, see [LICENSE.txt](LICENSE.txt).
