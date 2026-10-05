# CLAUDE.md

Guidance for Claude Code in this repository.

**cpp-script-bench**: fair benchmarks of script languages embedded in C++ (TeaScript, ChaiScript,
AngelScript, Lua, Python; native C++ as the reference).

## Start here

- **Read [PLAN.md](PLAN.md) first.** It holds the fairness rules, the measurement model, the
  discipline catalogue, the roadmap (what's next), the decision log and the open questions.
- Keep PLAN.md current: record every decision in the decision log (with date), tick roadmap
  items, and move answered open questions into the decision log.
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

# Linux / WSL, GCC (from Git Bash: prefix the wsl call with MSYS_NO_PATHCONV=1)
wsl -e bash -lc "cd <repo path in WSL> && cmake --preset gcc-local && cmake --build --preset gcc-local-release"
```

Runner options: `--list`, `--bench a,b`, `--variant v|all`, `--n N[,N...]`, `--warmup W`, `--runs R`,
`--budget S`, `--scripts dir`, `--json file`.

- Only Release builds give meaningful numbers.
- Never take timings while something else compiles or runs. WSL and Windows share the CPU.
- ChaiScript's `fib_recursive` with n = 25 takes ~18 s per call, ~72 s per run even with the time
  budget. Use `--n 20` (and `--bench`, `--runs`, `--warmup`) when you only verify results.
- Verify every change with both toolchains (MSVC and GCC). Every result must be "ok", i.e. match
  the C++ reference checksum.

## Layout

- `harness/`: `csb::Language` interface (`include/csb/language.hpp`), `csb::run_main()`, statistics,
  JSON report, system info.
- `benchmarks/<name>/`: `README.md` (spec), `reference.cpp`, one script per language
  (`<name>.tea`, `.chai`, ...). Registry: `benchmarks/registry.cpp`.
- `adapters/<lang>/`: one runner executable per language.
- `cmake/`: `csb_apply_compiler_options()` (same flags for all code built from source),
  `csb_git_describe()`.

## Adding a benchmark

1. `benchmarks/<name>/README.md`: task, pseudo code, n (or a series of n), checksum, rules, notes per language.
2. `benchmarks/<name>/reference.cpp`: `std::int64_t csb::reference::<name>( std::int64_t n )`.
3. One script per language, defining `bench( n )` and returning the integer checksum.
4. Register it with its n (or series) in `benchmarks/registry.cpp` and add `reference.cpp` to
   `benchmarks/CMakeLists.txt`.
5. Update the discipline table in PLAN.md.

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

## Code style

- Match the existing code: 4 spaces indent, spaces inside parentheses (`f( x )`, `if( x )`),
  opening brace on the same line for control statements and on a new line for functions.
- Every file starts with the SPDX header:
  `SPDX-FileCopyrightText: Copyright (C) 2026 Florian Thake, <contact |at| tea-age.solutions>.` and
  `SPDX-License-Identifier: MIT`.
- C++20. The harness has no third-party dependencies.
- Commit messages: a summary line, a blank line, then bullet points.
