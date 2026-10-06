# cpp-script-bench: Plan

A living document: goals, rules, architecture and roadmap. Last updated: 2026-10-06.

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
   AngelScript, Lua. Python is the documented exception: the installed Python is used as it is
   (official or distro binaries, built separately, usually with PGO). The report records its exact
   version and build. A library may add a flag its own build requires for correct code, e.g.
   `-fno-strict-aliasing` for AngelScript with GCC.
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
| load     | make the script runnable; sub-phases where the language can separate them: `parse`, `compile`, `top_level`; `parse_compile` where parsing and compiling is one step (AngelScript, Lua) |
| first    | the very first call of `bench( n )`                       |
| execute  | R measured calls of `bench( n )` after W warm-up calls: **the headline numbers** |
| teardown | destroy the engine                                        |

### Protocol

Each script defines a function `bench( n )` which returns an integer checksum. The host calls it
through the language's normal C++ API, once per sample. The cost of this one call is negligible
compared to the workload. The interop disciplines measure call costs explicitly.

**Warm-up engine.** Before the first measured run of each variant, the runner makes one throw-away
run: create an engine, load the first selected benchmark, call `bench( n )` once with its smallest
n, destroy the engine. Nothing of it is measured or reported. Without it, the first run of a process
is slower than the following ones (cold caches, first use of the code, the libraries and their
files), e.g. Python's startup with MSVC: ~31 ms instead of ~5.4 ms.

### Series

A benchmark can define a series of workload sizes in the registry instead of a single n
(`fib_recursive`: n = 15 / 20 / 25). Each n is a separate run with a fresh engine, so the results
show how the run time grows with n. `--n` replaces the series of all selected benchmarks
(e.g. `--n 20` or `--n 10,20`).

### Statistics

Defaults: 3 warm-up calls, 10 measured calls; both can be changed on the command line.

**Time budget** (default 30 s, `--budget`, 0 = unlimited). It limits all calls of `bench( n )` of
one run (one benchmark, one n), starting with the first call:
- A call is only made if it is expected to finish within the budget, estimated with the duration of
  the latest call.
- At least 3 calls are always measured (fewer only if `--runs` is lower). A warm-up call is only made
  if the budget also has room for those 3.
- A run which was cut short is marked: `budget_limited` in the JSON, "(time budget)" on the console.

With the defaults the budget only takes effect when one call takes longer than about 2 s.
Example: ChaiScript's `fib_recursive` with n = 25 takes ~18 s per call. With the budget it makes
1 first + 0 warm-up + 3 measured calls (~72 s) instead of 1 + 3 + 10 (~4 min).

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
  The interpreter uses the isolated configuration without the `site` module, so no site-packages
  or `.pth` files are read.

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
`no-threads`, with `CHAISCRIPT_NO_THREADS`). AngelScript likewise: `csb_angelscript` (variants
`default` and `no-line-cues`, thread-safe) and `csb_angelscript_nothreads` (variants `no-threads` and
`no-threads-no-line-cues`, with `AS_NO_THREADS`). Without line cues means compiled with
`asEP_BUILD_WITHOUT_LINE_CUES`.

**Headline variant.** Each language has one variant for the main comparison: TeaScript `vm-O1`,
ChaiScript `no-threads`, AngelScript `no-threads-no-line-cues`; Lua has only one. It is independent
of the default variant. The runners mark it in their output and in the JSON (`"headline": true`).

**Scripts are separate files** (not strings in C++ code), so they get syntax highlighting and each
community can review them.

## 5. Languages and versions

| Language    | Version                                   | Embedded via                       | Notes |
|-------------|-------------------------------------------|------------------------------------|-------|
| TeaScript   | HEAD (0.18.0 pre-release); pinned to 0.18.0 after its release | native API | header-only |
| ChaiScript  | `develop` HEAD, pinned by commit hash (now `f635b51e`, 2026-05-02; reports itself as 7.0.0) | native API | the last release (6.1.0) is from 2018; headline build: `no-threads` |
| AngelScript | 2.38.0 (Aug 2026)                         | native API + string, array, dictionary, math add-ons | compiled library; headline: `AS_NO_THREADS`, without line cues |
| Lua         | 5.5.1                                     | Lua C API; LuaBridge3 `3.0-rc12` for the bindings (Phase 4) | compiled library, as C |
| Python      | the installed system Python (any version pybind11 supports, 3.9+) | pybind11 3.1.0 (embedded interpreter) | prebuilt; exact version and build recorded |
| C++         | n/a                                       | reference                          | |

Deferred: Jinx (limited core library, dormant). It may be added later if it can run most disciplines.

**Pinning policy:** every library is used at a defined version or commit. The result files record
the exact git state. A library only moves forward when that is done on purpose.

## 6. Toolchains

| Toolchain | Optimization            | Instruction set     |
|-----------|-------------------------|---------------------|
| MSVC 2022 | `/O2 /Ob2` (Release)    | `/arch:AVX2`        |
| GCC 11    | `-O2`                   | `-march=x86-64-v3`  |

- No LTO and no PGO for code built from source.
- Results are only compared within one toolchain on one machine. MSVC and GCC numbers never appear
  in the same chart.
- GCC is tested on WSL and on a Linux VPS:
  - WSL: Ubuntu 22.04, GCC 11.4, Python 3.10. For local tests. It stays as it is, because it is
    also the primary Linux build system of TeaScript.
  - VPS: Ubuntu 24.04, GCC 13, Python 3.12. Controlled via ssh. How to use it best is decided after
    Phase 3.
- GCC baseline: GCC 11 (11.4 on WSL Ubuntu 22.04). TeaScript needs fmt there (`CSB_FMT_DIR`),
  because GCC < 13 has no `std::format`. GCC 13 may become the baseline later.

## 7. Disciplines

Status: ✅ implemented, ⬜ planned.

### Core

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| fib_recursive  | recursive `fib( n )`: call and recursion overhead; series n = 15 / 20 / 25 | ✅ |
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
- **Python JIT:** the official Python 3.14 builds for Windows contain the experimental JIT (off by
  default, `PYTHON_JIT=1`). It could become an optional variant outside the main comparison (rule 8).

## 8. Roadmap

- [x] **Phase 1a:** project setup: CMake with MSVC/GCC presets, harness, C++ reference, TeaScript
      runner with variants, `fib_recursive`.
- [x] **Phase 1b:** ChaiScript runners (default and no-threads); `loop` and `mandelbrot` benchmarks.
- [x] **Phase 1c:** apply the decisions of 2026-10-04: series over n (`fib_recursive`:
      15 / 20 / 25), time budget for slow runs, headline variants marked (ChaiScript `no-threads`).
- [x] **Phase 2:** Lua and AngelScript runners, with scripts for the three core benchmarks.
- [x] **Phase 3:** Python (pybind11) runner.
- [ ] **Phase 4 (next):** the full discipline catalogue, including interop and host API.
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
- Series are defined per benchmark in the registry (`BenchmarkInfo::series`). `--n` replaces them
  with a list from the command line. How to show series in the report is still open.
- Time budget: 30 s by default per run (one benchmark, one n), at least 3 measured calls; the rules
  are in section 3. 30 s rather than 60 s: with the defaults the budget only takes effect for calls
  longer than about 2 s, and ChaiScript's fib(25) needs ~72 s with either budget, because of the
  3 measured calls.
- Each runner marks its headline variant (console and JSON), independent of the default variant.
- GCC: `-O2`. Baseline is GCC 11 from WSL (Ubuntu 22.04), the simple choice for now; it may move
  to GCC 13 later, which supports C++20 better.
- Python: start with 3.14.x.

**2026-10-06**
- Python: the runner uses the Python installed on the system instead of a fixed version, so the
  suite is easier to use. CMake finds the default with `find_package( Python COMPONENTS Interpreter
  Development.Embed )`; another installed Python can be chosen with the standard hints
  `Python_ROOT_DIR` or `Python_EXECUTABLE`. Any version pybind11 supports is accepted.
- The report records the full `sys.version` (including the compiler Python was built with), and
  whether it is a free-threaded build and whether the JIT is on. The headline uses the default
  build with the GIL, and the experimental JIT stays off (rule 8).
- Python's speed differs a lot between versions (3.11 was a big step). Published results use a
  current Python. CMake can't provide one quickly on every platform: there is an official NuGet
  package for Windows, but no official binaries for Linux.
- Local Linux tests run on WSL Ubuntu 22.04 (Python 3.10), which stays unchanged. The VPS (Ubuntu
  24.04, GCC 13, Python 3.12) is controlled via ssh; how to use it best is decided after Phase 3.
- Lua: compiled as C, like the official distribution (on Linux with `LUA_USE_LINUX`, like
  `make linux`). The runner calls `bench` through the Lua C API (`lua_getglobal` + `lua_pcall`), the
  canonical way to call Lua from C and C++. LuaBridge3 (pinned to `3.0-rc12`, the latest release; there
  is no final 3.0 yet) is only needed for the bindings in Phase 4. Its `LuaRef::call<LuaRef>()` does
  not compile with MSVC (ambiguous overload, also on master).
- Lua's standard library is opened without `package` (`require`), and `dofile` and `loadfile` are
  removed, matching ChaiScript's `No_Load_Modules` and `No_External_Scripts`.
- Lua scripts: helper functions are `local function` (idiomatic; called through an upvalue instead of
  the global table), `bench` is global. Constants are `<const>` where TeaScript uses `const`.
- AngelScript: built twice like ChaiScript (default thread-safe, and `AS_NO_THREADS`), each with and
  without line cues. Headline: `no-threads-no-line-cues`. The AngelScript manual ("Fine tuning")
  recommends both when they are not needed, and it matches ChaiScript's `no-threads` headline.
  Automatic garbage collection stays on, because GC work during a call counts (rule 10).
  The runner reuses one context and looks `bench` up once, as the manual recommends.
- AngelScript with GCC: `-fno-strict-aliasing`, like the SDK's own makefile (`projects/gnuc`);
  without it GCC warns that the VM's type punning may be broken by the optimizer.
- Sub-phase name `parse_compile` for languages which parse and compile in one step.
- Python runner (pybind11 v3.1.0, the latest release):
  - One interpreter per engine: startup is `Py_InitializeFromConfig()`, teardown `Py_Finalize()`.
    Restarting the interpreter in one process works for the standard library.
  - The isolated configuration (recommended for embedding by the Python documentation): environment
    variables like `PYTHONPATH` and `PYTHON_JIT` are ignored. No `site` module (like `python -S`):
    site-packages and `.pth` files are machine-dependent disk IO and not part of the standard library.
  - The Python home is set to the installation CMake found (`sys.base_prefix`). On Windows its DLL is
    copied next to the runner, so exactly that Python is used, independent of `PATH`.
  - The runner refuses to run if the JIT is enabled (`sys._jit`, Python 3.14+). A free-threaded build
    is reported as the variant `free-threaded`, which is not a headline. `Py_GetVersion()` (the full
    `sys.version`, including the compiler) is the version string.
  - load: `Py_CompileString()` (`parse_compile`), then `PyEval_EvalCode()` in a fresh module-like
    namespace (`top_level`). The result must be exactly an `int` (not a `bool`).
  - Python scripts: module-level functions, all hot variables are locals.
- The first engine in a process started slower than the following ones (cold caches, first use of
  the code, the libraries and their files), e.g. Python with MSVC ~31 ms vs ~5.4 ms, Lua ~130 us vs
  ~45 us. Solution: one throw-away warm-up run per variant before the first measured run (section 3).

## 10. Open questions

- TeaScript host call (decide by Phase 4 at the latest): the runner calls `bench( n )` with
  `CallFuncEx`. In compile mode this looks up the function by name and creates a new VM machine
  for each call (`Machine<false>`, without suspend support), and runs it with `ExecSubroutine`.
  `ExecuteProgram` would use the engine's own machine (`Machine<true>`) and `Exec`. Which path is
  the fair counterpart to the competitors' host calls? Compare both paths with a measurement first.
- Series: how to show them in the report (e.g. one row per n, or a chart over n)? Phase 5; the JSON
  already has one entry per n.
- Process priority and CPU pinning for less noise? Phase 5, before the published runs.
- Format of the published results: Markdown tables, charts, a web page? Phase 5.
