# Result sets

Each set is measured on one machine with one toolchain. Numbers compare only within a set: the sets differ in hardware, OS and compiler. How the sets are measured: [PLAN.md](../PLAN.md), section 3 and the decision log of 2026-10-29.

| Set | CPU | OS | Virtualization | Compiler | Measured | cpp-script-bench | Status |
|---|---|---|---|---|---|---|---|
| [msvc19.44-i7-11700-windows10](msvc19.44-i7-11700-windows10/README.md) | i7-11700 | Windows 10 Pro 22H2 | Hyper-V is enabled: this OS runs in its root partition | MSVC 19.44.35229 | 2026-10-29 | 0.1.0 (18996a9) | preview |
