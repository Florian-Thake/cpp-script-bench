# cpp-script-bench: Plan

A living document: goals, rules, architecture and roadmap. Last updated: 2026-10-29.

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
| bind     | register what the script needs from the host: C++ functions, types and data (only the interop benchmarks) |
| load     | make the script runnable; sub-phases where the language can separate them: `parse`, `compile`, `top_level`; `parse_compile` where parsing and compiling is one step (AngelScript, Lua) |
| first    | the very first call of `bench( n )`                       |
| execute  | R measured calls of `bench( n )` after W warm-up calls: **the headline numbers** |
| teardown | destroy the engine                                        |

Most host API benchmarks (group `host`, Phase 4d) are different: they measure a C++ operation of the
embedding API (e.g. creating an engine). Each sample is one call of the adapter's `run_host()`, which
measures the operation itself and reports its sub-phases; the runner keeps the first call, the
warm-up, the samples, the time budget and the statistics as for `bench( n )`. The registry marks them
with `Sampling::run_host`. `call_by_name` and `call_by_handle` use the normal phases, like
`host_to_script`: the measured `call_bench( n )` is the host's loop of calls.

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
runner/       orchestrates all runners for a result set (run.py), merges their JSON; tables and charts (later)
```

**One executable per language.** Reasons:
- Heap state and caches stay isolated between languages.
- Peak memory per language becomes meaningful.
- A crash (e.g. a stack overflow from deep recursion) doesn't end the whole run.
- No header or symbol clashes (e.g. `Python.h`).
- Each user only builds the languages they have.

**Adapter interface** (`harness/include/csb/language.hpp`): `create_engine()`, `bind()`, `load()`,
`call_bench( n )`, `collect_garbage()`, `destroy_engine()`, and `supports()`, which lets a variant
report a benchmark as "n/a". The host API benchmarks use `run_host( info, n, source )` instead: one
call is one sample, in which the adapter prepares what the sample needs (e.g. a fresh engine) without
measuring it, measures the C++ operation itself, and returns its duration, its sub-phases and a
checksum. Every runner's `main()` provides a `LanguageEntry` with its variants, and calls
`csb::run_main()`. The interop and host API benchmarks share their host side (C++ functions, types,
data, measuring loops) in `benchmarks/<name>/host.hpp`, which the C++ reference and the runners use.

**Variants.** One language can have several ways of running code. They are selected with
`--variant <name>` or `--variant all`, without rebuilding. TeaScript: `vm-O0`, `vm-O1` (default),
`vm-O2`, `ast`. The CMake option `CSB_TEASCRIPT_DEFAULT_VARIANT` changes the default.
Variants which need a different build become separate executables: ChaiScript is built as
`csb_chaiscript` (variant `default`, thread-safe) and `csb_chaiscript_nothreads` (variant
`no-threads`, with `CHAISCRIPT_NO_THREADS`). AngelScript likewise: `csb_angelscript` (variants
`default` and `no-line-cues`, thread-safe) and `csb_angelscript_nothreads` (variants `no-threads` and
`no-threads-no-line-cues`, with `AS_NO_THREADS`). Without line cues means compiled with
`asEP_BUILD_WITHOUT_LINE_CUES`. Lua and Python bind C++ with their binding libraries (LuaBridge3,
pybind11) in the variant without a name, and with the plain C API in the variant `c-api`, which runs
only the interop benchmarks (a free-threaded Python: `free-threaded` and `free-threaded-c-api`).

**Headline variant.** Each language has one variant for the main comparison: TeaScript `vm-O1`,
ChaiScript `no-threads`, AngelScript `no-threads-no-line-cues`, Lua and Python the variant without a
name. It is independent of the default variant. The runners mark it in their output and in the JSON
(`"headline": true`).

**Scripts are separate files** (not strings in C++ code), so they get syntax highlighting and each
community can review them.

## 5. Languages and versions

| Language    | Version                                   | Embedded via                       | Notes |
|-------------|-------------------------------------------|------------------------------------|-------|
| TeaScript   | HEAD (0.18.0 pre-release); pinned to 0.18.0 after its release | native API | header-only |
| ChaiScript  | `develop` HEAD, pinned by commit hash (now `f635b51e`, 2026-05-02; reports itself as 7.0.0) | native API; ChaiScript_Extras `math.hpp` (`master` at `dd116d7`, 2026-04-17) | the last release (6.1.0) is from 2018; headline build: `no-threads` |
| AngelScript | 2.38.0 (Aug 2026)                         | native API + string, array, dictionary, math add-ons (math with `AS_USE_FLOAT=0`: double) | compiled library; headline: `AS_NO_THREADS`, without line cues |
| Lua         | 5.5.1                                     | Lua C API; LuaBridge3 `3.0-rc12` for the bindings of C++ (the C API in the variant `c-api`) | compiled library, as C |
| Python      | the installed system Python (any version pybind11 supports, 3.9+) | pybind11 3.1.0 (embedded interpreter, bindings of C++; the C API in the variant `c-api`) | prebuilt; exact version and build recorded |
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
- GCC is tested with GCC 11.4 (Ubuntu 22.04) and GCC 13.3 (Ubuntu 24.04).
- GCC baseline: GCC 11. TeaScript needs fmt there (`CSB_FMT_DIR`), because GCC < 13 has no
  `std::format`. GCC 13 may become the baseline later.

## 7. Disciplines

Status: ✅ implemented, ⬜ planned.

### Core

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| fib_recursive  | recursive `fib( n )`: call and recursion overhead; series n = 15 / 20 / 25 | ✅ |
| loop           | counting loop, body `sum = sum + (i & 7)`         | ✅ |
| int_arith      | Collatz step counts for 1..n: `%` and integer division | ✅ |
| mandelbrot     | float arithmetic: Mandelbrot grid, checksum = integer iteration count | ✅ |
| math_builtins  | sum of `floor( sqrt( i ) )`, an exact integer checksum | ✅ |
| calls          | tiny functions with 0, 1 and 3 arguments, called in one loop | ✅ |
| tak            | Tak function: recursion with 3 arguments and calls as arguments; series n = 4 / 5 / 6 | ✅ |
| branching      | if/else dispatch, short-circuit logic             | ✅ |

### Data

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| var_global     | the loop of `loop` with a script-level (global) accumulator | ✅ |
| var_nested     | four nested loops; the body reads variables of four enclosing scopes | ✅ |
| var_lifecycle  | new block-local variables in every loop iteration | ✅ |
| sieve          | sieve of Eratosthenes on an array built by appending: push, index read/write | ✅ |
| sort           | insertion sort by hand of generated values       | ✅ |
| byte_buffer    | fill a byte buffer, then an Adler-32 style checksum over it | ✅ |
| maps           | string keys: insert, lookup, remove, membership  | ✅ |
| strings        | number -> string, concatenation, comparison, substring, string -> number | ✅ |
| records        | field reads and writes of one record (no allocation) | ✅ |
| binary_trees   | allocation-heavy: build and check binary trees of records; series over the depth | ✅ |

The catalogue entries `var_access`, `arrays` and `objects` were split into several benchmarks, and
`byte_buffer` was added (decision log 2026-10-14).

### Interop (script <-> C++)

| Discipline     | Workload                                         | Status |
|----------------|--------------------------------------------------|--------|
| script_to_host | the loop of `calls`, but the functions are registered C++ functions | ✅ |
| host_to_script | the mirror: a C++ loop calls the script functions of `calls` | ✅ |
| cpp_objects    | the loop of `records` on a bound C++ type (`Vec3i`). TeaScript has no class binding: "n/a" | ✅ |
| buffer         | fill an RGBA image buffer owned by the host (from the old BufferOverhead test) | ✅ |
| string_exchange | strings from the host to a script function and back | ✅ |
| array_exchange | arrays from the host to a script function and back (converted both ways); series over the size | ✅ |

The catalogue entry `data_exchange` was split into `string_exchange` and `array_exchange` (decision
log 2026-10-25).

### Host API (C++ level)

Costs the host application pays for embedding, measured by the harness:

| Discipline         | Workload                                        | Status |
|--------------------|-------------------------------------------------|--------|
| engine_lifecycle   | create and destroy an engine with the standard library | ✅ |
| engine_minimal     | the same with the smallest engine the API allows | ✅ |
| parse_compile      | generated sources of 100 / 1,000 / 10,000 functions. Headline is the total "source -> runnable"; sub-phases where available (TeaScript: parse + compile; ChaiScript: parse only; AngelScript, Lua and Python: one combined step) | ✅ |
| register_functions | register N C++ functions                        | ✅ |
| register_variables | register N host variables                       | ✅ |
| call_by_name       | the host calls script functions by name, among M functions | ✅ |
| call_by_handle     | the same through handles looked up once         | ✅ |
| engine_reset       | reset an engine and load the script again       | ✅ |

The catalogue entries `registration` and `function_lookup` were split, `engine_minimal` was added, and
`memory` moved to Phase 5 (decision log 2026-10-28).

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
- [x] **Phase 4:** the full discipline catalogue, in four subphases. Each discipline is implemented for
      all languages at once (one commit each).
  - [x] **4a Core:** `int_arith`, `calls`, `tak`, `branching`, `math_builtins` (design round
        2026-10-09, implemented 2026-10-14).
  - [x] **4b Data:** `var_global`, `var_nested`, `var_lifecycle`, `sieve`, `sort`, `byte_buffer`,
        `maps`, `strings`, `records`, `binary_trees` (design round 2026-10-14, implemented
        2026-10-18).
  - [x] **4c Interop:** `script_to_host`, `host_to_script`, `cpp_objects`, `buffer`,
        `string_exchange`, `array_exchange` (design round and implementation 2026-10-25). Extends
        `csb::Language`; brings in LuaBridge3.
  - [x] **4d Host API:** `engine_lifecycle`, `engine_minimal`, `parse_compile`,
        `register_functions`, `register_variables`, `call_by_name`, `call_by_handle`, `engine_reset`
        (design round and implementation 2026-10-28). A new kind of benchmark: the adapter times C++
        operations (`run_host()`).
- [ ] **Phase 5 (next):** orchestration and report: run all runners, merge JSON, tables and charts; `memory`
      (peak working set and allocation counts per engine and per workload, one process per run).
      Design round part 1 (where and how results are measured and published) on 2026-10-29. Steps:
      [x] system info in the JSON, [x] the orchestrator (`runner/run.py`), [ ] `memory` (the peak
      memory of each process is recorded; design round part 2), [ ] the report generator, [ ] the noise
      check, [ ] the first result sets (marked "preview" until TeaScript 0.18.0).
- [ ] **Phase 6:** pin TeaScript 0.18.0, automate fetching the dependencies (optional), invite
      community reviews, publish. Before that: adapt `engine_reset` to the parameter of TeaScript's
      `ResetState()` once it exists (decision log 2026-10-28).

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
- GCC: `-O2`. Baseline is GCC 11 (Ubuntu 22.04), the simple choice for now; it may move
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
- Linux tests use the distribution's Python: 3.10 (Ubuntu 22.04) and 3.12 (Ubuntu 24.04).
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

**2026-10-09**
- Both warm-ups stay. They remove different cold costs: the warm-up engine the process-wide ones
  (for startup, load and first of the first run), the warm-up calls the per-engine ones (for the
  headline). Measured with `--warmup 0` (MSVC): only the first call of an engine is slower, and only
  noticeably for short calls (AngelScript `loop` n = 1000: 26 us first, 11 us after; Python: 31 us vs
  29 us). One warm-up call would be enough today; 3 stay as a margin, e.g. for Phase 4's
  allocation-heavy disciplines.
- GCC 13.3 (Ubuntu 24.04) is tested as well. TeaScript uses `std::format` there, not fmt.
- The git versions in the result files (cpp-script-bench, TeaScript, ChaiScript, pybind11) are
  determined on every build, not only when CMake configures. Before, a build after a new commit
  without a re-run of CMake reported the previous commit. `csb_target_git_version()` generates a
  header, which is only rewritten when the version changed, so nothing is recompiled otherwise.
- Machine-specific setup (paths, hosts, access) stays out of the repo; only toolchain and OS
  versions are documented here.
- Phase 4 is split into four subphases by category, in this order: 4a Core and 4b Data fit the
  existing `bench( n )` protocol, 4c Interop extends the adapter interface, and 4d Host API reuses
  its binding code and needs a new measurement model. The disciplines of one category share their
  fairness decisions (e.g. the natural container per language), so each subphase starts with one
  design round. Within a subphase, one discipline is implemented for all languages at a time (one
  commit each), so the scripts can be reviewed per discipline.
- TeaScript host call (was an open question): the runner keeps `CallFuncEx`. It is TeaScript's API
  for calling a script function from C++, the counterpart of `lua_pcall`, AngelScript's `Execute()`
  and the function handles of pybind11 and ChaiScript. Users call script functions this way, so the
  benchmark does too. (`ExecuteProgram` is meant for running whole programs.)
- 4a design round. The sizes are tuned during the implementation: the slowest headline variant
  should need 0.1-0.5 s per call, like `loop` and `mandelbrot` (except ChaiScript's recursion).
  - `int_arith`: Collatz step counts for s = 1..n:
    `x = s; while x != 1: (if x % 2 == 0: x = x / 2 else: x = 3 * x + 1); total = total + 1`.
    `%` and integer division (`//` in Python and Lua, `mod` and `/` in TeaScript), no bit
    operations (`loop` covers them). n = 10,000 (checksum 849,666). The values stay below 2^31
    for all n < 113,383.
  - `calls`: `f0() = 1`, `f1( a ) = a & 7`, `f3( a, b, c ) = (a + b + c) & 7`, called in the loop
    of `loop`: `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )`. Named functions, called by name like
    `fib`. One combined benchmark, not one per argument count. n = 100,000 (checksum 800,000).
  - `tak`: if `y < x`, `tak( x, y, z )` returns
    `tak( tak( x - 1, y, z ), tak( y - 1, z, x ), tak( z - 1, x, y ) )`, else `z`;
    `bench( n ) = tak( 3n, 2n, n )`. Series n = 4 / 5 / 6 (results 5 / 10 / 7;
    1,733 / 10,345 / 63,609 calls). Not larger: ChaiScript's cost of a recursion is quadratic in
    its calls (n = 6 ~11 s per call; n = 7 would take minutes per call). tak is not deeper than
    fib; it adds 3 arguments and calls as arguments.
  - `branching`: `k = i & 7`; an if/else chain `k == 0` (+1), `k == 1` (+2), `k < 4` (+3),
    else (+4); then `if (k > 2 and k < 6) or k == 7` (+5). No switch or match (TeaScript and Lua
    have none); explicit parentheses, so operator precedence plays no role. n = 1,000,000
    (checksum 5,625,000).
  - `math_builtins`: the sum of `floor( sqrt( i ) )` for i = 1..n, accumulated in the type the
    functions return and converted to an integer once at the end. Exact, because `sqrt` is
    correctly rounded (IEEE 754) and `floor` is exact, so no tolerance is needed (this answers
    the open question). `sin` is dropped: TeaScript has none, and it is not correctly rounded.
    n = 100,000 (checksum 21,032,170).
- Math functions per language: Python `from math import sqrt, floor` and Lua
  `local sqrt, floor = math.sqrt, math.floor` (the idiomatic fast forms), AngelScript the
  scriptmath add-on, TeaScript the Core Library's convenience functions `sqrt` and `floor` (not the
  native `_sqrt` and `_trunc`), ChaiScript the `math.hpp` module of ChaiScript_Extras. That is
  ChaiScript's official module for `<cmath>` (its standard library has no math functions), the
  counterpart of AngelScript's add-on; pinned to a commit when it is added. Its overload dispatch
  with an `int` argument is checked during the implementation.
- ChaiScript's quadratic call cost only affects recursion. The saved parameters are cleared
  whenever the script call depth returns to 0, and the host calls `bench` through a function
  handle, so calls made directly from `bench` stay linear (measured: 10k / 20k / 40k calls of a
  1-argument function in 7.5 / 15 / 31 ms). The note in `fib_recursive/README.md` is corrected.

**2026-10-14**
- `int_arith`: n = 5,000 instead of 10,000 (checksum 387,968). With 10,000 the slowest headline
  (ChaiScript `no-threads`, MSVC) needed ~600 ms per call, above the target of 0.1-0.5 s.
- AngelScript's math add-on (scriptmath) registers only float versions of its functions by default
  (`AS_USE_FLOAT` is 1 unless defined, e.g. only `float sqrt(float)`). The add-on is compiled with
  `AS_USE_FLOAT=0`, its own switch for the double versions, so AngelScript computes in double
  precision like the other languages. A float sum could not even hold the `math_builtins` checksum
  exactly (it is above 2^24).
- ChaiScript_Extras is pinned to `master` at `dd116d7` (2026-04-17). Its `math.hpp` module is loaded
  in `create_engine()` for every benchmark, like AngelScript's math add-on, so its cost is part of
  ChaiScript's startup. It is required for the ChaiScript runners (`CSB_CHAISCRIPT_EXTRAS_DIR`), so
  the startup numbers don't depend on the build.
- `tak`: ChaiScript needs 3.6 s per call for n = 6 (MSVC), not ~11 s as estimated in the design
  round. n = 7 (395,757 calls) would still take minutes per call, so the series stays 4 / 5 / 6.
  The other languages need at most ~50 ms for n = 6, below the usual target; the series is limited
  by ChaiScript's cost of a recursion.
- `branching`: n = 300,000 instead of 1,000,000 (checksum 1,687,500). With 1,000,000 the slowest
  headlines (TeaScript `vm-O1` and ChaiScript `no-threads`, MSVC) needed ~1.1 s per call.
- Python's chained comparison (`2 < k < 6`) is not used: the spec's `and` form evaluates the same
  comparisons as in the other languages, and it is the faster form in CPython (the chained one
  was ~9% slower in this loop with Python 3.14).
- `math_builtins`: n = 100,000 as planned (checksum 21,032,170).
  - ChaiScript does not convert an `int` argument for the math module's functions (overloads for
    `float`, `double` and `long double`): `sqrt(i)` fails. The script converts with `i + 0.0`, the
    fastest usual form (MSVC, per call: `to_double(i)` 161 ms, `double(i)` 155 ms, `i + 0.0`
    122 ms). TeaScript's `sqrt` does the same `+ 0.0` internally.
  - Loading the math module raises ChaiScript's startup by ~0.6 ms (MSVC: ~3.2 to ~3.8 ms).
  - Lua's `math.floor` and Python's `math.floor` return integers, so their sums are integers; the
    other languages sum doubles and convert once at the end.
  - Python's `math` is a built-in module in the tested builds, so `from math import ...` in the
    script's top level reads nothing from disk.
- `csb_target_git_version()` creates one custom target per header, so a runner can record several
  git versions (ChaiScript and ChaiScript_Extras).
- 4b design round. The six catalogue entries become ten benchmarks: `var_access` is split into
  `var_global` and `var_nested`, `arrays` into `sieve` and `sort`, `objects` into `records` and
  `binary_trees`, and `byte_buffer` is new. One combined number would hide which part is slow. The
  sizes are starting points, tuned during the implementation as in 4a.
- Common rules of the Data benchmarks:
  - The natural container per language. Arrays: TeaScript Tuple, ChaiScript Vector, AngelScript
    `array<T>`, Lua table as a sequence, Python list. Maps: TeaScript Map, ChaiScript Map,
    AngelScript `dictionary`, Lua table, Python dict. Records: TeaScript named Tuple, ChaiScript
    `class`, AngelScript script class, Lua table with named fields, Python class with `__init__`.
  - Maps use string keys only. ChaiScript's Map and AngelScript's dictionary have no int keys, and
    Lua stores dense int keys in the array part of a table, so an int-key test would measure arrays
    there.
  - Containers grow by appending in a loop. No native fill or preallocation (`[x] * n`, Lua 5.5's
    `table.create`, `array<T>(n, v)`, `resize`), see rule 4. In Lua, `t[i] = v` with the next index
    counts as appending. Exception: `byte_buffer` (below). Changed on 2026-10-18: a capacity is
    reserved where the language can.
  - Lua indexes from 1; the index ranges of the specs shift accordingly.
  - Number -> string in each language's natural form: TeaScript `"item" % i`, ChaiScript
    `"item" + to_string(i)`, AngelScript `"item" + i`, Lua `"item" .. i`, Python `f"item{i}"`.
    String -> number: TeaScript `d as i64` (the cast operator; `_strtonum( d )` ends in the same
    conversion, unary `+d` is a workaround), ChaiScript `to_int(d)`, AngelScript `parseInt(d)`, Lua
    `tonumber(d)`, Python `int(d)`.
  - Library functions are cached in locals where that is the idiomatic fast form (e.g. Lua
    `local sub = string.sub`), like in `math_builtins`.
  - Explicit delete (TeaScript `undef`, Python `del`) is dropped from `var_lifecycle`: only these
    two languages have it, and deleting locals is unusual in both.
  - Methods are out of scope: TeaScript has no classes and no automatic `this` (functions in named
    Tuple elements could only emulate them). An OOP discipline may follow when TeaScript gets class
    support.
- The specs (checksums computed with a Python model of each spec):
  - `var_global`: the loop of `loop`, but `sum` is a script-level variable, read and written in
    `bench`: TeaScript top-level `def`, ChaiScript `global` (a top-level `var` is not visible in
    functions), AngelScript global `int`, Lua global (no `local`), Python module variable with
    `global`. n = 1,000,000 (checksum 3,500,000).
  - `var_nested`: four nested counting loops over a, b, c, d = 0..n-1 with the body
    `sum = sum + ((a + b + c + d) & 7)`, which reads variables of four enclosing scopes. Shows
    whether the cost of a lookup grows with the scope depth (TeaScript and ChaiScript resolve names
    at runtime, the others at compile time). n = 30 (810,000 iterations, checksum 2,834,976).
  - `var_lifecycle`: per iteration `a = i & 7; b = a + 1; b = b * a; sum = sum + b`, with new
    block-local variables (`const`/`def`, `var`, `local`, `int`; Python has no block scope).
    n = 1,000,000 (checksum 21,000,000).
  - `sieve`: append n + 1 flags `true`; for i = 2 while i * i <= n: if `flags[i]`, clear the
    multiples from i * i; then count the primes <= n. The bound i * i <= n keeps all values below
    2^31. n = 300,000 (checksum 25,997).
  - `sort`: append n values of the generator `x = (x * 75 + 74) % 65537` (x = 1 at the start),
    sort them with insertion sort, checksum the sum of a[i] * (i & 7) (fits 32 bit for n <= 4,681).
    n = 1,000 (255,665 shifts, checksum 116,225,934).
  - `byte_buffer`: a byte buffer of n zero bytes, filled with the low bytes of the generator of
    `sort`, then an Adler-32 style checksum over it (`a = (a + buf[i]) % 65521`,
    `b = (b + a) % 65521`, a = 1 and b = 0 at the start); returns a + b. n = 300,000
    (checksum 101,572).
    - The buffer is created at its full size, the natural form of a fixed-size buffer (TeaScript's
      Buffer cannot grow past its capacity): TeaScript `_buf( n )` and `_buf_resize`, Python
      `bytearray(n)`, AngelScript `array<uint8>(n)`. Lua and ChaiScript have no byte type: they use
      their array, built by appending in the write loop, with the values masked to 0..255. They are
      included (documented), not "n/a".
    - Multi-byte access (`_buf_get_u32` etc.) is left out: only TeaScript and Python can do it in
      place. It would fit the idiomatic track.
    - Not the same as 4c's `buffer`, which is owned by the host.
  - `maps`: build the keys "k0" .. "k{n-1}" once into an array; insert key -> i, look up all keys,
    remove every second key, count which keys are still members. n = 10,000 (checksum 50,000,000;
    fits 32 bit for n < 65,536).
  - `strings`: per i: `s = "item" + i`; count `s < prev`; add the length of `s`; `d` = substring of
    `s` from index 4 (the digits); add `int(d) & 7`; `prev = s`. n = 100,000 (checksum 1,238,894).
  - `records`: one record with the fields x, y, z (all 0), created once; per iteration
    `p.x = p.x + (i & 7); p.y = p.y + (p.x & 3); p.z = p.z + (p.y & 1)`; returns x + y + z. Field
    access only, no allocation in the loop. n = 1,000,000 (checksum 5,250,000).
  - `binary_trees`: the Benchmarks Game algorithm with record nodes (left, right): `make( d )`
    builds a full tree of depth d, `check` counts its nodes. `bench( n )` builds and checks
    2^(16 - n) trees of depth n, ~131k nodes per call for every n. Series n = 4 / 8 / 12 (checksums
    126,976 / 130,816 / 131,056). Measures allocation and garbage collection; the series shows the
    effect of the tree size at the same volume (and ChaiScript's cost of a recursion).

**2026-10-18**
- TeaScript: 4b is implemented and tuned with `v0.17.0-24-gb758046` (one commit after the one used
  for 4a; it adds the `breakpoint` statement).
- `var_global`: n = 1,000,000 as planned, the same n as `loop`, so the two results compare directly
  (the difference is the cost of the global variable). The slowest headline, ChaiScript
  `no-threads` with MSVC, needs ~0.51 s per call (GCC: ~0.2 s), just above the target of 0.1-0.5 s;
  the direct comparison is worth more.
- Lua's global is a field of `_ENV`, as planned. A top-level `local` would be an upvalue of `bench`,
  the usual form of module-level state in Lua, but not a global (documented in the README).
- `var_nested`: n = 25 instead of 30 (390,625 iterations, checksum 1,367,184). With 30 the slowest
  headline (ChaiScript `no-threads`, MSVC) needed ~0.74 s per call; with 25 it needs ~0.35 s (GCC:
  TeaScript `vm-O1` ~0.16 s, the slowest there).
- `var_lifecycle`: n = 300,000 instead of 1,000,000 (checksum 6,300,000). With 1,000,000 the
  slowest headlines (MSVC) needed ~1.2 s (ChaiScript `no-threads`) and ~0.74 s (TeaScript `vm-O1`)
  per call; with 300,000 ~0.36 s and ~0.22 s. TeaScript defines `a` with `const`, like `k` in
  `branching`.
- Capacity reserve (changes the rule of 2026-10-14 on growing containers): where the language can
  reserve the capacity of a container without changing its size, the script does so before the
  appends: AngelScript `reserve`, ChaiScript `reserve`, Lua `table.create`. It is the natural form
  when the final size is known, and the appends are still made one by one. TeaScript (Tuple) and
  Python (list) have no such function; when TeaScript gets one, its scripts use it. Native fill and
  preallocated sizes stay excluded (`[x] * n`, `array<T>(n, v)`, `resize`).
  Found with `sieve`: AngelScript's array add-on grows the buffer by exactly one element per
  `insertLast` (a new allocation and a copy of the whole array), so appending is quadratic: 1.5 s
  for 300,001 appends (MSVC), ~22 ms for the whole sieve with `reserve`. The others grow
  geometrically.
- `sieve`: n = 40,000 instead of 300,000 (checksum 4,203). ChaiScript's `push_back` for a Vector is
  a ChaiScript function (clone, then the native `push_back_ref`), ~9 us per call with MSVC, so the
  headline needed ~3.6 s per call with 300,000; with 40,000 ~0.47 s (GCC ~0.3 s).
  Python's `flags.append(True)` stays: a cached bound method made no measurable difference with
  Python 3.14.
- `sort`: n = 1,000 as planned (checksum 116,225,934); the slowest headline, ChaiScript
  `no-threads` with MSVC, needs ~0.41 s per call. TeaScript's `_tuple_append` shares the value
  object of its argument, so the script appends a copy of `x` (`def v := x`); appending `x` itself
  would make every element the same variable. The other forms of a copy have the same speed.
- `byte_buffer`: n = 100,000 instead of 300,000 (checksum 106,756). With 300,000 the slowest
  headline (ChaiScript `no-threads`, MSVC) needed ~1.4 s per call; with 100,000 ~0.47 s (GCC
  ~0.25 s). Lua and ChaiScript reserve the capacity of their arrays (`table.create`, `reserve`).
  The read loop uses index reads in every language; Python's idiomatic `for byte in buf:` would be
  ~9% faster, but measures element iteration instead (documented in the README).
- `maps`: n = 50,000 instead of 10,000 (checksum 1,250,000,000). With 10,000 the slowest headline
  (ChaiScript `no-threads`, MSVC) needed only ~75 ms per call; with 50,000 ~0.39 s (GCC ~0.25 s).
  The checksum limits n to 65,535. The capacity rule also applies to the map: Lua creates it with
  `table.create( 0, n )`; the other maps have no reserve. The keys are read by index in every
  language, like in `byte_buffer` (Python's `for key in keys` would be ~5% faster).
- `strings`: n = 70,000 instead of 100,000 (checksum 863,894). With 100,000 the slowest headline
  (ChaiScript `no-threads`, MSVC) needed ~0.6 s per call; with 70,000 ~0.43 s (GCC ~0.22 s).
  Alternatives measured for the competitors (details in the README): ChaiScript keeps the length
  of `s` in a local for `substr` (~4% faster than a second `s.size()`), and keeps
  `"item" + to_string(i)` (interpolation was ~7% slower). AngelScript keeps `"item" + i`:
  `formatInt` was ~30% faster with MSVC but ~12% slower with GCC. Python's `"item" + str(i)` is as
  fast as the f-string.
- `records`: n = 150,000 instead of 1,000,000 (checksum 787,500). With 1,000,000 the slowest
  headlines (MSVC) needed ~2.9 s (ChaiScript `no-threads`) and ~1.0 s (TeaScript `vm-O1`) per call;
  with 150,000 ~0.43 s and ~0.15 s. Python keeps the plain class: `__slots__` was 0-5% faster, close
  to the noise.
- `binary_trees`: the series n = 4 / 8 / 12 as planned. TeaScript `vm-O1` needs ~0.26 s per call
  (MSVC), the slowest headline apart from ChaiScript, whose cost of a recursion makes it
  ~0.52 / 0.94 / 7.6 s (n = 12 is limited by the time budget, like `fib_recursive` with 25).
  - TeaScript uses `false` for "no child" (the user's choice): every node has `left` and `right`
    like in the other languages, and "no child" needs no object of its own, like `None`, `nil` or
    `null`. An empty Tuple (the idiom of TeaScript's linked list demo) allocates two more Tuples per
    leaf (~13% slower); a leaf as a Tuple without elements (~20% faster) is not a record with `left`
    and `right`.
  - ChaiScript has no null value: a leaf is created with `Node()`, which leaves the attributes
    undefined (`is_var_undef()`).
  - Python keeps the plain class like in `records`: `__slots__` was 0-9% faster (the most for
    n = 12).
- Phase 4b is done.

**2026-10-25**
- TeaScript: the pin moves to `v0.17.0-30-g4ceb756` (six commits after the one used for 4b, with fixes in
  the TeaStackVM and the type registration). All benchmarks are ok with MSVC, GCC 11 and GCC 13.
- `sort` (reviewed): TeaScript keeps the copy `def v := x` inside the loop. Defining `v` once before the
  loop and assigning `v := x` in it gives a wrong result: the assignment writes into the value object
  which the previously appended element shares.
- `records` and `binary_trees` (reviewed): Python keeps plain classes, the natural form for the average
  Python user; `__slots__` is an optimization.
- 4c design round. The catalogue entry `data_exchange` becomes `string_exchange` and `array_exchange`.
  The sizes are starting points, tuned during the implementation as in 4a and 4b. Feature checks with
  small scratch programs (MSVC):
  - LuaBridge3 `3.0-rc12` compiles and works with Lua 5.5.1 (functions, a class with a constructor,
    fields and a method). A read-write field needs `addProperty( "x", &T::x, &T::x )`.
  - Per call of a host function from the script: Lua ~30 ns with LuaBridge3, ~15 ns with the C API;
    Python ~160 ns with pybind11, ~36 ns with the C API (`METH_FASTCALL`); TeaScript ~380 ns with
    `RegisterUserCallback` (a script function: ~510 ns).
  - A bound C++ struct in the loop of `records` (n = 150,000): Lua with LuaBridge3 ~108 ms (a Lua
    table: ~5 ms), Python with pybind11 ~258 ms (a Python class: ~13.5 ms).
  - TeaScript: `_buf_set_u32` on a Buffer created by the host works, and the host reads the result
    from it; the value must be a `u64`. Python: a `memoryview` (format `I`) over host memory works,
    the script writes directly into the C++ buffer.
- Interface: `Language::bind( info )` registers what an interop benchmark needs from the host
  (functions, types, data). The runner calls it after `create_engine()` and before `load()`, and
  measures it as its own phase `bind`. The benchmarks of the other groups register nothing, so their
  startup numbers don't change. `call_bench( n )` stays the measured unit: the script-driven
  benchmarks (`script_to_host`, `cpp_objects`, `buffer`) call `bench( n )` as before, in the
  host-driven ones (`host_to_script`, `string_exchange`, `array_exchange`) it runs the host's C++
  loop, which calls script functions. The host side (the C++ functions, the type, the image buffer,
  the checksum) is shared by the C++ reference and all runners (`benchmarks/<name>/host.hpp`).
  `Language::supports( info )` lets a variant report a benchmark as "n/a".
- Binding layer (the user's choice): the headline of Lua and Python uses their C++ binding libraries,
  LuaBridge3 and pybind11. The average C++ user prefers the higher abstraction to programming against
  a C API. The plain C APIs become variants (n/a outside the interop group), so the cost of the
  abstraction stays visible: it is 2x (Lua) and 4.5x (Python) per host call, see the feature checks.
  TeaScript (`RegisterUserCallback`), ChaiScript (`chaiscript::fun`) and AngelScript (the native
  calling convention, its default on x64) have one API each.
- `script_to_host`: the loop of `calls`, `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )`, but `f0`, `f1`
  and `f3` are C++ functions registered by the host. n = 100,000 (checksum 800,000), like `calls`,
  so script and host functions compare directly. Python imports them in the script's top level
  (`from host import f0, f1, f3`), Lua caches them in locals (like the math functions in
  `math_builtins`); in the other languages they are global functions.
- `host_to_script`: the mirror of `script_to_host`: a C++ loop computes
  `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )` with the script functions of `calls`. n = 100,000
  (checksum 800,000). Each language looks up the function handle once where its API has one.
  TeaScript calls by name with `CallFuncEx` (the user's choice, like `bench` since 2026-10-09; its
  public API has no function handle).
- `string_exchange`: per i, the host passes the C++ string `"item" + i` to the script function
  `f( s )`, which returns `s`; the host converts the result back to a `std::string` and adds its
  length. n = 100,000 (checksum 888,890).
- `array_exchange`: the host converts a `std::vector<int>` of n elements (`i & 7`) into the script's
  natural array (the containers of 4b) and passes it to `f( a )`, which returns a new array with every
  element + 1, built by appending; the host converts it back and sums it. Every call moves the same
  volume: 100,000 / n round trips. Series n = 10 / 1,000 / 100,000 (checksums 390,000 / 450,000 /
  450,000).
- Dropped: script variables set and read by the host (part of the catalogue entry `data_exchange`).
  ChaiScript and AngelScript bind host variables by reference, so the host side is plain memory and
  the benchmark would measure `var_global` again. Also dropped: 4d's `value_conversion`, which would
  measure the conversions of `string_exchange` and `array_exchange` without a call.
- `cpp_objects`: a C++ struct `Vec3i` (int fields x, y, z), bound with a default constructor and
  read-write fields; the script creates one and runs the loop of `records`. n = 150,000 (checksum
  787,500), like `records`, so a script record and a bound C++ object compare directly. Fields only,
  no methods. TeaScript is "n/a", without the emulation with callbacks and passthrough data (changes
  the decision of 2026-10-04): it would measure `script_to_host` again. A real implementation may
  follow when TeaScript can bind C++ types.
- `buffer`: the host owns an image of n x n `uint32` pixels. `bench( n )` writes every pixel with
  `(x & 255) + (y & 255) * 256 + ((x + y) & 255) * 65536` (below 2^24); the host checks a weighted
  sum over the image. n = 256 to start (Full HD is too slow for ChaiScript). Each language writes in
  its natural form for host memory: TeaScript `_buf_set_u32( buf, pos, v )` on a Buffer created by the
  host (a Buffer cannot wrap host memory); ChaiScript `buf[i] = v` on the host's `std::vector` bound by
  reference; AngelScript `image[i] = v` on a registered type with `opIndex`; Lua `buf[i] = v` on a
  userdata with `__newindex`; Python `buf[i] = v` on a `memoryview` over the host memory.
  TeaScript computes `v` as a `u64` with `u64` literals (`x bit_and 255u64`; an operation with a
  `u64` operand gives a `u64`), so no cast per pixel is needed (the user's choice): ~8% faster than
  `as u64`. The loop counters stay `i64`, `_seq` takes only `i64`.
- Order: the interface and `script_to_host` first (with LuaBridge3), then `host_to_script`,
  `cpp_objects`, `buffer`, `string_exchange`, `array_exchange`.
- The interface is implemented as planned. The console shows `bind` as a column ("-" outside the
  interop group), the JSON as `bind_s`. Lua and Python keep the variant without a name as their
  headline and get the variant `c-api` (a free-threaded Python: `free-threaded-c-api`), which reports
  the other benchmarks as "n/a". LuaBridge3 is required for the Lua runner (`CSB_LUABRIDGE3_DIR`), so
  the headline always exists.
- Python: the module `host` is pybind11's embedded module, to which `bind()` adds the functions of the
  benchmark; the `c-api` variant creates a module with `PyMethodDef`s and puts it into `sys.modules`.
  Both are imported in `bind()`, so the import in the script's top level only finds them.
- `script_to_host`: n = 100,000 as planned (checksum 800,000). The slowest headline, ChaiScript
  `no-threads`, needs ~147 ms per call with MSVC (GCC: ~64 ms), within the target. The binding layer
  costs Lua ~2x (MSVC: 9.9 ms with LuaBridge3, 5.1 ms with the C API) and Python ~6x (48.7 ms with
  pybind11, 8.4 ms with the C API); numbers in the README.
- AngelScript keeps the native calling convention. The generic one (`asCALL_GENERIC`) was ~37% faster
  with GCC 11, but ~15% slower with MSVC, so it is not faster with both toolchains (README).
- `host_to_script`: n = 100,000 as planned (checksum 800,000). The host's loop is a template in
  `host.hpp`, so every runner and the C++ reference run the same C++ code; the script defines only
  `f0`, `f1` and `f3`. The slowest headlines need ~215 ms (TeaScript `vm-O1`) and ~180 ms
  (ChaiScript `no-threads`) per call with MSVC (GCC: ~152 and ~72 ms), within the target.
  - Lua: the `c-api` variant calls through registry references (`luaL_ref`), the C API's function
    handle, following the rule of the design round; `lua_getglobal` on every call was ~17% slower.
  - Python imports the module `host` only for the benchmarks which bind something (before: for the
    whole interop group, which showed up as `bind` time of `host_to_script`).
- `cpp_objects`: n = 150,000 as planned (checksum 787,500), like `records`. TeaScript has no script
  ("n/a"). The slowest headline, ChaiScript `no-threads`, needs ~460 ms per call with MSVC (GCC:
  ~198 ms), within the target. AngelScript binds `Vec3i` as a value type, its form for a small struct;
  its fields are properties at their offsets, as fast as the script class of `records`. The binding
  libraries cost the most here: Lua ~106 ms with LuaBridge3 and ~60 ms with the C API (a Lua table:
  ~5 ms), Python ~257 ms with pybind11 and ~31 ms with the C API's member descriptors (a Python class:
  ~13.5 ms); numbers in the README.
- One run of Python's `cpp_objects` (pybind11, MSVC) needed 1.27 s instead of ~257 ms, in all of its
  samples, while the other benchmarks in the same process were normal; it was not reproducible.
  Single engine instances which are slow for their whole run were seen with MSVC in two languages
  (see the open question on repeated runs).
- `buffer`: n = 400 instead of 256 (160,000 pixels, checksum 6,097,724,102,400, computed by the host
  in 64 bit). With 256 the slowest headline needed ~120 ms with MSVC, but only ~47 ms with GCC; with
  400 ChaiScript `no-threads` needs ~295 ms with MSVC and ~119 ms with GCC, TeaScript `vm-O1` ~145 and
  ~128 ms.
  - `bind()` runs before n is known, so the host binds an image object and resizes and clears its
    memory in `call_bench( n )` before every call; the checksum over it is computed in the same call.
    `bench( n )` returns the number of pixels, which the host checks. Python takes the memoryview in
    `bench`, since the memory moves when the host resizes the image.
  - Changed from the design round: ChaiScript and Lua write with a method `set( index, value )`
    instead of the index operator. It was faster with both toolchains (ChaiScript ~13% / ~25%, Lua
    with LuaBridge3 ~3.2x; the C API variant is equal). AngelScript keeps `opIndex`, which was faster
    than a method. Numbers in the README.
  - LuaBridge3's `__newindex` converts an integer key with `lua_tostring()` before it calls the
    fallback of the class, so the fallback gets a numeric string.
- `string_exchange`: n = 250,000 instead of 100,000. With 100,000 the slowest headline, TeaScript
  `vm-O1`, needed only ~62 ms with MSVC and ~40 ms with GCC; with 250,000 ~155 and ~103 ms. The
  checksum adds the last digit of every result to its length, so a result with the same length but
  another content is detected (checksum 3,513,890). Each language passes the string in its normal way
  (AngelScript: `const string &in`, the result from `GetReturnObject()`); numbers in the README.
- `array_exchange`: every call moves 40,000 elements instead of 100,000 in each direction, series
  n = 10 / 1,000 / 40,000 instead of 10 / 1,000 / 100,000 (4,000 / 40 / 1 round trips, checksums
  156,000 / 180,000 / 180,000). With 100,000 the slowest headline, ChaiScript `no-threads`, needed
  ~0.9-1.0 s per call with MSVC (its `push_back` is a ChaiScript function, see `sieve`); with 40,000
  ~0.35-0.4 s (GCC ~0.2 s). TeaScript `vm-O1` needs ~25-37 ms (GCC ~16-23 ms). The host converts with
  each language's normal API; AngelScript's `array<int>` stores its elements contiguously, so the host
  copies them as a block (`GetBuffer()`). Lua and Python use the converters of their binding
  libraries (LuaBridge3 `Vector.h`, pybind11 `stl.h`) for the headline.
- Phase 4c is done.

**2026-10-28**
- TeaScript: the pin moves to `v0.17.0-32-gbdd696c`. Until 0.18.0 is released, TeaScript is pinned to
  its HEAD by default (the user's rule); every move is verified with all toolchains. All benchmarks
  are ok with MSVC, GCC 11 and GCC 13.
- 4d design round. The catalogue entry `registration` becomes `register_functions` and
  `register_variables`, `function_lookup` becomes `call_by_name` and `call_by_handle`, `engine_minimal`
  is new, and `memory` moves to Phase 5. One combined number would hide which part is slow, as in 4b.
  The sizes are starting points, tuned during the implementation as before.
- Measurement model: the host group gets a new adapter method `run_host( info, n )`. It prepares a
  sample without measuring it (e.g. a fresh engine), measures only the operation itself, checks the
  result afterwards and returns the duration, the sub-phases and a checksum, like `load()`, which
  already measures its own sub-phases. The runner keeps everything else: the first call, the warm-up
  calls, the samples, the time budget, the statistics and the JSON; the sub-phases get statistics of
  their own. Startup, bind and load of the other groups don't apply ("-").
- The C++ runner reports the host group as "n/a", it embeds nothing. The `c-api` variants of Lua and
  Python run the host benchmarks which use the binding layer (`register_functions`,
  `register_variables`, `call_by_name`, `call_by_handle`); the others are "n/a" there (the same code as
  the variant without a name).
- `engine_lifecycle`: per sample, n cycles of creating an engine with the standard library (as
  `create_engine()` does) and destroying it; sub-phases `create` and `destroy`. The startup and
  teardown of the other benchmarks are single measurements per run; this gives them samples. n = 10 to
  start, so Lua's ~45 us per engine are well above the resolution of the clock. The checksum is the
  number of engines; every API reports a failure.
- `engine_minimal`: the same with the smallest engine each API allows: Lua `luaL_newstate()` without
  libraries, AngelScript the engine without add-ons, ChaiScript `ChaiScript_Basic` without `Std_Lib`,
  TeaScript `config::minimal()`. Python is "n/a": the interpreter always loads its core modules, and
  the isolated configuration is already its minimum.
- `parse_compile`: one template per language (`parse_compile.<ext>`: a header, one function, a footer
  with `bench( n )`); the harness repeats the function N times and numbers it before anything is
  measured. A function has about 10 lines: a parameter, locals, a loop, an if/else, arithmetic and a
  call of a helper of the header. Every sample loads into a fresh engine (TeaScript and ChaiScript
  reject a redefinition). Series N = 100 / 1,000 / 10,000. The headline is "source -> runnable", with
  the sub-phases of `load()`. An unmeasured call of `bench( n )` gives the checksum.
  - Python: one step, `Py_CompileString()` (`parse_compile`), then `top_level`, not `ast.parse` +
    `compile()` as planned: `ast.parse` builds Python objects for the AST, which the normal path never
    does.
  - Lua: the generated functions are globals. `local function` would exceed Lua's limit of 200 local
    variables per function.
- `register_functions`: register N C++ functions in a fresh engine with each language's normal API:
  `f1` of `script_to_host` under the names `f0` .. `f{N-1}`, generated before the measurement. An
  unmeasured script calls them for the checksum. `register_variables`: the same with N host `int`
  variables, in each API's normal form of a global variable (the user's choice; in TeaScript,
  functions are variables as well: `AddVar`). Series N = 10 / 100 / 1,000.
- `call_by_name` and `call_by_handle`: a generated script defines M tiny functions `f_k( a )`; the host
  makes 100,000 calls and cycles through them. `call_by_name` looks the function up by name at every
  call (Lua `lua_getglobal` or LuaBridge3's `getGlobal`, Python the namespace dict, AngelScript
  `GetFunctionByName()`, ChaiScript `eval<std::function<...>>( name )`, TeaScript `CallFuncEx`).
  `call_by_handle` looks every function up once and calls its handle, as in `host_to_script`
  (ChaiScript's `std::function`, Lua's `LuaRef` or registry reference, Python's function object,
  AngelScript's `asIScriptFunction *`). Series M = 10 / 1,000 / 10,000: shows whether the cost of a
  lookup grows with the number of functions.
- TeaScript `call_by_handle` is "n/a" (the user's choice). Its counterpart of a handle is the
  `FunctionPtr` from `GetVar()` + `GetValueCopy<FunctionPtr>()`, but calling it needs the `Context`,
  which only a class derived from `Engine` reaches. That is the low-level API, which users can use, but
  not part of the default public API. A public call of a `FunctionPtr` may be added to TeaScript later;
  then it is used here. `host_to_script` keeps `CallFuncEx` (2026-10-09).
- `engine_reset`: reset an engine and load a small script again (sub-phases `reset` and `load`), to
  compare with a fresh engine. TeaScript `ResetState()`, ChaiScript `set_state()` with the state saved
  after the creation, AngelScript `Discard()` of the module and a new module. Lua and Python have no
  reset; their form of reuse counts (the user's choice): a fresh `_ENV` table, a fresh globals dict. It
  does not undo what a script changed in libraries or modules (documented).
- `memory` moves to Phase 5. The peak working set is a value per process (on Windows it can't be
  reset), so it needs one process per run, which is the job of the orchestration. Allocation counts
  need an allocator hook per library (`lua_Alloc`, `asSetGlobalMemoryFunctions()`,
  `PyMem_SetAllocator()`, `operator new`), and these don't count the same things.
- Order: the interface with `engine_lifecycle` and `engine_minimal` first, then `parse_compile`,
  `register_functions` and `register_variables`, `call_by_name` and `call_by_handle`, `engine_reset`.
- The interface is implemented as planned. `BenchmarkInfo::has_script` marks the benchmarks without a
  script. The console shows "-" for startup, bind and load of the host group and the medians of the
  sub-phases after the status; the JSON has no `startup_s`, `bind_s`, `load_s` and `teardown_s` there,
  but `phases_s` with the statistics and samples of every sub-phase. The warm-up engine of a variant
  is one call of `run_host()` when the first selected benchmark is a host benchmark.
- `engine_lifecycle`: n = 10 as planned; each cycle is the runner's `create_engine()` and
  `destroy_engine()`. Per engine (headline variants, MSVC): Lua ~30 us, AngelScript ~0.48 ms,
  TeaScript ~1.05 ms, ChaiScript ~4.1 ms, Python ~6.5 ms; numbers in the README.
- `engine_minimal`: n = 10 like `engine_lifecycle`. 100 engines per sample did not make the spread
  smaller (2-6% with both). ChaiScript uses `Library_Options::No_Stdlib`, its own option for an engine
  without `Std_Lib` (an empty module instead), not a `ChaiScript_Basic` constructed by hand.
  TeaScript uses `config::minimal()`, not the protected constructor of `Engine` without the Core Library
  (the low-level API, see `call_by_handle`).
- `parse_compile`: the series 100 / 1,000 / 10,000 as planned. `BenchmarkInfo::script` (`ScriptKind`)
  replaces `has_script`: `file`, `none`, or `generated` for a template, which the runner expands for
  each n before anything is measured (the lines between `csb:repeat` and `csb:end` n times, with
  `__I__` replaced by the index; `__LAST__` by n-1). The function is about 10 lines as planned
  (README); `bench( n )` calls the first and the last one. The generated functions are named `fn<k>`
  instead of `f<k>`: `f64` is a type name in TeaScript. A sample is the sum of the sub-phases of
  `load()`, the lookup of `bench` afterwards is not part of it. The slowest headline, ChaiScript
  `no-threads`, needs ~1.36 s with MSVC for 10,000 functions (GCC ~0.88 s): its `top_level` grows
  quadratically, because ChaiScript's function table (`QuickFlatMap`) is searched linearly at every
  `def`. Numbers in the README.
- `register_functions` and `register_variables`: the series 10 / 100 / 1,000 as planned. The
  registrations go where `script_to_host` puts its functions: global in TeaScript, ChaiScript and
  AngelScript, the table `host` in Lua, the module `host` in Python. Creating the table is measured in
  Lua (both variants do it in the same step as the registrations); importing or creating the module in
  Python is not. The names (`fn<k>`, `v<k>`), AngelScript's declarations and the C API lists
  (`luaL_Reg`, `PyMethodDef`) are created before the measurement. A generated check script calls the
  first and the last registration afterwards (a template without a repeated block, only `__LAST__`).
  `uses_binding()` (`benchmarks/registry.hpp`) names the benchmarks the `c-api` variants run.
  - Variables: bound by reference where the API binds a C++ variable (ChaiScript
    `add_global( var( std::ref( x ) ) )`, AngelScript `RegisterGlobalProperty`, LuaBridge3 read-write
    properties), a copy where it only copies (TeaScript `AddVar`, Lua's C API, Python `m.attr` and
    `PyModule_AddIntConstant`).
  - The samples of n = 10 take only a few microseconds, so their spread is larger (up to ~15%). The
    slowest headline is ChaiScript's `register_functions`, ~12.7 ms for 1,000 functions with MSVC; it
    grows quadratically like its `top_level` in `parse_compile`. Numbers in the READMEs.
- `call_by_name` and `call_by_handle`: the series M = 10 / 1,000 / 10,000 as planned, 100,000 calls of
  `fn<k>( a ) = a + k` per call of `call_bench`, which cycle through the functions (checksums 800,000 /
  50,300,000 / 500,300,000).
  - They use the normal phases like `host_to_script` (changes the measurement model of the design round
    for them): one engine per run, the generated script is loaded once, the measured `call_bench( n )`
    is the host's loop. With `run_host()` every sample would need a fresh engine and the load of up to
    10,000 functions, unmeasured but within the time budget (ChaiScript: ~1.3 s per sample).
    `BenchmarkInfo::sampling` (`Sampling::run_host` or `Sampling::call_bench`) replaces the check of the
    group in the runner.
  - `load()` gets the size n of the run, so the adapter creates the names and looks up the handles
    there, like `host_to_script` (n was only known to `call_bench`).
  - Lookups by name: TeaScript `CallFuncEx`, ChaiScript `eval<std::function<int( int )>>( name )` (its
    documented way), AngelScript `GetFunctionByName()`, Lua `lua_getglobal()` (C API) or LuaBridge3's
    `getGlobal()`, Python the item of the namespace dict (`PyDict_GetItemString()` with the C API).
    TeaScript has no script for `call_by_handle` ("n/a", like `cpp_objects`).
  - The slowest headline is ChaiScript's `call_by_name` with 10,000 functions, ~1.47 s with MSVC (GCC
    ~0.85 s), above the usual 0.1-0.5 s: its `eval()` parses the name and searches the function table
    linearly (~14.7 us per call, ~2.8 us with 10 functions). The 100,000 calls stay, so the numbers
    compare with `call_by_handle` and `host_to_script` (like the series of ChaiScript's recursion).
    Through handles, the time per call grows at most ~30% from 10 to 10,000 functions in every
    language. Numbers in the READMEs.
- `engine_reset`: a small script (a top-level variable, a helper, `bench`), n = 100 only for the check
  (checksum 345). One sample: a fresh engine which has loaded and called the script (not measured), then
  the reset and the load again (measured, sub-phases `reset` and `load`), then `bench( n )` as the check.
  - The resets as planned. AngelScript also calls `Unprepare()` of the context, which holds the function
    it executed last; `DiscardModule()` removes the module. Lua: every load of this benchmark runs in its
    own environment (the `_ENV` upvalue of the chunk, a table with `__index = _G`), which the reset
    replaces; `bench` is looked up there. Python: the reset releases the old namespace, the load creates
    a fresh one as always.
  - Lua's, Python's and AngelScript's old definitions are freed later by their garbage collection (in
    Python a cycle: the functions refer to the namespace), which is not part of the reset (README).
  - TeaScript's `ResetState()` bootstraps the Core Library again, so it costs about as much as a fresh
    engine (MSVC: ~0.92 ms reset, ~1.0 ms creation). For the others, reset and load cost a fraction of
    a fresh engine. Numbers in the README.
- Phase 4d is done, and with it Phase 4.
- TeaScript's `ResetState()` will get a parameter before the 0.18.0 release, which controls the parts
  that are reset, so the Core Library can stay without a new bootstrap (the user). `engine_reset` is
  adapted once it exists.

**2026-10-29**
- Phase 5 design round, part 1: where and how the results are measured and published (the user's
  decisions; the measurement model was proposed in the same round).
- The results are part of the repo and readable on GitHub in the browser. One directory per
  configuration (machine, OS, toolchain), `results/<set>/`, with a generated `README.md` (setup, tables,
  charts), the charts as SVG files and the merged raw data of all processes (`results.json`).
  `results/README.md` lists the sets with their setup, date and versions. A Python script with the
  standard library only generates the charts: nothing to install, and the same input gives the same
  files, so the diffs in git stay small. GitHub's Mermaid charts have no logarithmic axis, which the
  range from C++ to ChaiScript needs (several orders of magnitude within one benchmark). A web page
  may follow later from the same data.
- Tables: one row per benchmark and n (answers the open question on series), the headline variants as
  columns, each cell the median and the factor vs. C++. Series additionally as a chart over n. The
  other variants in tables of their own. No overall score: one number would weight the disciplines
  arbitrarily.
- Every set states its setup: CPU (model, cores and threads), RAM, OS and version, virtualization
  (e.g. a Hyper-V root partition, WSL2, KVM with the number of vCPUs), the compiler with version and
  flags, the version or commit of every library and of cpp-script-bench, the date, and the settings
  (warm-up, runs, budget, processes per run, priority and pinning, the load before and after). Every
  chart has a short label of its set in its footer (CPU, OS, compiler, commit, date), so a chart shown
  elsewhere still says where it comes from. Host names, addresses, paths and user names are never
  recorded. The runners' JSON gets what is missing for this: cores and threads, RAM, the OS
  distribution and its version, whether a hypervisor is present, and the compiler flags (the library
  versions are already part of each runner's version string).
- A set is built only from runs of one clean commit of cpp-script-bench (no `-dirty`) and one version
  per library; mixed inputs are rejected. Any change of a measured component (a library, a script, the
  harness, the compiler) means a full rerun of the set with all languages. No partial updates, e.g.
  only TeaScript measured again on another day. Earlier sets stay in the git history.
- Until TeaScript 0.18.0, the sets are marked "preview" and rerun on request, not at every move of the
  TeaScript pin. The regular sets follow with 0.18.0 (Phase 6).
- Machines: the MSVC set on Windows, the main GCC set (GCC 11) with WSL2 on the same machine, so the
  two sets differ in OS and toolchain only. With WSL2 installed, Windows itself runs on the hypervisor
  (as its root partition), so neither set is bare metal; CPU-bound code runs at nearly native speed in
  both. A second GCC set (GCC 13, AMD Zen 4) on a cloud VM with 2 vCPUs only if the noise check shows
  a spread close to that of WSL2: on such a VM, the load of the other vCPU and of other guests of the
  host (steal time) can't be controlled, only recorded.
- Measurement model: the orchestrator (`runner/`, Python, standard library only) starts one process
  per run (variant, benchmark, n). That gives the peak memory of each run (`memory`) and repeats every
  run in several processes (answers the open question on repeated runs). The repeats are interleaved
  across the languages (A B C ... A B C ...), so slow changes of the machine during the hours of a run
  affect all languages alike. Reported per run: the median of the process medians, and the spread
  between the processes. Priority and pinning (answers the open question): Windows the high priority
  class and one fixed core, Linux `taskset` with the normal priority (a higher one needs root).
- Noise check before the first sets: the same subset of benchmarks in 5 processes each, on Windows and
  WSL2, with an idle machine and with light interactive use (an editor, a browser). It decides the
  number of processes per run, the core for the pinning, and how quiet the machine must be during a
  run.
- Implemented: the system info and the orchestrator.
  - The runners record the physical cores, the logical processors, the physical memory, the OS product
    or distribution (Windows: ProductName and DisplayVersion; Linux: `PRETTY_NAME` of os-release), the
    hypervisor (the CPUID signature; Hyper-V's root partition is marked) and the C++ flags of the
    build. In a VM, these are the values of the VM (WSL2 has half the memory of the host by default).
  - `runner/run.py`, Python 3.10 or newer (the oldest Python on the test machines). Each process is a
    runner with `--bench b --n n --variant v --json f`; the warm-up engine of the runner stays in every
    process. Measured from the outside for each process: the wall and the CPU time, the peak memory
    (Windows: the peak working set and the peak private memory of the exited process; Linux: the peak
    resident set from `wait4()`), and the load of the machine during the process: the busy logical
    CPUs minus the process itself (`GetSystemTimes()`, `/proc/stat`), on Linux also the steal time.
    The load of a process shorter than about 1 s says little: Windows counts CPU times in ticks of
    15.6 ms.
  - The orchestrator pins itself, its processes inherit it. Default: the last logical CPU, until the
    noise check decides. The set records the power plan (Windows) or the frequency governor (Linux).
  - Runs which are "n/a" are not repeated. A set is refused if its runners were built from different
    commits, from a modified checkout (`-dirty`, also of a library; `--allow-dirty` marks a test set),
    or not as a Release build.
  - The processes and their console output go into a work directory in the build directory
    (`<build>/results/<UTC time>/`, not in git); `results.json` merges them, one entry per run with
    all its processes. An interrupted set is continued with `--resume` (crashed processes are run
    again); the set records the number of sessions.
  - With the defaults, a set has 50 rows (benchmark and n), 15 variants and 3 processes per run:
    2,250 processes (900 with the headline variants only).

## 10. Open questions

- Script alternatives: a second script per benchmark and language (e.g. TeaScript `_strtonum` or
  `_strtonumex` instead of `as i64`, Python `"item" + str(i)` instead of an f-string), selectable
  on the command line and reported outside the headline? Until then, alternatives are compared
  with a scratch copy of the script and `--scripts`. Phase 5.
