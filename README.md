# cpp-script-bench

Fair benchmarks of script languages embedded in C++.

Each benchmark discipline has an equivalent script for every language. A small C++ host runs it
through the language's own C++ embedding API and does all the measuring. Only CPU and RAM work is
measured, no IO.

> **Status:** early development. The harness and the runners for all languages work, with the
> core benchmarks. See [PLAN.md](PLAN.md) for the full plan and the fairness rules.

## Languages

| Language    | Embedded via               | Status    |
|-------------|----------------------------|-----------|
| C++         | native reference           | available |
| TeaScript   | native C++ API             | available (variants: TeaStackVM O0/O1/O2, AST eval; headline: O1) |
| ChaiScript  | native C++ API + ChaiScript_Extras (math module) | available (two builds: default thread-safe, and `CHAISCRIPT_NO_THREADS`; headline: `CHAISCRIPT_NO_THREADS`) |
| AngelScript | native C++ API + add-ons (string, array, dictionary, math in double precision) | available (two builds: default thread-safe, and `AS_NO_THREADS`; each with and without line cues; headline: `AS_NO_THREADS` without line cues) |
| Lua         | Lua C API; LuaBridge3 for the bindings (later) | available |
| Python      | pybind11 (embedded)        | available (the installed Python, see below) |

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
- a C++20 compiler: MSVC 2022 or GCC (plus the C compiler for Lua, and MASM for AngelScript with MSVC;
  both come with the compilers)
- the script language libraries you want to test (see below)
- for Python: an installed Python 3.9 or newer with its development files (on Windows included in the
  python.org installer; on Debian/Ubuntu the package `python3-dev`)

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
        "CSB_TEASCRIPT_DIR": "C:/path/to/TeaScriptLibrary"
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
| `CSB_WITH_CHAISCRIPT`           | build the ChaiScript runners `csb_chaiscript` and `csb_chaiscript_nothreads` |
| `CSB_CHAISCRIPT_DIR`            | root of ChaiScript (contains `include/chaiscript`); use the `develop` branch |
| `CSB_CHAISCRIPT_EXTRAS_DIR`     | root of ChaiScript_Extras (contains `include/chaiscript/extras`), for its math module; required for the ChaiScript runners |
| `CSB_WITH_ANGELSCRIPT`          | build the AngelScript runners `csb_angelscript` and `csb_angelscript_nothreads` |
| `CSB_ANGELSCRIPT_DIR`           | the `sdk` directory of the AngelScript SDK (contains `angelscript/include` and `add_on`) |
| `CSB_WITH_LUA`                  | build the Lua runner `csb_lua` |
| `CSB_LUA_DIR`                   | root of the Lua source distribution (contains `src/lua.h`) |
| `CSB_WITH_PYTHON`               | build the Python runner `csb_python` |
| `CSB_PYBIND11_DIR`              | root of pybind11 (contains `include/pybind11`) |
| `Python_ROOT_DIR`, `Python_EXECUTABLE` | optional: another installed Python than the one CMake finds by default (standard hints of CMake's FindPython) |
| `CSB_ARCH`                      | `x86-64-v3` (AVX2, default) or `default` |

## Running

There is one runner executable per language: `csb_cpp`, `csb_teascript`, `csb_chaiscript`, `csb_lua`,
`csb_python`, ...
Languages with two builds have two: `csb_chaiscript` and `csb_chaiscript_nothreads`,
`csb_angelscript` and `csb_angelscript_nothreads`.

```
csb_teascript --list                        # benchmarks and variants
csb_teascript                               # all benchmarks, default variant
csb_teascript --variant all                 # TeaStackVM O0, O1, O2 and AST eval
csb_teascript --bench fib_recursive --n 30 --runs 20 --json results.json
csb_chaiscript_nothreads --budget 0         # no time budget, always 3 warm-up and 10 measured calls
```

Some benchmarks run a series of workload sizes, e.g. `fib_recursive` with n = 15, 20 and 25. Each n
is a separate row. `--n 20` or `--n 10,20` replaces the series.

Slow runs are limited by a time budget (default 30 s for all calls of one benchmark and n). At least
3 calls are always measured. Before the first run of each variant, the runner makes one unmeasured
warm-up run, so the first run is not slowed down by cold caches. The details are in
[PLAN.md](PLAN.md#3-what-is-measured).

Each language has one headline variant for the main comparison. The runners mark it with
"(headline)".

Columns in the output:

| Column  | Meaning |
|---------|---------|
| startup | creating the engine including its standard library |
| load    | making the script runnable: parse, compile, run its top-level code |
| first   | the very first call of `bench( n )` |
| runs    | the number of measured calls; fewer than `--runs` if the time budget ended the run early, marked with "(time budget)" |
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
