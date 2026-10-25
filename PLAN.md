# cpp-script-bench: Plan

A living document: goals, rules, architecture and roadmap. Last updated: 2026-10-25.

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
| ChaiScript  | `develop` HEAD, pinned by commit hash (now `f635b51e`, 2026-05-02; reports itself as 7.0.0) | native API; ChaiScript_Extras `math.hpp` (`master` at `dd116d7`, 2026-04-17) | the last release (6.1.0) is from 2018; headline build: `no-threads` |
| AngelScript | 2.38.0 (Aug 2026)                         | native API + string, array, dictionary, math add-ons (math with `AS_USE_FLOAT=0`: double) | compiled library; headline: `AS_NO_THREADS`, without line cues |
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
| script_to_host | the loop of `calls`, but the functions are registered C++ functions | ⬜ |
| host_to_script | the mirror: a C++ loop calls the script functions of `calls` | ⬜ |
| cpp_objects    | the loop of `records` on a bound C++ type (`Vec3i`). TeaScript has no class binding: "n/a" | ⬜ |
| buffer         | fill an RGBA image buffer owned by the host (from the old BufferOverhead test) | ⬜ |
| string_exchange | strings from the host to a script function and back | ⬜ |
| array_exchange | arrays from the host to a script function and back (converted both ways); series over the size | ⬜ |

The catalogue entry `data_exchange` was split into `string_exchange` and `array_exchange` (decision
log 2026-10-25).

### Host API (C++ level)

Costs the host application pays for embedding, measured by the harness:

| Discipline      | Workload                                        | Status |
|-----------------|-------------------------------------------------|--------|
| engine_lifecycle | create and destroy an engine, empty and with the standard library | ⬜ |
| parse_compile   | generated sources of 100 / 1,000 / 10,000 functions. Headline is the total "source -> runnable"; sub-phases where available (TeaScript: parse + compile; ChaiScript: parse only; AngelScript and Lua: one combined step; Python: `ast.parse` + `compile()`) | ⬜ |
| registration    | register N C++ functions and variables          | ⬜ |
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
- [ ] **Phase 4:** the full discipline catalogue, in four subphases. Each discipline is implemented for
      all languages at once (one commit each).
  - [x] **4a Core:** `int_arith`, `calls`, `tak`, `branching`, `math_builtins` (design round
        2026-10-09, implemented 2026-10-14).
  - [x] **4b Data:** `var_global`, `var_nested`, `var_lifecycle`, `sieve`, `sort`, `byte_buffer`,
        `maps`, `strings`, `records`, `binary_trees` (design round 2026-10-14, implemented
        2026-10-18).
  - [ ] **4c Interop (next):** `script_to_host`, `host_to_script`, `cpp_objects`, `buffer`,
        `string_exchange`, `array_exchange` (design round 2026-10-25). Extends `csb::Language`; brings
        in LuaBridge3.
  - [ ] **4d Host API:** `engine_lifecycle`, `parse_compile`, `registration`, `function_lookup`,
        `engine_reset`, `memory`. A new kind of benchmark: the runner times C++ operations. `memory` may
        move to Phase 5.
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
  - TeaScript: a call of a script function from C++ takes ~636 ns with `CallFuncEx`, ~570 ns through a
    cached `FunctionPtr`. `_buf_set_u32` on a Buffer created by the host works, and the host reads the
    result from it; the value must be a `u64`. Python: a `memoryview` (format `I`) over host memory
    works, the script writes directly into the C++ buffer.
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
  (`from csb_host import f0, f1, f3`), Lua caches them in locals (like the math functions in
  `math_builtins`); in the other languages they are global functions.
- `host_to_script`: the mirror of `script_to_host`: a C++ loop computes
  `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )` with the script functions of `calls`. n = 100,000
  (checksum 800,000). Each language looks up the function handle once where its API has one.
  TeaScript calls by name with `CallFuncEx` (the user's choice, like `bench` since 2026-10-09; a
  cached `FunctionPtr` is ~10% faster, but needs the Context, which the public API doesn't expose).
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

## 10. Open questions

- Series: how to show them in the report (e.g. one row per n, or a chart over n)? Phase 5; the JSON
  already has one entry per n.
- Process priority and CPU pinning for less noise? Phase 5, before the published runs.
- Format of the published results: Markdown tables, charts, a web page? Phase 5.
- Script alternatives: a second script per benchmark and language (e.g. TeaScript `_strtonum` or
  `_strtonumex` instead of `as i64`, Python `"item" + str(i)` instead of an f-string), selectable
  on the command line and reported outside the headline? Until then, alternatives are compared
  with a scratch copy of the script and `--scripts`. Phase 5.
