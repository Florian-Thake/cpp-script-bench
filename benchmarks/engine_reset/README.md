# engine_reset

**Group:** host. **Measures:** what an application pays to reuse an engine for a new start of its
script: reset the engine, then load the script again. The alternative is a fresh engine
(`engine_lifecycle`) and the same load.

## Task

```
create a fresh engine, load the script, call bench( n )     (not measured)
reset the engine                                             (measured: "reset")
load the script again                                        (measured: "load")
check: bench( n )                                            (not measured)
```

- The script is small: a top-level variable `offset = 3`, a helper `f1( a ) = a & 7` and
  `bench( n ) = offset + the sum of f1( i ) for i = 0 .. n-1`.
- n = 100, which only matters for the check. Checksum: 345.
- One sample is one call of `run_host()` (`host.hpp`): a fresh engine for every sample, which has run
  the script once before it is reset.
- C++ is "n/a". The `c-api` variants of Lua and Python are "n/a": their engine is the same as the
  headline's.

## Rules

- Each language resets with its own API for it. Lua and Python have none; their form of reuse counts
  (PLAN.md, decision log 2026-10-28): a fresh environment for the script.
- TeaScript and ChaiScript reject a redefinition, so a reset which kept the old definitions would make
  the second load fail.

## The reset per language

| Language    | Reset                                      |
|-------------|--------------------------------------------|
| TeaScript   | `ResetStateFor( Reset_ScriptState )`: removes the local scopes, in which the script runs, and the script arguments; the global scope with the Core Library and what the host registered stays |
| ChaiScript  | `set_state()` with the state from `get_state()` right after the creation of the engine |
| AngelScript | `Unprepare()` of the context (it holds the function it executed last), `DiscardModule()` of the module of the script; the load builds a new module |
| Lua         | a fresh environment table for the script (`_ENV`, the upvalue of the chunk), whose metatable reads the globals of the state (`__index = _G`); every load of this benchmark runs in such an environment, and `bench` is looked up there |
| Python      | the old namespace of the script is released; the load creates a fresh one, as for every benchmark |

## Notes per language

- TeaScript: since 0.18.0 the main script runs in a local scope of its own by default, not in the global
  scope with the Core Library. `ResetStateFor( Reset_ScriptState )` (`Reset_LocalScopes` and
  `Reset_ScriptArgs`) restarts the VM, clears the state of the parser and removes that scope and the script
  arguments. `ResetState()` (`Reset_All`) would bootstrap the Core Library again, which costs about as much
  as creating an engine (`engine_lifecycle`); the benchmark used it until 2026-11-20 (PLAN.md, decision
  log).
- ChaiScript: the state contains the functions, global objects and types of the engine; the user
  defined type conversions are not part of it (ChaiScript's documentation).
- AngelScript: what the engine has registered stays; only the script goes away.
- Lua and Python: what a script changed outside its environment (e.g. in a library or a module) stays
  changed. The old environment becomes garbage. Lua's garbage collector and Python's cycle collector
  free it later, which is not part of the reset (the functions of a Python namespace refer to it with
  `__globals__`, so it is a cycle). AngelScript frees a discarded module when nothing refers to it any
  more.

## Measurements (per sample, median)

| Language    | MSVC: reset | load     | total    | GCC 11: reset | load     | total    |
|-------------|-------------|----------|----------|---------------|----------|----------|
| TeaScript `vm-O1` | ~1.7 us     | ~27 us   | ~29 us   | ~1.5 us       | ~25 us   | ~26 us   |
| ChaiScript `no-threads` | ~21 us | ~61 us | ~82 us | ~11 us      | ~39 us   | ~49 us   |
| AngelScript `no-threads-no-line-cues` | ~2.4 us | ~60 us | ~63 us | ~1.9 us | ~51 us | ~53 us |
| Lua         | ~0.2 us     | ~7.6 us  | ~7.8 us  | ~0.2 us       | ~5.3 us  | ~5.5 us  |
| Python      | < 0.1 us    | ~63 us   | ~63 us   | < 0.1 us      | ~39 us   | ~39 us   |

For comparison, a fresh engine with MSVC (`engine_lifecycle`, create per engine): TeaScript ~1.0 ms,
ChaiScript ~3.8 ms, AngelScript ~0.44 ms, Lua ~24 us, Python ~5.4 ms, plus the load.

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
