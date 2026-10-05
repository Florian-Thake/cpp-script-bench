# cpp-script-bench: Plan

A living document: goals, rules, architecture and roadmap. Last updated: 2026-10-05.

## 1. Goal

A fair and reproducible comparison of script languages which can be embedded in C++.
Primary: TeaScript, ChaiScript, AngelScript. Also: Lua (via LuaBridge3) and Python (via pybind11).
Native C++ is the reference mark.

The disciplines cover the language core (arithmetic, calls, loops, ...), data handling, the
interaction with the C++ host, and the host-side costs of embedding (startup, parse, compile, ...).

## 2. Fairness rules

1. **Same algorithm, same input, same result.** Every script returns a checksum, and the harness
   checks it against the C++ reference. A wrong result makes the run invalid. The checksum must have
   the language's integer type; the runners don't convert other types (e.g. a float or a string).
2. **Inputs come from the host at runtime**, so no optimizer can compute the workload ahead of time.
3. **Each language uses its natural construct for the task**, e.g. numeric `for` in Lua,
   `for ... in range` in Python, `forall` in TeaScript. Alternative variants may be reported as
   variants, but are never picked as the headline because they happen to be faster.
4. **No native shortcuts.** No `sum()`, `sorted()`, `table.sort` or similar where the actual work
   runs in native code, unless every language uses an equivalent helper.
   (A separate "idiomatic" track which allows this may come later.)
5. **Hot code lives in functions and uses local variables** in every language. This is idiomatic
   for Lua and Python and makes a big difference for them.
6. **Values stay within 32-bit signed integers, or are masked explicitly.** ChaiScript's `int` is
   32-bit, Python integers are unbounded. No discipline may depend on overflow behaviour.
7. **Same compiler and flags for everything built from source**: harness, TeaScript, ChaiScript,
   AngelScript, Lua. Python is the documented exception: its official binaries are built separately
   (with PGO).
8. **Execution modes are reported separately.** TeaScript has TeaStackVM and AST eval; the others
   have one default mode each. JIT engines (LuaJIT, the Python JIT) can only appear as optional
   entries outside the main comparison.
9. **AngelScript's static typing** is a property of the language. It is documented, not compensated.
10. **Garbage collection:** a full collection runs before each measured call; GC work during the
    call counts.
11. **Transparency:** all scripts, settings and raw results are public. Reviews from every language
    community are invited, in particular because the maintainer is the author of TeaScript.

## 3. What is measured

### Phases

Every benchmark run goes through these phases, each measured separately:

| Phase    | What                                                      |
|----------|-----------------------------------------------------------|
| startup  | create the engine including its standard library          |
| load     | make the script runnable; sub-phases where the language can separate them: `parse`, `compile`, `top_level` |
| first    | the very first call of `bench( n )`                       |
| execute  | R measured calls of `bench( n )` after W warm-up calls: **the headline numbers** |
| teardown | destroy the engine                                        |

### Protocol

Each script defines a function `bench( n )` which returns an integer checksum. The host calls it
through the language's normal C++ API, once per sample. The cost of this one call is negligible
compared to the workload. The interop disciplines measure call costs explicitly.

### Statistics

Defaults: 3 warm-up calls, 10 measured calls; both can be changed on the command line.
Reported: min, median, mean, standard deviation, max, and all raw samples (JSON).
Every result file also records the CPU, OS, compiler, build configuration, instruction set, and
the version and git state of cpp-script-bench and of each language library.

### No IO

Only work between CPU and RAM is measured:
- Scripts are read from disk before anything is measured.
- Nothing is printed inside a measured region. Results are returned as checksums and printed afterwards.
- No module imports or `require` inside measured regions. Python imports happen during setup.
- TeaScript: the tsb path stays empty, so Bootstrap neither reads nor writes `.tsb` files.
- Python's startup reads its standard library from disk. Its startup number is marked accordingly.

Tests which include file IO (e.g. loading TeaScript `.tsb` versus Python `.pyc`) may become a
separate, opt-in category later.

## 4. Architecture

```
harness/      command line, measuring loop, verification, statistics, JSON report (library)
benchmarks/   registry + one directory per discipline:
              README.md (spec), reference.cpp (C++ reference), <name>.tea, .chai, .as, .lua, .py
adapters/     one runner executable per language: csb_cpp, csb_teascript, ...
cmake/        CMake helpers
runner/       (later) orchestrates all runners, collects JSON, produces tables and charts
```

**One executable per language.** Reasons:
- Heap state and caches stay isolated between languages.
- Peak memory per language becomes meaningful.
- A crash (e.g. a stack overflow from deep recursion) doesn't end the whole run.
- No header or symbol clashes (e.g. `Python.h`).
- Each user only builds the languages they have.

**Adapter interface** (`harness/include/csb/language.hpp`): `create_engine()`, `load()`,
`call_bench( n )`, `collect_garbage()`, `destroy_engine()`. Every runner's `main()` provides a
`LanguageEntry` with its variants, and calls `csb::run_main()`.

**Variants.** One language can have several ways of running code. They are selected with
`--variant <name>` or `--variant all`, without rebuilding. TeaScript: `vm-O0`, `vm-O1` (default),
`vm-O2`, `ast`. The CMake option `CSB_TEASCRIPT_DEFAULT_VARIANT` changes the default.
Variants which need a different build become separate executables: ChaiScript is built as
`csb_chaiscript` (variant `default`, thread-safe) and `csb_chaiscript_nothreads` (variant
`no-threads`, with `CHAISCRIPT_NO_THREADS`).

**Scripts are separate files** (not strings in C++ code), so they get syntax highlighting and each
community can review them.

## 5. Languages and versions

| Language    | Version                                   | Embedded via                       | Notes |
|-------------|-------------------------------------------|------------------------------------|-------|
| TeaScript   | HEAD (0.18.0 pre-release); pinned to 0.18.0 after its release | native API | header-only |
| ChaiScript  | `develop` HEAD, pinned by commit hash (now `f635b51e`, 2026-05-02; reports itself as 7.0.0) | native API | the last release (6.1.0) is from 2018; headline build: `no-threads` |
| AngelScript | 2.38.0 (Aug 2026)                         | native API + string, array, dictionary, math add-ons | compiled library |
| Lua         | 5.5.1                                     | LuaBridge3                         | compiled library |
| Python      | 3.15.0 (final due 2026-10-09) or 3.14.x   | pybind11 3.1 (embedded interpreter) | prebuilt DLL |
| C++         | n/a                                       | reference                          | |

Deferred: Jinx (limited core library, dormant). It may be added later if it can run most disciplines.

**Pinning policy:** every library is used at a defined version or commit. The result files record
the exact git state. A library only moves forward when that is done on purpose.

## 6. Toolchains

| Toolchain | Optimization            | Instruction set     |
|-----------|-------------------------|---------------------|
| MSVC 2022 | `/O2 /Ob2` (Release)    | `/arch:AVX2`        |
| GCC       | `-O2`                   | `-march=x86-64-v3`  |

- No LTO and no PGO for code built from source.
- Results are only compared within one toolchain on one machine. MSVC and GCC numbers never appear
  in the same chart.
- GCC is tested on WSL and on a Linux VPS.

## 7. Disciplines

Status: ✅ implemented, ⬜ planned.

### Core

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| fib_recursive  | recursive `fib( n )`: call and recursion overhead | ✅ |
| loop           | counting loop, body `sum = sum + (i & 7)`         | ✅ |
| int_arith      | Collatz step counts for 1..N                      | ⬜ |
| mandelbrot     | float arithmetic: Mandelbrot grid, checksum = integer iteration count | ✅ |
| math_builtins  | sums of `sqrt`/`sin` (compared with a tolerance)  | ⬜ |
| calls          | tiny function called N times with 0, 1 and 3 arguments | ⬜ |
| tak            | Tak function: deeper recursion                    | ⬜ |
| branching      | if/else dispatch, short-circuit logic             | ⬜ |

### Data

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| var_access     | local vs global vs nested-scope lookup           | ⬜ |
| var_lifecycle  | create, assign and destroy; explicit delete where the language has it | ⬜ |
| strings        | concatenation, comparison, substrings, number <-> string | ⬜ |
| arrays         | sieve of Eratosthenes, sorting by hand, push/index | ⬜ |
| maps           | insert, lookup and remove with int and string keys | ⬜ |
| objects        | records with fields; binary-trees (allocation-heavy) | ⬜ |

### Interop (script <-> C++)

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| host_to_script | the host calls a script function N times         | ⬜ |
| script_to_host | the script calls a registered C++ function N times | ⬜ |
| data_exchange  | the host sets and reads script variables, strings and arrays | ⬜ |
| cpp_objects    | a bound C++ type (`Vec3`/`Entity`) used from the script. TeaScript has no class binding: its version (callbacks + passthrough data) is off by default and shown as "n/a" | ⬜ |
| buffer         | fill an RGBA image buffer (from the old BufferOverhead test) | ⬜ |

### Host API (C++ level)

Costs the host application pays for embedding, measured by the harness:

| Discipline      | Workload                                        | Status |
|-----------------|-------------------------------------------------|--------|
| engine_lifecycle | create and destroy an engine, empty and with the standard library | ⬜ |
| parse_compile   | generated sources of 100 / 1,000 / 10,000 functions. Headline is the total "source -> runnable"; sub-phases where available (TeaScript: parse + compile; ChaiScript: parse only; AngelScript and Lua: one combined step; Python: `ast.parse` + `compile()`) | ⬜ |
| registration    | register N C++ functions and variables          | ⬜ |
| value_conversion | C++ <-> script values: int, double, string, array | ⬜ |
| function_lookup | call by name vs through a cached handle         | ⬜ |
| engine_reset    | reset/reuse an engine                           | ⬜ |
| memory          | peak working set and allocation counts per engine and per workload | ⬜ |

### Later / optional

- **Build footprint:** C++ compile time of a minimal embedding translation unit, binary size of a
  minimal host. Measured by the build, not the harness, since it involves disk IO by nature.
- **Coroutines/generators:** TeaScript `CoroutineScriptEngine`, Lua coroutines, Python generators,
  AngelScript context suspend. ChaiScript has none.
- **Composite workload:** e.g. a "game tick" which mixes the above.
- **Idiomatic track:** each language at its best, native helpers allowed (see rule 4).
- **IO track:** e.g. loading precompiled scripts from disk.

## 8. Roadmap

- [x] **Phase 1a:** project setup: CMake with MSVC/GCC presets, harness, C++ reference, TeaScript
      runner with variants, `fib_recursive`.
- [x] **Phase 1b:** ChaiScript runners (default and no-threads); `loop` and `mandelbrot` benchmarks.
- [ ] **Phase 1c (next):** apply the decisions of 2026-10-04: series over n (`fib_recursive`:
      15 / 20 / 25), time budget for slow runs, ChaiScript `no-threads` as headline (README, report).
- [ ] **Phase 2:** Lua (LuaBridge3) and AngelScript runners.
- [ ] **Phase 3:** Python (pybind11) runner.
- [ ] **Phase 4:** the full discipline catalogue, including interop and host API.
- [ ] **Phase 5:** orchestration and report: run all runners, merge JSON, tables and charts.
- [ ] **Phase 6:** pin TeaScript 0.18.0, automate fetching the dependencies (optional), invite
      community reviews, publish.

## 9. Decision log

**2026-10-04**
- New, separate project `cpp-script-bench` (the old BenchmarkScripts repo stays as it is).
- CMake from the beginning. MSVC and GCC.
- TeaScript headline variant: TeaStackVM with O1. O0, O2 and AST eval selectable via `--variant`.
- Jinx deferred.
- `cpp_objects` for TeaScript: off by default, shown as "n/a".
- C++-level (host API) disciplines added. Build footprint later.
- ChaiScript: `develop` branch, both builds (default thread-safe, and `CHAISCRIPT_NO_THREADS`).
  Engine options `No_Load_Modules` and `No_External_Scripts`, so no disk IO.
- Benchmark scripts use plain assignments (`x = x + y`), no compound assignment, so all languages
  do the same operations.
- ChaiScript scripts use the value of the last expression instead of `return`, because ChaiScript
  implements `return` with a C++ exception (see `benchmarks/fib_recursive/README.md`). This gives
  ChaiScript its best case, and matches the TeaScript scripts.
- ChaiScript headline: the `no-threads` build, the closer match to TeaScript's single-threaded
  engine. The default (thread-safe) build stays available and is reported as a variant.
- `fib_recursive` is reported as a series over n (e.g. 15 / 20 / 25) instead of one default n,
  which makes ChaiScript's quadratic cost per call visible.
- Slow runs get a time budget: stop measuring after a configurable time, with at least 3 samples.

**2026-10-05**
- The checksum must be returned as an integer type (TeaScript: `Integer`, ChaiScript: `int`). The
  runners reject other types instead of converting them, so e.g. a float result can't pass by accident.

## 10. Open questions

- GCC: which version as baseline, and `-O2` or `-O3`? GCC 11.4 (WSL Ubuntu 22.04) works, but
  TeaScript needs fmt there (`CSB_FMT_DIR`), because GCC < 13 has no `std::format`.
  WSL still needs CMake (`sudo apt install cmake`; Ubuntu 22.04 ships 3.22.1, which is enough).
- Python: 3.15.0 or 3.14.x as the first version?
- Series (e.g. for `fib_recursive`): how to define them (per benchmark in the registry, or on the
  command line), and how to show them in the report?
- Time budget: the default budget per benchmark?
- Process priority and CPU pinning for less noise?
- Format of the published results: Markdown tables, charts, a web page?
