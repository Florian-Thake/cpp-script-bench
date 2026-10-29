# CLAUDE.md

Guidance for Claude Code in this repository.

**cpp-script-bench**: fair benchmarks of script languages embedded in C++ (TeaScript, ChaiScript,
AngelScript, Lua, Python; native C++ as the reference).

## Start here

- **Read [PLAN.md](PLAN.md) first.** It holds the fairness rules, the measurement model, the
  discipline catalogue, the roadmap (what's next), the decision log and the open questions.
- Keep PLAN.md current: record every decision in the decision log (with date), tick roadmap
  items, and move answered open questions into the decision log.
- If `LOCAL.md` exists, read it too: the setup of the local machines (paths, hosts, access). It is
  git-ignored. Such details go there, never into committed files.
- Fairness is the core value of this project. The maintainer is the TeaScript author, so never
  favour TeaScript. When a script choice affects a competitor's speed, give that language its best
  idiomatic form and document the reason in the benchmark's README.

## Build and run

Local library paths are in `CMakeUserPresets.json`, which is per machine and git-ignored. Its
presets `msvc-local` and `gcc-local` inherit from `msvc` and `gcc` in `CMakePresets.json`.

```
# Windows, MSVC
cmake --preset msvc-local
cmake --build --preset msvc-local-release
build/msvc-local/adapters/<lang>/Release/csb_<lang>.exe --list

# Linux, GCC
cmake --preset gcc-local
cmake --build --preset gcc-local-release
build/gcc-local/adapters/<lang>/csb_<lang> --list
```

Runner options: `--list`, `--bench a,b`, `--variant v|all`, `--n N[,N...]`, `--warmup W`, `--runs R`,
`--budget S`, `--scripts dir`, `--json file`.

Result sets: `python runner/run.py <build dir>/adapters` (Linux: `python3`), one process per run; see
README.md and PLAN.md (decision log 2026-10-29). A full set takes hours: only on a quiet machine. On
Windows, the user starts `runner/sets.py` (MSVC, then WSL2, then publish) in a terminal of their own,
outside any Claude session; a later session picks up the results (`build/sets/<time>/status.json`).

- Only Release builds give meaningful numbers.
- Never take timings while something else compiles or runs on the same machine. A VM or WSL shares
  the CPU with its host.
- ChaiScript's `fib_recursive` with n = 25 takes ~18 s per call, ~72 s per run even with the time
  budget. Use `--n 20` (and `--bench`, `--runs`, `--warmup`) when you only verify results.
  Its `tak` with n = 6 takes ~3.6 s per call, its `binary_trees` with n = 12 ~7.6 s.
- Verify every change with both toolchains (MSVC and GCC). Every result must be "ok", i.e. match
  the C++ reference checksum.

## Layout

- `harness/`: `csb::Language` interface (`include/csb/language.hpp`), `csb::run_main()`, statistics,
  JSON report, system info.
- `benchmarks/<name>/`: `README.md` (spec), `reference.cpp`, one script per language
  (`<name>.tea`, `.chai`, ...), for interop and host API benchmarks `host.hpp` (the host side). Registry:
  `benchmarks/registry.cpp`.
- `adapters/<lang>/`: one runner executable per language.
- `cmake/`: `csb_apply_compiler_options()` (same flags for all code built from source),
  `csb_git_describe()`.
- `runner/`: the orchestrator `run.py` (Python standard library only, 3.10+, so it runs on every machine).

## Adding a benchmark

1. `benchmarks/<name>/README.md`: task, pseudo code, n (or a series of n), checksum, rules, notes per language.
2. `benchmarks/<name>/reference.cpp`: `std::int64_t csb::reference::<name>( std::int64_t n )`.
3. One script per language, defining `bench( n )` and returning the integer checksum.
4. Register it with its n (or series) in `benchmarks/registry.cpp` and add `reference.cpp` to
   `benchmarks/CMakeLists.txt`.
5. Interop benchmarks: the C++ functions, types and data in `benchmarks/<name>/host.hpp`, registered in
   the `bind()` of every adapter. Lua and Python bind twice: with LuaBridge3 / pybind11 (headline) and
   with the plain C API (variant `c-api`).
6. Host API benchmarks (group `host`): `Sampling::run_host` in the registry and `run_host()` in every
   adapter, which prepares and checks a sample without measuring it and measures only the C++
   operation (PLAN.md, section 3). A host loop of calls (`call_by_name`) uses the normal phases
   instead, like `host_to_script`. A shared measuring loop goes into `benchmarks/<name>/host.hpp`. In
   the registry: `ScriptKind::none` if the benchmark has no script, `ScriptKind::generated` for a
   template, which the runner expands per n (`load()` gets n). C++ is "n/a"; `uses_binding()` names
   the benchmarks of the `c-api` variants.
7. Update the discipline table in PLAN.md.

## Adding a language

1. `adapters/<lang>/CMakeLists.txt` and `main.cpp`: implement `csb::Language`, and provide a
   `csb::LanguageEntry` with its variants and its headline variant.
2. Add `CSB_WITH_<LANG>` and `CSB_<LANG>_DIR` to the top-level `CMakeLists.txt`, and the
   subdirectory to `adapters/CMakeLists.txt`.
3. Libraries built from source (Lua, AngelScript) must use `csb_apply_compiler_options()` too.
4. No disk IO inside measured phases. Disable module or file loading features of the engine.
5. Add one script per existing benchmark. Update the language tables in README.md and PLAN.md.

## Script rules (details in PLAN.md, section 2)

- Same algorithm as the spec, the language's natural constructs, hot code in functions with locals.
- Plain assignments (`x = x + y`), no compound assignment. Values stay within 32-bit signed ints.
- ChaiScript: no `return`. It throws a C++ exception unless it is the last statement; use the
  value of the last expression.
- TeaScript: there is no `while`. Use `repeat` with `if( <negated condition> ) { stop }`.
- Lua: helper functions are `local function`, `bench` is global. Constants are `local x <const>`
  where TeaScript uses `const`.
- AngelScript: statically typed; the entry point is exactly `int bench( int n )`.
- Python: module-level functions, hot variables are locals; don't shadow built-ins (e.g. `total`
  instead of `sum`).
- Containers (PLAN.md, decision log 2026-10-14 and 2026-10-18): the natural container per language;
  grow by appending in a loop, with a capacity reserved where the language can (AngelScript and
  ChaiScript `reserve`, Lua `table.create`); no native fill. Lua indexes from 1: shift the loop
  ranges, not every access.
- TeaScript: `_tuple_append` shares the value object it gets; append a copy of a variable
  (`def v := x`), not the variable itself.
- Measure plausible alternatives of a competitor's script; use the faster form if it is idiomatic
  and faster with both toolchains, and document the numbers in the benchmark's README.

## Code style

- Match the existing code: 4 spaces indent, spaces inside parentheses (`f( x )`, `if( x )`),
  opening brace on the same line for control statements and on a new line for functions.
- Every file starts with the SPDX header:
  `SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.` and
  `SPDX-License-Identifier: MIT`.
- C++20. The harness has no third-party dependencies.
- Commit messages: a summary line, a blank line, then bullet points.
