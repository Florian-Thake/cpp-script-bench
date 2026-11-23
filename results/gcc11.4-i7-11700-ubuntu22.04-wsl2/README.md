# Result set gcc11.4-i7-11700-ubuntu22.04-wsl2

> **Preview:** at least one language is at a pre-release version (TeaScript before 0.18.0). The set will be measured again for the release.

| | |
|---|---|
| CPU | 11th Gen Intel(R) Core(TM) i7-11700 @ 2.50GHz, 8 cores, 16 threads |
| Memory | 15.5 GiB |
| OS | Ubuntu 22.04.4 LTS (Linux 6.18.58.1-microsoft-standard-WSL2) |
| Virtualization | WSL2 (a Hyper-V VM, 16 virtual CPUs) |
| Compiler | GCC 11.4.0, Release, `-O2 -DNDEBUG -march=x86-64-v3` |
| cpp-script-bench | 0.1.0 (19b31d8) |
| AngelScript | 2.38.0 (AS_64BIT_PTR AS_NO_THREADS AS_LINUX AS_X64_GCC) |
| ChaiScript | 7.0.0 (wasm-latest-36-gf635b51e); ChaiScript_Extras (v1.0.1-13-gdd116d7) |
| Lua | 5.5.1; LuaBridge3 (3.0-rc12) |
| Python | 3.14.10 (main, Nov 22 2026, 13:49:54) [GCC 11.4.0]; pybind11 3.1.0 (v3.1.0) |
| TeaScript | 0.18.0 (v0.17.0-45-g63f3c06) |
| Measured | 2026-11-22 17:05 to 2026-11-22 18:14 UTC |
| Processes | 5 per run, interleaved across the languages; pinned to logical CPU 15, priority normal |
| Per process | 3 warm-up and 10 measured calls, time budget 30 s (at least 3 calls) |
| Machine load | other processes used 0.01 logical CPUs in the median, at most 0.04 (processes of at least 1 s) |
| Status | preview |

All numbers compare only within this set. **How to read:** each cell is the median time of one call of `bench( n )` (host API benchmarks: of one sample) over all processes: the median of the medians of the processes. Below it: the factor to C++, and the spread between the processes (slowest minus fastest process median, relative to the median); "budget": the time budget limited the number of calls. The headline variant of each language is shown; the other variants follow below. The rules are in [PLAN.md](../../PLAN.md#2-fairness-rules), the raw data in [results.json](results.json).

## Core

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/core-dark.svg">
  <img alt="Core: median time per call of each language, log scale" src="charts/core-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 15 | **700 ns**<br><sub>14.3%</sub> | **32.2 µs**<br><sub>46× · 4.0%</sub> | **1.61 ms**<br><sub>2,297× · 2.1%</sub> | **29.2 µs**<br><sub>41.8× · 7.9%</sub> | **50.4 µs**<br><sub>72× · 6.3%</sub> | **902 µs**<br><sub>1,288× · 15.6%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 20 | **8.4 µs**<br><sub>2.4%</sub> | **374 µs**<br><sub>44.5× · 13.4%</sub> | **138 ms**<br><sub>16,485× · 8.0%</sub> | **320 µs**<br><sub>38.1× · 4.5%</sub> | **542 µs**<br><sub>64.6× · 4.6%</sub> | **9.4 ms**<br><sub>1,119× · 31.6%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 25 | **122 µs**<br><sub>0.3%</sub> | **3.68 ms**<br><sub>30.2× · 2.4%</sub> | **17.5 s**<br><sub>143,174× · 0.8% · budget</sub> | **3.36 ms**<br><sub>27.6× · 1.2%</sub> | **5.75 ms**<br><sub>47.2× · 7.9%</sub> | **104 ms**<br><sub>855× · 15.0%</sub> |
| [loop](../../benchmarks/loop/README.md) | 1,000,000 | **347 µs**<br><sub>0.2%</sub> | **4.77 ms**<br><sub>13.8× · 0.3%</sub> | **180 ms**<br><sub>517× · 1.4%</sub> | **4.28 ms**<br><sub>12.3× · 1.5%</sub> | **29.5 ms**<br><sub>85× · 8.0%</sub> | **182 ms**<br><sub>523× · 39.6%</sub> |
| [int_arith](../../benchmarks/int_arith/README.md) | 5,000 | **473 µs**<br><sub>3.6%</sub> | **4.32 ms**<br><sub>9.11× · 3.4%</sub> | **106 ms**<br><sub>225× · 1.7%</sub> | **5.64 ms**<br><sub>11.9× · 0.4%</sub> | **20 ms**<br><sub>42.2× · 1.0%</sub> | **209 ms**<br><sub>441× · 26.9%</sub> |
| [mandelbrot](../../benchmarks/mandelbrot/README.md) | 100 | **417 µs**<br><sub>0.4%</sub> | **3.62 ms**<br><sub>8.7× · 0.7%</sub> | **164 ms**<br><sub>395× · 2.6%</sub> | **3.72 ms**<br><sub>8.92× · 2.8%</sub> | **16.6 ms**<br><sub>39.7× · 4.3%</sub> | **216 ms**<br><sub>518× · 1.7%</sub> |
| [math_builtins](../../benchmarks/math_builtins/README.md) | 100,000 | **131 µs**<br><sub>0.1%</sub> | **6.69 ms**<br><sub>51.2× · 0.6%</sub> | **61.1 ms**<br><sub>467× · 9.1%</sub> | **2.58 ms**<br><sub>19.7× · 2.7%</sub> | **5.71 ms**<br><sub>43.7× · 7.3%</sub> | **138 ms**<br><sub>1,052× · 4.7%</sub> |
| [calls](../../benchmarks/calls/README.md) | 100,000 | **54.1 µs**<br><sub>0.1%</sub> | **4.08 ms**<br><sub>75.5× · 10.6%</sub> | **109 ms**<br><sub>2,007× · 15.5%</sub> | **5.16 ms**<br><sub>95.4× · 4.5%</sub> | **10.7 ms**<br><sub>198× · 7.8%</sub> | **151 ms**<br><sub>2,795× · 20.7%</sub> |
| [tak](../../benchmarks/tak/README.md) | 4 | **1.5 µs**<br><sub>0.0%</sub> | **30 µs**<br><sub>20× · 6.7%</sub> | **3.02 ms**<br><sub>2,012× · 9.0%</sub> | **22.9 µs**<br><sub>15.3× · 3.7%</sub> | **39.8 µs**<br><sub>26.5× · 4.4%</sub> | **1.16 ms**<br><sub>775× · 9.9%</sub> |
| [tak](../../benchmarks/tak/README.md) | 5 | **8.9 µs**<br><sub>2.2%</sub> | **184 µs**<br><sub>20.7× · 14.9%</sub> | **89.4 ms**<br><sub>10,041× · 1.8%</sub> | **139 µs**<br><sub>15.6× · 4.1%</sub> | **232 µs**<br><sub>26.1× · 2.3%</sub> | **6.57 ms**<br><sub>738× · 8.1%</sub> |
| [tak](../../benchmarks/tak/README.md) | 6 | **54.2 µs**<br><sub>2.3%</sub> | **1.09 ms**<br><sub>20.2× · 6.9%</sub> | **3.53 s**<br><sub>65,022× · 0.1% · budget</sub> | **846 µs**<br><sub>15.6× · 2.4%</sub> | **1.37 ms**<br><sub>25.3× · 1.6%</sub> | **43 ms**<br><sub>792× · 12.8%</sub> |
| [branching](../../benchmarks/branching/README.md) | 300,000 | **212 µs**<br><sub>0.3%</sub> | **5.51 ms**<br><sub>25.9× · 0.7%</sub> | **150 ms**<br><sub>704× · 7.8%</sub> | **4.46 ms**<br><sub>21× · 0.4%</sub> | **19.2 ms**<br><sub>90.5× · 0.9%</sub> | **253 ms**<br><sub>1,192× · 16.1%</sub> |

## Data

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/data-dark.svg">
  <img alt="Data: median time per call of each language, log scale" src="charts/data-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [var_global](../../benchmarks/var_global/README.md) | 1,000,000 | **415 µs**<br><sub>5.8%</sub> | **5.55 ms**<br><sub>13.4× · 0.2%</sub> | **199 ms**<br><sub>480× · 1.7%</sub> | **10.7 ms**<br><sub>25.9× · 0.9%</sub> | **39.8 ms**<br><sub>95.9× · 2.3%</sub> | **183 ms**<br><sub>441× · 3.8%</sub> |
| [var_nested](../../benchmarks/var_nested/README.md) | 25 | **124 µs**<br><sub>3.8%</sub> | **2.88 ms**<br><sub>23.3× · 0.6%</sub> | **135 ms**<br><sub>1,094× · 1.9%</sub> | **3.96 ms**<br><sub>32× · 0.3%</sub> | **14.6 ms**<br><sub>118× · 4.2%</sub> | **158 ms**<br><sub>1,277× · 4.6%</sub> |
| [var_lifecycle](../../benchmarks/var_lifecycle/README.md) | 300,000 | **147 µs**<br><sub>1.7%</sub> | **1.81 ms**<br><sub>12.4× · 2.3%</sub> | **145 ms**<br><sub>990× · 4.4%</sub> | **2.08 ms**<br><sub>14.2× · 1.5%</sub> | **13.1 ms**<br><sub>89.5× · 4.2%</sub> | **178 ms**<br><sub>1,213× · 32.7%</sub> |
| [sieve](../../benchmarks/sieve/README.md) | 40,000 | **125 µs**<br><sub>0.4%</sub> | **2.98 ms**<br><sub>23.9× · 1.2%</sub> | **303 ms**<br><sub>2,436× · 3.9%</sub> | **756 µs**<br><sub>6.07× · 4.1%</sub> | **2.58 ms**<br><sub>20.7× · 4.6%</sub> | **23.2 ms**<br><sub>186× · 2.1%</sub> |
| [sort](../../benchmarks/sort/README.md) | 1,000 | **104 µs**<br><sub>0.0%</sub> | **6.29 ms**<br><sub>60.5× · 6.4%</sub> | **278 ms**<br><sub>2,676× · 0.4%</sub> | **4.31 ms**<br><sub>41.4× · 38.9%</sub> | **10.5 ms**<br><sub>101× · 12.1%</sub> | **167 ms**<br><sub>1,606× · 19.6%</sub> |
| [byte_buffer](../../benchmarks/byte_buffer/README.md) | 100,000 | **590 µs**<br><sub>0.6%</sub> | **2.54 ms**<br><sub>4.3× · 5.7%</sub> | **252 ms**<br><sub>426× · 6.7%</sub> | **2.15 ms**<br><sub>3.64× · 1.5%</sub> | **14.2 ms**<br><sub>24× · 8.0%</sub> | **86.8 ms**<br><sub>147× · 1.4%</sub> |
| [maps](../../benchmarks/maps/README.md) | 50,000 | **7.84 ms**<br><sub>1.0%</sub> | **53.9 ms**<br><sub>6.87× · 1.6%</sub> | **244 ms**<br><sub>31.2× · 3.8%</sub> | **12 ms**<br><sub>1.53× · 0.3%</sub> | **14 ms**<br><sub>1.78× · 2.3%</sub> | **143 ms**<br><sub>18.3× · 4.0%</sub> |
| [strings](../../benchmarks/strings/README.md) | 70,000 | **1.88 ms**<br><sub>2.1%</sub> | **47.9 ms**<br><sub>25.5× · 2.8%</sub> | **234 ms**<br><sub>124× · 8.7%</sub> | **15.7 ms**<br><sub>8.36× · 0.3%</sub> | **14.9 ms**<br><sub>7.92× · 1.0%</sub> | **86.1 ms**<br><sub>45.8× · 4.3%</sub> |
| [records](../../benchmarks/records/README.md) | 150,000 | **88.5 µs**<br><sub>2.3%</sub> | **2.63 ms**<br><sub>29.8× · 0.6%</sub> | **205 ms**<br><sub>2,321× · 1.2%</sub> | **5.01 ms**<br><sub>56.6× · 0.4%</sub> | **10.9 ms**<br><sub>123× · 5.1%</sub> | **121 ms**<br><sub>1,372× · 1.5%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 4 | **1.58 ms**<br><sub>3.2%</sub> | **50.4 ms**<br><sub>31.9× · 1.1%</sub> | **304 ms**<br><sub>192× · 3.2%</sub> | **12.7 ms**<br><sub>8.03× · 2.2%</sub> | **14.5 ms**<br><sub>9.18× · 1.0%</sub> | **182 ms**<br><sub>115× · 13.2%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 8 | **1.68 ms**<br><sub>1.5%</sub> | **80.4 ms**<br><sub>47.7× · 2.0%</sub> | **698 ms**<br><sub>414× · 2.4%</sub> | **13.4 ms**<br><sub>7.97× · 1.6%</sub> | **15.2 ms**<br><sub>9× · 6.6%</sub> | **192 ms**<br><sub>114× · 12.6%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 12 | **1.7 ms**<br><sub>2.0%</sub> | **66.6 ms**<br><sub>39.1× · 1.9%</sub> | **7.24 s**<br><sub>4,255× · 0.2% · budget</sub> | **14.5 ms**<br><sub>8.5× · 0.8%</sub> | **22.1 ms**<br><sub>13× · 1.4%</sub> | **192 ms**<br><sub>113× · 12.5%</sub> |

## Interop (script <-> C++)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/interop-dark.svg">
  <img alt="Interop (script &lt;-&gt; C++): median time per call of each language, log scale" src="charts/interop-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [script_to_host](../../benchmarks/script_to_host/README.md) | 100,000 | **54.1 µs**<br><sub>0.7%</sub> | **11.9 ms**<br><sub>221× · 1.5%</sub> | **62.5 ms**<br><sub>1,156× · 9.5%</sub> | **6.19 ms**<br><sub>115× · 0.6%</sub> | **15.8 ms**<br><sub>292× · 2.7%</sub> | **71.2 ms**<br><sub>1,317× · 2.0%</sub> |
| [host_to_script](../../benchmarks/host_to_script/README.md) | 100,000 | **54.1 µs**<br><sub>1.3%</sub> | **15.5 ms**<br><sub>286× · 0.7%</sub> | **71.4 ms**<br><sub>1,320× · 0.5%</sub> | **12.2 ms**<br><sub>226× · 0.6%</sub> | **15 ms**<br><sub>277× · 0.4%</sub> | **166 ms**<br><sub>3,077× · 18.0%</sub> |
| [cpp_objects](../../benchmarks/cpp_objects/README.md) | 150,000 | **88.5 µs**<br><sub>0.1%</sub> | **2.51 ms**<br><sub>28.4× · 0.4%</sub> | **196 ms**<br><sub>2,213× · 1.2%</sub> | **84.6 ms**<br><sub>956× · 1.1%</sub> | **115 ms**<br><sub>1,306× · 2.6%</sub> | n/a |
| [buffer](../../benchmarks/buffer/README.md) | 400 | **204 µs**<br><sub>1.2%</sub> | **2.95 ms**<br><sub>14.5× · 1.0%</sub> | **115 ms**<br><sub>563× · 5.8%</sub> | **16 ms**<br><sub>78.6× · 1.2%</sub> | **17.9 ms**<br><sub>87.7× · 4.3%</sub> | **128 ms**<br><sub>627× · 1.4%</sub> |
| [string_exchange](../../benchmarks/string_exchange/README.md) | 250,000 | **4.21 ms**<br><sub>1.3%</sub> | **26.5 ms**<br><sub>6.28× · 0.2%</sub> | **40.6 ms**<br><sub>9.62× · 0.4%</sub> | **26.4 ms**<br><sub>6.26× · 1.1%</sub> | **16.7 ms**<br><sub>3.96× · 1.4%</sub> | **103 ms**<br><sub>24.4× · 16.6%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 10 | **80.2 µs**<br><sub>109.3%</sub> | **3.66 ms**<br><sub>45.6× · 0.8%</sub> | **223 ms**<br><sub>2,783× · 5.2%</sub> | **2.02 ms**<br><sub>25.1× · 1.6%</sub> | **2.15 ms**<br><sub>26.8× · 6.1%</sub> | **22.5 ms**<br><sub>280× · 1.7%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 1,000 | **40.8 µs**<br><sub>2.9%</sub> | **2.28 ms**<br><sub>56× · 2.5%</sub> | **195 ms**<br><sub>4,791× · 5.0%</sub> | **966 µs**<br><sub>23.7× · 1.2%</sub> | **1.38 ms**<br><sub>33.7× · 3.2%</sub> | **15.7 ms**<br><sub>385× · 3.0%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 40,000 | **108 µs**<br><sub>8.7%</sub> | **2.34 ms**<br><sub>21.6× · 0.7%</sub> | **199 ms**<br><sub>1,838× · 0.9%</sub> | **1.08 ms**<br><sub>9.98× · 0.9%</sub> | **1.54 ms**<br><sub>14.3× · 7.4%</sub> | **19.8 ms**<br><sub>183× · 9.8%</sub> |

## Host API (C++ level)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/host-dark.svg">
  <img alt="Host API (C++ level): median time per call of each language, log scale" src="charts/host-light.svg">
</picture>

| Benchmark | n | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|
| [engine_lifecycle](../../benchmarks/engine_lifecycle/README.md) | 10 | **3.56 ms**<br><sub>1.3%</sub> | **22 ms**<br><sub>1.3%</sub> | **205 µs**<br><sub>1.3%</sub> | **43.5 ms**<br><sub>1.5%</sub> | **7.94 ms**<br><sub>1.1%</sub> |
| [engine_minimal](../../benchmarks/engine_minimal/README.md) | 10 | **195 µs**<br><sub>3.0%</sub> | **75.8 µs**<br><sub>6.4%</sub> | **25.6 µs**<br><sub>3.7%</sub> | n/a | **93 µs**<br><sub>4.5%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 100 | **3.05 ms**<br><sub>1.2%</sub> | **2.83 ms**<br><sub>1.0%</sub> | **310 µs**<br><sub>2.9%</sub> | **2.97 ms**<br><sub>2.4%</sub> | **1.83 ms**<br><sub>5.0%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 1,000 | **31.3 ms**<br><sub>2.0%</sub> | **32.6 ms**<br><sub>0.5%</sub> | **2.92 ms**<br><sub>2.0%</sub> | **31.7 ms**<br><sub>3.4%</sub> | **16.6 ms**<br><sub>1.1%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 10,000 | **504 ms**<br><sub>4.8%</sub> | **862 ms**<br><sub>0.4%</sub> | **28.8 ms**<br><sub>0.7%</sub> | **343 ms**<br><sub>0.5%</sub> | **193 ms**<br><sub>2.6%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 10 | **13.3 µs**<br><sub>4.1%</sub> | **24.8 µs**<br><sub>2.2%</sub> | **2 µs**<br><sub>10.0%</sub> | **25.2 µs**<br><sub>1.2%</sub> | **5.2 µs**<br><sub>4.8%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 100 | **138 µs**<br><sub>2.8%</sub> | **293 µs**<br><sub>1.9%</sub> | **15.7 µs**<br><sub>4.2%</sub> | **177 µs**<br><sub>8.6%</sub> | **29.6 µs**<br><sub>3.0%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 1,000 | **2.04 ms**<br><sub>1.1%</sub> | **7.83 ms**<br><sub>0.8%</sub> | **141 µs**<br><sub>3.7%</sub> | **1.59 ms**<br><sub>2.6%</sub> | **136 µs**<br><sub>1.8%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 10 | **10.5 µs**<br><sub>4.3%</sub> | **2 µs**<br><sub>7.5%</sub> | **3.8 µs**<br><sub>6.6%</sub> | **2.5 µs**<br><sub>20.0%</sub> | **4.9 µs**<br><sub>8.2%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 100 | **105 µs**<br><sub>0.8%</sub> | **14.5 µs**<br><sub>4.1%</sub> | **27.7 µs**<br><sub>1.8%</sub> | **15.9 µs**<br><sub>5.0%</sub> | **27.4 µs**<br><sub>3.1%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 1,000 | **1.11 ms**<br><sub>2.2%</sub> | **135 µs**<br><sub>1.0%</sub> | **247 µs**<br><sub>1.6%</sub> | **163 µs**<br><sub>5.6%</sub> | **115 µs**<br><sub>1.9%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10 | **11.4 ms**<br><sub>1.5%</sub> | **157 ms**<br><sub>5.7%</sub> | **9.89 ms**<br><sub>4.8%</sub> | **8.02 ms**<br><sub>5.8%</sub> | **51.5 ms**<br><sub>18.4%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 1,000 | **16.5 ms**<br><sub>0.5%</sub> | **225 ms**<br><sub>3.3%</sub> | **11.7 ms**<br><sub>2.8%</sub> | **9.61 ms**<br><sub>3.0%</sub> | **53 ms**<br><sub>11.4%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10,000 | **19.3 ms**<br><sub>0.5%</sub> | **823 ms**<br><sub>2.3%</sub> | **13 ms**<br><sub>2.3%</sub> | **10.7 ms**<br><sub>1.0%</sub> | **53.9 ms**<br><sub>18.4%</sub> |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10 | **4.36 ms**<br><sub>1.0%</sub> | **20.7 ms**<br><sub>4.7%</sub> | **3.78 ms**<br><sub>1.0%</sub> | **3.43 ms**<br><sub>1.6%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 1,000 | **4.39 ms**<br><sub>2.4%</sub> | **20.9 ms**<br><sub>0.8%</sub> | **3.88 ms**<br><sub>1.0%</sub> | **4.18 ms**<br><sub>2.5%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10,000 | **4.41 ms**<br><sub>1.9%</sub> | **22.4 ms**<br><sub>1.1%</sub> | **3.9 ms**<br><sub>0.9%</sub> | **4.36 ms**<br><sub>1.2%</sub> | n/a |
| [engine_reset](../../benchmarks/engine_reset/README.md) | 100 | **54.1 µs**<br><sub>3.8%</sub> | **49.2 µs**<br><sub>4.1%</sub> | **5.75 µs**<br><sub>3.5%</sub> | **54.8 µs**<br><sub>2.9%</sub> | **26.1 µs**<br><sub>2.3%</sub> |

## Series

Benchmarks with a series of workload sizes, over n (log-log).

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-fib_recursive-dark.svg">
  <img alt="fib_recursive: median time per call over n" src="charts/series-fib_recursive-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-tak-dark.svg">
  <img alt="tak: median time per call over n" src="charts/series-tak-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-binary_trees-dark.svg">
  <img alt="binary_trees: median time per call over n" src="charts/series-binary_trees-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-array_exchange-dark.svg">
  <img alt="array_exchange: median time per call over n" src="charts/series-array_exchange-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-parse_compile-dark.svg">
  <img alt="parse_compile: median time per call over n" src="charts/series-parse_compile-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-register_functions-dark.svg">
  <img alt="register_functions: median time per call over n" src="charts/series-register_functions-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-register_variables-dark.svg">
  <img alt="register_variables: median time per call over n" src="charts/series-register_variables-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-call_by_name-dark.svg">
  <img alt="call_by_name: median time per call over n" src="charts/series-call_by_name-light.svg">
</picture>

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/series-call_by_handle-dark.svg">
  <img alt="call_by_handle: median time per call over n" src="charts/series-call_by_handle-light.svg">
</picture>

## Variants

All variants of each language; the factor is relative to its headline variant.

### AngelScript

| Benchmark | n | no-threads-no-line-cues (headline) | default | no-line-cues | no-threads |
|---|--:|--:|--:|--:|--:|
| fib_recursive | 15 | **32.2 µs**<br><sub>4.0%</sub> | **31.8 µs**<br><sub>0.986× · 4.9%</sub> | **30.5 µs**<br><sub>0.946× · 11.8%</sub> | **32.1 µs**<br><sub>0.997× · 1.2%</sub> |
| fib_recursive | 20 | **374 µs**<br><sub>13.4%</sub> | **355 µs**<br><sub>0.95× · 7.7%</sub> | **338 µs**<br><sub>0.905× · 4.7%</sub> | **370 µs**<br><sub>0.988× · 6.7%</sub> |
| fib_recursive | 25 | **3.68 ms**<br><sub>2.4%</sub> | **3.71 ms**<br><sub>1.01× · 2.5%</sub> | **3.59 ms**<br><sub>0.976× · 4.8%</sub> | **3.82 ms**<br><sub>1.04× · 3.6%</sub> |
| loop | 1,000,000 | **4.77 ms**<br><sub>0.3%</sub> | **6.57 ms**<br><sub>1.38× · 0.1%</sub> | **4.32 ms**<br><sub>0.904× · 0.4%</sub> | **7.19 ms**<br><sub>1.51× · 0.3%</sub> |
| int_arith | 5,000 | **4.32 ms**<br><sub>3.4%</sub> | **5.59 ms**<br><sub>1.3× · 5.0%</sub> | **4.39 ms**<br><sub>1.02× · 0.2%</sub> | **5.34 ms**<br><sub>1.24× · 0.3%</sub> |
| mandelbrot | 100 | **3.62 ms**<br><sub>0.7%</sub> | **3.84 ms**<br><sub>1.06× · 0.6%</sub> | **3.64 ms**<br><sub>1× · 0.4%</sub> | **3.85 ms**<br><sub>1.06× · 0.5%</sub> |
| math_builtins | 100,000 | **6.69 ms**<br><sub>0.6%</sub> | **6.97 ms**<br><sub>1.04× · 1.4%</sub> | **6.78 ms**<br><sub>1.01× · 1.3%</sub> | **6.94 ms**<br><sub>1.04× · 0.6%</sub> |
| calls | 100,000 | **4.08 ms**<br><sub>10.6%</sub> | **3.99 ms**<br><sub>0.977× · 3.6%</sub> | **3.89 ms**<br><sub>0.954× · 1.0%</sub> | **4.01 ms**<br><sub>0.984× · 3.4%</sub> |
| tak | 4 | **30 µs**<br><sub>6.7%</sub> | **31.3 µs**<br><sub>1.04× · 3.4%</sub> | **30.2 µs**<br><sub>1.01× · 4.5%</sub> | **31.3 µs**<br><sub>1.04× · 4.5%</sub> |
| tak | 5 | **184 µs**<br><sub>14.9%</sub> | **185 µs**<br><sub>1× · 8.9%</sub> | **185 µs**<br><sub>1× · 9.8%</sub> | **188 µs**<br><sub>1.02× · 4.8%</sub> |
| tak | 6 | **1.09 ms**<br><sub>6.9%</sub> | **1.13 ms**<br><sub>1.04× · 16.6%</sub> | **1.09 ms**<br><sub>0.999× · 5.6%</sub> | **1.12 ms**<br><sub>1.02× · 3.7%</sub> |
| branching | 300,000 | **5.51 ms**<br><sub>0.7%</sub> | **7.54 ms**<br><sub>1.37× · 9.4%</sub> | **5.56 ms**<br><sub>1.01× · 1.0%</sub> | **7.27 ms**<br><sub>1.32× · 0.4%</sub> |
| var_global | 1,000,000 | **5.55 ms**<br><sub>0.2%</sub> | **7.59 ms**<br><sub>1.37× · 0.1%</sub> | **5.33 ms**<br><sub>0.961× · 0.1%</sub> | **8.01 ms**<br><sub>1.44× · 0.1%</sub> |
| var_nested | 25 | **2.88 ms**<br><sub>0.6%</sub> | **3.78 ms**<br><sub>1.31× · 0.3%</sub> | **2.88 ms**<br><sub>1× · 0.8%</sub> | **3.8 ms**<br><sub>1.32× · 0.5%</sub> |
| var_lifecycle | 300,000 | **1.81 ms**<br><sub>2.3%</sub> | **2.91 ms**<br><sub>1.6× · 0.6%</sub> | **1.86 ms**<br><sub>1.03× · 2.2%</sub> | **3.27 ms**<br><sub>1.8× · 0.6%</sub> |
| sieve | 40,000 | **2.98 ms**<br><sub>1.2%</sub> | **3.28 ms**<br><sub>1.1× · 0.9%</sub> | **2.98 ms**<br><sub>1× · 2.8%</sub> | **3.31 ms**<br><sub>1.11× · 2.4%</sub> |
| sort | 1,000 | **6.29 ms**<br><sub>6.4%</sub> | **6.16 ms**<br><sub>0.98× · 7.8%</sub> | **6 ms**<br><sub>0.954× · 1.7%</sub> | **6.11 ms**<br><sub>0.972× · 2.7%</sub> |
| byte_buffer | 100,000 | **2.54 ms**<br><sub>5.7%</sub> | **4.24 ms**<br><sub>1.67× · 1.8%</sub> | **4.1 ms**<br><sub>1.62× · 6.4%</sub> | **3.38 ms**<br><sub>1.33× · 3.4%</sub> |
| maps | 50,000 | **53.9 ms**<br><sub>1.6%</sub> | **54.6 ms**<br><sub>1.01× · 5.9%</sub> | **53.6 ms**<br><sub>0.995× · 0.4%</sub> | **54.6 ms**<br><sub>1.01× · 1.4%</sub> |
| strings | 70,000 | **47.9 ms**<br><sub>2.8%</sub> | **47.7 ms**<br><sub>0.996× · 0.2%</sub> | **47.3 ms**<br><sub>0.988× · 0.4%</sub> | **48.5 ms**<br><sub>1.01× · 3.0%</sub> |
| records | 150,000 | **2.63 ms**<br><sub>0.6%</sub> | **2.97 ms**<br><sub>1.13× · 0.7%</sub> | **2.63 ms**<br><sub>0.997× · 0.7%</sub> | **3.12 ms**<br><sub>1.19× · 1.2%</sub> |
| binary_trees | 4 | **50.4 ms**<br><sub>1.1%</sub> | **59.1 ms**<br><sub>1.17× · 2.0%</sub> | **58.3 ms**<br><sub>1.16× · 0.9%</sub> | **50.8 ms**<br><sub>1.01× · 1.9%</sub> |
| binary_trees | 8 | **80.4 ms**<br><sub>2.0%</sub> | **91.1 ms**<br><sub>1.13× · 2.0%</sub> | **87.6 ms**<br><sub>1.09× · 2.8%</sub> | **81.4 ms**<br><sub>1.01× · 3.5%</sub> |
| binary_trees | 12 | **66.6 ms**<br><sub>1.9%</sub> | **78.6 ms**<br><sub>1.18× · 1.6%</sub> | **74.2 ms**<br><sub>1.11× · 1.8%</sub> | **68.7 ms**<br><sub>1.03× · 2.3%</sub> |
| script_to_host | 100,000 | **11.9 ms**<br><sub>1.5%</sub> | **12 ms**<br><sub>1× · 1.3%</sub> | **11.7 ms**<br><sub>0.981× · 1.2%</sub> | **12.1 ms**<br><sub>1.02× · 0.4%</sub> |
| host_to_script | 100,000 | **15.5 ms**<br><sub>0.7%</sub> | **16.6 ms**<br><sub>1.07× · 2.5%</sub> | **16.5 ms**<br><sub>1.06× · 1.0%</sub> | **15.9 ms**<br><sub>1.03× · 1.1%</sub> |
| cpp_objects | 150,000 | **2.51 ms**<br><sub>0.4%</sub> | **3 ms**<br><sub>1.19× · 0.5%</sub> | **2.51 ms**<br><sub>0.997× · 0.6%</sub> | **3.13 ms**<br><sub>1.24× · 0.5%</sub> |
| buffer | 400 | **2.95 ms**<br><sub>1.0%</sub> | **3.25 ms**<br><sub>1.1× · 1.1%</sub> | **2.93 ms**<br><sub>0.993× · 0.6%</sub> | **3.32 ms**<br><sub>1.13× · 0.8%</sub> |
| string_exchange | 250,000 | **26.5 ms**<br><sub>0.2%</sub> | **27.5 ms**<br><sub>1.04× · 0.1%</sub> | **27.3 ms**<br><sub>1.03× · 0.2%</sub> | **26.7 ms**<br><sub>1.01× · 0.9%</sub> |
| array_exchange | 10 | **3.66 ms**<br><sub>0.8%</sub> | **3.78 ms**<br><sub>1.03× · 2.3%</sub> | **3.7 ms**<br><sub>1.01× · 1.5%</sub> | **3.79 ms**<br><sub>1.04× · 1.8%</sub> |
| array_exchange | 1,000 | **2.28 ms**<br><sub>2.5%</sub> | **2.35 ms**<br><sub>1.03× · 0.4%</sub> | **2.27 ms**<br><sub>0.994× · 1.3%</sub> | **2.38 ms**<br><sub>1.04× · 0.8%</sub> |
| array_exchange | 40,000 | **2.34 ms**<br><sub>0.7%</sub> | **2.43 ms**<br><sub>1.04× · 2.1%</sub> | **2.36 ms**<br><sub>1.01× · 1.6%</sub> | **2.43 ms**<br><sub>1.04× · 0.8%</sub> |
| engine_lifecycle | 10 | **3.56 ms**<br><sub>1.3%</sub> | **3.83 ms**<br><sub>1.08× · 2.0%</sub> | **3.83 ms**<br><sub>1.08× · 0.5%</sub> | **3.54 ms**<br><sub>0.993× · 0.3%</sub> |
| engine_minimal | 10 | **195 µs**<br><sub>3.0%</sub> | **216 µs**<br><sub>1.11× · 3.4%</sub> | **214 µs**<br><sub>1.09× · 5.6%</sub> | **194 µs**<br><sub>0.995× · 3.1%</sub> |
| parse_compile | 100 | **3.05 ms**<br><sub>1.2%</sub> | **3.18 ms**<br><sub>1.04× · 0.8%</sub> | **3.17 ms**<br><sub>1.04× · 2.4%</sub> | **3.04 ms**<br><sub>0.999× · 0.9%</sub> |
| parse_compile | 1,000 | **31.3 ms**<br><sub>2.0%</sub> | **32.6 ms**<br><sub>1.04× · 0.9%</sub> | **32.5 ms**<br><sub>1.04× · 0.1%</sub> | **31.3 ms**<br><sub>1× · 1.9%</sub> |
| parse_compile | 10,000 | **504 ms**<br><sub>4.8%</sub> | **527 ms**<br><sub>1.04× · 4.5%</sub> | **513 ms**<br><sub>1.02× · 4.5%</sub> | **505 ms**<br><sub>1× · 5.1%</sub> |
| register_functions | 10 | **13.3 µs**<br><sub>4.1%</sub> | **14.2 µs**<br><sub>1.06× · 2.8%</sub> | **14.5 µs**<br><sub>1.08× · 4.2%</sub> | **13.1 µs**<br><sub>0.978× · 1.5%</sub> |
| register_functions | 100 | **138 µs**<br><sub>2.8%</sub> | **148 µs**<br><sub>1.07× · 0.9%</sub> | **149 µs**<br><sub>1.08× · 2.6%</sub> | **137 µs**<br><sub>0.994× · 1.5%</sub> |
| register_functions | 1,000 | **2.04 ms**<br><sub>1.1%</sub> | **2.21 ms**<br><sub>1.08× · 1.6%</sub> | **2.18 ms**<br><sub>1.07× · 3.6%</sub> | **2.06 ms**<br><sub>1.01× · 2.0%</sub> |
| register_variables | 10 | **10.5 µs**<br><sub>4.3%</sub> | **11 µs**<br><sub>1.06× · 4.1%</sub> | **11 µs**<br><sub>1.05× · 2.3%</sub> | **10.3 µs**<br><sub>0.99× · 3.4%</sub> |
| register_variables | 100 | **105 µs**<br><sub>0.8%</sub> | **109 µs**<br><sub>1.04× · 3.3%</sub> | **109 µs**<br><sub>1.04× · 2.9%</sub> | **104 µs**<br><sub>0.998× · 1.6%</sub> |
| register_variables | 1,000 | **1.11 ms**<br><sub>2.2%</sub> | **1.16 ms**<br><sub>1.04× · 2.6%</sub> | **1.15 ms**<br><sub>1.03× · 3.8%</sub> | **1.13 ms**<br><sub>1.02× · 3.8%</sub> |
| call_by_name | 10 | **11.4 ms**<br><sub>1.5%</sub> | **12.1 ms**<br><sub>1.06× · 2.7%</sub> | **12.1 ms**<br><sub>1.05× · 0.8%</sub> | **11.6 ms**<br><sub>1.01× · 1.5%</sub> |
| call_by_name | 1,000 | **16.5 ms**<br><sub>0.5%</sub> | **16.8 ms**<br><sub>1.02× · 0.4%</sub> | **16.8 ms**<br><sub>1.02× · 1.6%</sub> | **16.5 ms**<br><sub>1× · 0.3%</sub> |
| call_by_name | 10,000 | **19.3 ms**<br><sub>0.5%</sub> | **19.3 ms**<br><sub>1× · 0.8%</sub> | **19.3 ms**<br><sub>0.999× · 0.6%</sub> | **19.4 ms**<br><sub>1× · 0.9%</sub> |
| call_by_handle | 10 | **4.36 ms**<br><sub>1.0%</sub> | **4.7 ms**<br><sub>1.08× · 0.7%</sub> | **4.64 ms**<br><sub>1.06× · 0.8%</sub> | **4.55 ms**<br><sub>1.04× · 0.9%</sub> |
| call_by_handle | 1,000 | **4.39 ms**<br><sub>2.4%</sub> | **4.73 ms**<br><sub>1.08× · 0.7%</sub> | **4.66 ms**<br><sub>1.06× · 0.6%</sub> | **4.56 ms**<br><sub>1.04× · 2.3%</sub> |
| call_by_handle | 10,000 | **4.41 ms**<br><sub>1.9%</sub> | **4.8 ms**<br><sub>1.09× · 1.8%</sub> | **4.7 ms**<br><sub>1.07× · 1.0%</sub> | **4.58 ms**<br><sub>1.04× · 1.2%</sub> |
| engine_reset | 100 | **54.1 µs**<br><sub>3.8%</sub> | **55.7 µs**<br><sub>1.03× · 3.5%</sub> | **55.5 µs**<br><sub>1.03× · 3.7%</sub> | **54.2 µs**<br><sub>1× · 3.6%</sub> |

### ChaiScript

| Benchmark | n | no-threads (headline) | default |
|---|--:|--:|--:|
| fib_recursive | 15 | **1.61 ms**<br><sub>2.1%</sub> | **1.68 ms**<br><sub>1.05× · 1.6%</sub> |
| fib_recursive | 20 | **138 ms**<br><sub>8.0%</sub> | **139 ms**<br><sub>1× · 2.1%</sub> |
| fib_recursive | 25 | **17.5 s**<br><sub>0.8% · budget</sub> | **17.5 s**<br><sub>1× · 0.6% · budget</sub> |
| loop | 1,000,000 | **180 ms**<br><sub>1.4%</sub> | **191 ms**<br><sub>1.06× · 4.7%</sub> |
| int_arith | 5,000 | **106 ms**<br><sub>1.7%</sub> | **112 ms**<br><sub>1.05× · 2.9%</sub> |
| mandelbrot | 100 | **164 ms**<br><sub>2.6%</sub> | **167 ms**<br><sub>1.02× · 38.3%</sub> |
| math_builtins | 100,000 | **61.1 ms**<br><sub>9.1%</sub> | **77.1 ms**<br><sub>1.26× · 10.8%</sub> |
| calls | 100,000 | **109 ms**<br><sub>15.5%</sub> | **104 ms**<br><sub>0.955× · 2.3%</sub> |
| tak | 4 | **3.02 ms**<br><sub>9.0%</sub> | **3.09 ms**<br><sub>1.02× · 8.9%</sub> |
| tak | 5 | **89.4 ms**<br><sub>1.8%</sub> | **90.1 ms**<br><sub>1.01× · 1.5%</sub> |
| tak | 6 | **3.53 s**<br><sub>0.1% · budget</sub> | **3.53 s**<br><sub>1× · 0.6% · budget</sub> |
| branching | 300,000 | **150 ms**<br><sub>7.8%</sub> | **161 ms**<br><sub>1.07× · 18.5%</sub> |
| var_global | 1,000,000 | **199 ms**<br><sub>1.7%</sub> | **241 ms**<br><sub>1.21× · 8.9%</sub> |
| var_nested | 25 | **135 ms**<br><sub>1.9%</sub> | **137 ms**<br><sub>1.01× · 5.0%</sub> |
| var_lifecycle | 300,000 | **145 ms**<br><sub>4.4%</sub> | **185 ms**<br><sub>1.27× · 21.6%</sub> |
| sieve | 40,000 | **303 ms**<br><sub>3.9%</sub> | **316 ms**<br><sub>1.04× · 5.0%</sub> |
| sort | 1,000 | **278 ms**<br><sub>0.4%</sub> | **291 ms**<br><sub>1.05× · 9.0%</sub> |
| byte_buffer | 100,000 | **252 ms**<br><sub>6.7%</sub> | **280 ms**<br><sub>1.11× · 1.4%</sub> |
| maps | 50,000 | **244 ms**<br><sub>3.8%</sub> | **262 ms**<br><sub>1.07× · 2.4%</sub> |
| strings | 70,000 | **234 ms**<br><sub>8.7%</sub> | **255 ms**<br><sub>1.09× · 6.2%</sub> |
| records | 150,000 | **205 ms**<br><sub>1.2%</sub> | **229 ms**<br><sub>1.11× · 22.8%</sub> |
| binary_trees | 4 | **304 ms**<br><sub>3.2%</sub> | **324 ms**<br><sub>1.07× · 4.8%</sub> |
| binary_trees | 8 | **698 ms**<br><sub>2.4%</sub> | **713 ms**<br><sub>1.02× · 1.9%</sub> |
| binary_trees | 12 | **7.24 s**<br><sub>0.2% · budget</sub> | **7.26 s**<br><sub>1× · 1.4% · budget</sub> |
| script_to_host | 100,000 | **62.5 ms**<br><sub>9.5%</sub> | **70.6 ms**<br><sub>1.13× · 29.9%</sub> |
| host_to_script | 100,000 | **71.4 ms**<br><sub>0.5%</sub> | **73.8 ms**<br><sub>1.03× · 0.5%</sub> |
| cpp_objects | 150,000 | **196 ms**<br><sub>1.2%</sub> | **216 ms**<br><sub>1.11× · 33.4%</sub> |
| buffer | 400 | **115 ms**<br><sub>5.8%</sub> | **121 ms**<br><sub>1.05× · 5.6%</sub> |
| string_exchange | 250,000 | **40.6 ms**<br><sub>0.4%</sub> | **42.3 ms**<br><sub>1.04× · 32.8%</sub> |
| array_exchange | 10 | **223 ms**<br><sub>5.2%</sub> | **230 ms**<br><sub>1.03× · 1.7%</sub> |
| array_exchange | 1,000 | **195 ms**<br><sub>5.0%</sub> | **202 ms**<br><sub>1.04× · 2.1%</sub> |
| array_exchange | 40,000 | **199 ms**<br><sub>0.9%</sub> | **203 ms**<br><sub>1.02× · 1.8%</sub> |
| engine_lifecycle | 10 | **22 ms**<br><sub>1.3%</sub> | **22.2 ms**<br><sub>1.01× · 0.2%</sub> |
| engine_minimal | 10 | **75.8 µs**<br><sub>6.4%</sub> | **82.7 µs**<br><sub>1.09× · 6.6%</sub> |
| parse_compile | 100 | **2.83 ms**<br><sub>1.0%</sub> | **2.8 ms**<br><sub>0.989× · 3.2%</sub> |
| parse_compile | 1,000 | **32.6 ms**<br><sub>0.5%</sub> | **32.4 ms**<br><sub>0.994× · 1.4%</sub> |
| parse_compile | 10,000 | **862 ms**<br><sub>0.4%</sub> | **858 ms**<br><sub>0.995× · 0.7%</sub> |
| register_functions | 10 | **24.8 µs**<br><sub>2.2%</sub> | **25.4 µs**<br><sub>1.03× · 1.4%</sub> |
| register_functions | 100 | **293 µs**<br><sub>1.9%</sub> | **306 µs**<br><sub>1.04× · 4.2%</sub> |
| register_functions | 1,000 | **7.83 ms**<br><sub>0.8%</sub> | **7.85 ms**<br><sub>1× · 0.9%</sub> |
| register_variables | 10 | **2 µs**<br><sub>7.5%</sub> | **2.1 µs**<br><sub>1.05× · 4.8%</sub> |
| register_variables | 100 | **14.5 µs**<br><sub>4.1%</sub> | **15.2 µs**<br><sub>1.05× · 2.6%</sub> |
| register_variables | 1,000 | **135 µs**<br><sub>1.0%</sub> | **143 µs**<br><sub>1.06× · 2.3%</sub> |
| call_by_name | 10 | **157 ms**<br><sub>5.7%</sub> | **168 ms**<br><sub>1.07× · 2.0%</sub> |
| call_by_name | 1,000 | **225 ms**<br><sub>3.3%</sub> | **235 ms**<br><sub>1.04× · 1.1%</sub> |
| call_by_name | 10,000 | **823 ms**<br><sub>2.3%</sub> | **800 ms**<br><sub>0.971× · 7.7%</sub> |
| call_by_handle | 10 | **20.7 ms**<br><sub>4.7%</sub> | **21.5 ms**<br><sub>1.04× · 3.8%</sub> |
| call_by_handle | 1,000 | **20.9 ms**<br><sub>0.8%</sub> | **21.6 ms**<br><sub>1.03× · 29.5%</sub> |
| call_by_handle | 10,000 | **22.4 ms**<br><sub>1.1%</sub> | **23.1 ms**<br><sub>1.03× · 0.8%</sub> |
| engine_reset | 100 | **49.2 µs**<br><sub>4.1%</sub> | **49.8 µs**<br><sub>1.01× · 1.5%</sub> |

### Lua

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **6.19 ms**<br><sub>0.6%</sub> | **4.15 ms**<br><sub>0.669× · 2.1%</sub> |
| host_to_script | 100,000 | **12.2 ms**<br><sub>0.6%</sub> | **8.12 ms**<br><sub>0.665× · 7.5%</sub> |
| cpp_objects | 150,000 | **84.6 ms**<br><sub>1.1%</sub> | **45.6 ms**<br><sub>0.539× · 2.7%</sub> |
| buffer | 400 | **16 ms**<br><sub>1.2%</sub> | **8.13 ms**<br><sub>0.507× · 1.3%</sub> |
| string_exchange | 250,000 | **26.4 ms**<br><sub>1.1%</sub> | **21.2 ms**<br><sub>0.803× · 1.3%</sub> |
| array_exchange | 10 | **2.02 ms**<br><sub>1.6%</sub> | **1.61 ms**<br><sub>0.796× · 1.3%</sub> |
| array_exchange | 1,000 | **966 µs**<br><sub>1.2%</sub> | **766 µs**<br><sub>0.793× · 1.0%</sub> |
| array_exchange | 40,000 | **1.08 ms**<br><sub>0.9%</sub> | **924 µs**<br><sub>0.855× · 2.4%</sub> |
| register_functions | 10 | **2 µs**<br><sub>10.0%</sub> | **900 ns**<br><sub>0.45× · 11.1%</sub> |
| register_functions | 100 | **15.7 µs**<br><sub>4.2%</sub> | **7.45 µs**<br><sub>0.476× · 6.7%</sub> |
| register_functions | 1,000 | **141 µs**<br><sub>3.7%</sub> | **69.5 µs**<br><sub>0.494× · 4.9%</sub> |
| register_variables | 10 | **3.8 µs**<br><sub>6.6%</sub> | **800 ns**<br><sub>0.211× · 12.5%</sub> |
| register_variables | 100 | **27.7 µs**<br><sub>1.8%</sub> | **7 µs**<br><sub>0.253× · 5.0%</sub> |
| register_variables | 1,000 | **247 µs**<br><sub>1.6%</sub> | **68.6 µs**<br><sub>0.277× · 3.3%</sub> |
| call_by_name | 10 | **9.89 ms**<br><sub>4.8%</sub> | **3.05 ms**<br><sub>0.309× · 12.2%</sub> |
| call_by_name | 1,000 | **11.7 ms**<br><sub>2.8%</sub> | **5.2 ms**<br><sub>0.443× · 2.8%</sub> |
| call_by_name | 10,000 | **13 ms**<br><sub>2.3%</sub> | **6.98 ms**<br><sub>0.538× · 1.4%</sub> |
| call_by_handle | 10 | **3.78 ms**<br><sub>1.0%</sub> | **2.18 ms**<br><sub>0.576× · 3.0%</sub> |
| call_by_handle | 1,000 | **3.88 ms**<br><sub>1.0%</sub> | **2.31 ms**<br><sub>0.596× · 5.5%</sub> |
| call_by_handle | 10,000 | **3.9 ms**<br><sub>0.9%</sub> | **2.46 ms**<br><sub>0.629× · 11.9%</sub> |

### Python

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **15.8 ms**<br><sub>2.7%</sub> | **7.24 ms**<br><sub>0.458× · 3.1%</sub> |
| host_to_script | 100,000 | **15 ms**<br><sub>0.4%</sub> | **12.7 ms**<br><sub>0.851× · 12.5%</sub> |
| cpp_objects | 150,000 | **115 ms**<br><sub>2.6%</sub> | **31.3 ms**<br><sub>0.271× · 7.6%</sub> |
| buffer | 400 | **17.9 ms**<br><sub>4.3%</sub> | **17.9 ms**<br><sub>0.997× · 1.7%</sub> |
| string_exchange | 250,000 | **16.7 ms**<br><sub>1.4%</sub> | **15.6 ms**<br><sub>0.934× · 0.2%</sub> |
| array_exchange | 10 | **2.15 ms**<br><sub>6.1%</sub> | **1.54 ms**<br><sub>0.717× · 3.2%</sub> |
| array_exchange | 1,000 | **1.38 ms**<br><sub>3.2%</sub> | **1.09 ms**<br><sub>0.795× · 3.7%</sub> |
| array_exchange | 40,000 | **1.54 ms**<br><sub>7.4%</sub> | **1.25 ms**<br><sub>0.81× · 2.7%</sub> |
| register_functions | 10 | **25.2 µs**<br><sub>1.2%</sub> | **2.5 µs**<br><sub>0.0994× · 12.0%</sub> |
| register_functions | 100 | **177 µs**<br><sub>8.6%</sub> | **19.7 µs**<br><sub>0.111× · 8.1%</sub> |
| register_functions | 1,000 | **1.59 ms**<br><sub>2.6%</sub> | **185 µs**<br><sub>0.116× · 4.5%</sub> |
| register_variables | 10 | **2.5 µs**<br><sub>20.0%</sub> | **1.25 µs**<br><sub>0.5× · 28.0%</sub> |
| register_variables | 100 | **15.9 µs**<br><sub>5.0%</sub> | **11.8 µs**<br><sub>0.742× · 14.8%</sub> |
| register_variables | 1,000 | **163 µs**<br><sub>5.6%</sub> | **108 µs**<br><sub>0.66× · 13.5%</sub> |
| call_by_name | 10 | **8.02 ms**<br><sub>5.8%</sub> | **6.78 ms**<br><sub>0.846× · 18.0%</sub> |
| call_by_name | 1,000 | **9.61 ms**<br><sub>3.0%</sub> | **8.45 ms**<br><sub>0.88× · 2.1%</sub> |
| call_by_name | 10,000 | **10.7 ms**<br><sub>1.0%</sub> | **9.64 ms**<br><sub>0.904× · 2.6%</sub> |
| call_by_handle | 10 | **3.43 ms**<br><sub>1.6%</sub> | **2.92 ms**<br><sub>0.852× · 2.2%</sub> |
| call_by_handle | 1,000 | **4.18 ms**<br><sub>2.5%</sub> | **3.61 ms**<br><sub>0.866× · 1.6%</sub> |
| call_by_handle | 10,000 | **4.36 ms**<br><sub>1.2%</sub> | **3.85 ms**<br><sub>0.882× · 1.6%</sub> |

### TeaScript

| Benchmark | n | vm-O1 (headline) | ast | vm-O0 | vm-O2 |
|---|--:|--:|--:|--:|--:|
| fib_recursive | 15 | **902 µs**<br><sub>15.6%</sub> | **870 µs**<br><sub>0.964× · 3.1%</sub> | **1.02 ms**<br><sub>1.13× · 16.0%</sub> | **814 µs**<br><sub>0.903× · 19.6%</sub> |
| fib_recursive | 20 | **9.4 ms**<br><sub>31.6%</sub> | **9.17 ms**<br><sub>0.976× · 2.7%</sub> | **9.34 ms**<br><sub>0.994× · 14.5%</sub> | **8.61 ms**<br><sub>0.916× · 19.7%</sub> |
| fib_recursive | 25 | **104 ms**<br><sub>15.0%</sub> | **101 ms**<br><sub>0.97× · 0.5%</sub> | **103 ms**<br><sub>0.991× · 16.3%</sub> | **97.2 ms**<br><sub>0.932× · 15.0%</sub> |
| loop | 1,000,000 | **182 ms**<br><sub>39.6%</sub> | **137 ms**<br><sub>0.753× · 2.3%</sub> | **183 ms**<br><sub>1.01× · 1.6%</sub> | **173 ms**<br><sub>0.951× · 5.7%</sub> |
| int_arith | 5,000 | **209 ms**<br><sub>26.9%</sub> | **180 ms**<br><sub>0.864× · 3.5%</sub> | **209 ms**<br><sub>1× · 24.0%</sub> | **189 ms**<br><sub>0.907× · 0.5%</sub> |
| mandelbrot | 100 | **216 ms**<br><sub>1.7%</sub> | **170 ms**<br><sub>0.79× · 1.9%</sub> | **214 ms**<br><sub>0.991× · 1.7%</sub> | **215 ms**<br><sub>0.994× · 8.1%</sub> |
| math_builtins | 100,000 | **138 ms**<br><sub>4.7%</sub> | **111 ms**<br><sub>0.81× · 1.4%</sub> | **140 ms**<br><sub>1.01× · 13.1%</sub> | **136 ms**<br><sub>0.985× · 13.4%</sub> |
| calls | 100,000 | **151 ms**<br><sub>20.7%</sub> | **109 ms**<br><sub>0.722× · 1.4%</sub> | **132 ms**<br><sub>0.871× · 19.7%</sub> | **147 ms**<br><sub>0.971× · 16.5%</sub> |
| tak | 4 | **1.16 ms**<br><sub>9.9%</sub> | **988 µs**<br><sub>0.85× · 2.7%</sub> | **1.17 ms**<br><sub>1.01× · 9.8%</sub> | **1.21 ms**<br><sub>1.04× · 9.8%</sub> |
| tak | 5 | **6.57 ms**<br><sub>8.1%</sub> | **5.56 ms**<br><sub>0.847× · 1.3%</sub> | **6.47 ms**<br><sub>0.985× · 12.8%</sub> | **6.32 ms**<br><sub>0.961× · 10.3%</sub> |
| tak | 6 | **43 ms**<br><sub>12.8%</sub> | **34.3 ms**<br><sub>0.798× · 1.0%</sub> | **42.3 ms**<br><sub>0.983× · 13.6%</sub> | **41.2 ms**<br><sub>0.958× · 9.7%</sub> |
| branching | 300,000 | **253 ms**<br><sub>16.1%</sub> | **199 ms**<br><sub>0.784× · 0.6%</sub> | **258 ms**<br><sub>1.02× · 26.6%</sub> | **242 ms**<br><sub>0.954× · 16.7%</sub> |
| var_global | 1,000,000 | **183 ms**<br><sub>3.8%</sub> | **140 ms**<br><sub>0.766× · 5.7%</sub> | **183 ms**<br><sub>1× · 38.5%</sub> | **173 ms**<br><sub>0.946× · 10.9%</sub> |
| var_nested | 25 | **158 ms**<br><sub>4.6%</sub> | **106 ms**<br><sub>0.671× · 8.2%</sub> | **159 ms**<br><sub>1× · 2.4%</sub> | **154 ms**<br><sub>0.976× · 2.7%</sub> |
| var_lifecycle | 300,000 | **178 ms**<br><sub>32.7%</sub> | **114 ms**<br><sub>0.642× · 1.9%</sub> | **175 ms**<br><sub>0.983× · 15.8%</sub> | **173 ms**<br><sub>0.974× · 2.0%</sub> |
| sieve | 40,000 | **23.2 ms**<br><sub>2.1%</sub> | **27.2 ms**<br><sub>1.17× · 1.4%</sub> | **23 ms**<br><sub>0.99× · 2.3%</sub> | **21.2 ms**<br><sub>0.912× · 2.9%</sub> |
| sort | 1,000 | **167 ms**<br><sub>19.6%</sub> | **171 ms**<br><sub>1.02× · 1.7%</sub> | **167 ms**<br><sub>0.999× · 19.9%</sub> | **166 ms**<br><sub>0.993× · 1.4%</sub> |
| byte_buffer | 100,000 | **86.8 ms**<br><sub>1.4%</sub> | **76.8 ms**<br><sub>0.884× · 1.2%</sub> | **87.6 ms**<br><sub>1.01× · 2.1%</sub> | **84.2 ms**<br><sub>0.97× · 2.1%</sub> |
| maps | 50,000 | **143 ms**<br><sub>4.0%</sub> | **144 ms**<br><sub>1.01× · 5.0%</sub> | **143 ms**<br><sub>0.994× · 1.7%</sub> | **140 ms**<br><sub>0.973× · 0.7%</sub> |
| strings | 70,000 | **86.1 ms**<br><sub>4.3%</sub> | **66 ms**<br><sub>0.767× · 10.3%</sub> | **86.2 ms**<br><sub>1× · 11.5%</sub> | **83.6 ms**<br><sub>0.972× · 14.3%</sub> |
| records | 150,000 | **121 ms**<br><sub>1.5%</sub> | **90.2 ms**<br><sub>0.743× · 1.4%</sub> | **122 ms**<br><sub>1× · 7.9%</sub> | **120 ms**<br><sub>0.988× · 41.5%</sub> |
| binary_trees | 4 | **182 ms**<br><sub>13.2%</sub> | **176 ms**<br><sub>0.967× · 1.0%</sub> | **176 ms**<br><sub>0.968× · 7.2%</sub> | **176 ms**<br><sub>0.965× · 7.3%</sub> |
| binary_trees | 8 | **192 ms**<br><sub>12.6%</sub> | **171 ms**<br><sub>0.89× · 1.2%</sub> | **189 ms**<br><sub>0.984× · 6.7%</sub> | **183 ms**<br><sub>0.955× · 11.5%</sub> |
| binary_trees | 12 | **192 ms**<br><sub>12.5%</sub> | **179 ms**<br><sub>0.929× · 0.8%</sub> | **188 ms**<br><sub>0.978× · 1.2%</sub> | **206 ms**<br><sub>1.07× · 11.7%</sub> |
| script_to_host | 100,000 | **71.2 ms**<br><sub>2.0%</sub> | **58.2 ms**<br><sub>0.818× · 3.0%</sub> | **78.9 ms**<br><sub>1.11× · 14.8%</sub> | **69.4 ms**<br><sub>0.976× · 2.8%</sub> |
| host_to_script | 100,000 | **166 ms**<br><sub>18.0%</sub> | **76.9 ms**<br><sub>0.462× · 3.8%</sub> | **167 ms**<br><sub>1.01× · 13.1%</sub> | **150 ms**<br><sub>0.9× · 19.7%</sub> |
| buffer | 400 | **128 ms**<br><sub>1.4%</sub> | **101 ms**<br><sub>0.787× · 0.5%</sub> | **130 ms**<br><sub>1.01× · 3.0%</sub> | **127 ms**<br><sub>0.994× · 6.1%</sub> |
| string_exchange | 250,000 | **103 ms**<br><sub>16.6%</sub> | **49.7 ms**<br><sub>0.484× · 0.3%</sub> | **105 ms**<br><sub>1.02× · 16.7%</sub> | **102 ms**<br><sub>0.994× · 16.8%</sub> |
| array_exchange | 10 | **22.5 ms**<br><sub>1.7%</sub> | **19 ms**<br><sub>0.847× · 1.3%</sub> | **22.3 ms**<br><sub>0.99× · 1.9%</sub> | **22.1 ms**<br><sub>0.984× · 2.0%</sub> |
| array_exchange | 1,000 | **15.7 ms**<br><sub>3.0%</sub> | **14.2 ms**<br><sub>0.906× · 2.0%</sub> | **15.5 ms**<br><sub>0.985× · 3.5%</sub> | **15.1 ms**<br><sub>0.961× · 3.3%</sub> |
| array_exchange | 40,000 | **19.8 ms**<br><sub>9.8%</sub> | **15 ms**<br><sub>0.756× · 2.2%</sub> | **19.6 ms**<br><sub>0.989× · 10.1%</sub> | **19.3 ms**<br><sub>0.973× · 0.7%</sub> |
| engine_lifecycle | 10 | **7.94 ms**<br><sub>1.1%</sub> | **4.56 ms**<br><sub>0.574× · 0.8%</sub> | **7.23 ms**<br><sub>0.91× · 1.4%</sub> | **7.94 ms**<br><sub>1× · 0.7%</sub> |
| engine_minimal | 10 | **93 µs**<br><sub>4.5%</sub> | **91.9 µs**<br><sub>0.988× · 4.5%</sub> | **91.1 µs**<br><sub>0.979× · 2.6%</sub> | **93 µs**<br><sub>0.999× · 6.7%</sub> |
| parse_compile | 100 | **1.83 ms**<br><sub>5.0%</sub> | **1.02 ms**<br><sub>0.558× · 3.1%</sub> | **1.72 ms**<br><sub>0.94× · 1.3%</sub> | **1.81 ms**<br><sub>0.991× · 2.6%</sub> |
| parse_compile | 1,000 | **16.6 ms**<br><sub>1.1%</sub> | **9.74 ms**<br><sub>0.588× · 1.2%</sub> | **19.4 ms**<br><sub>1.17× · 2.0%</sub> | **16.5 ms**<br><sub>0.994× · 0.9%</sub> |
| parse_compile | 10,000 | **193 ms**<br><sub>2.6%</sub> | **98.5 ms**<br><sub>0.511× · 1.0%</sub> | **234 ms**<br><sub>1.22× · 0.3%</sub> | **192 ms**<br><sub>0.999× · 1.4%</sub> |
| register_functions | 10 | **5.2 µs**<br><sub>4.8%</sub> | **2.1 µs**<br><sub>0.404× · 2.4%</sub> | **5.3 µs**<br><sub>1.02× · 6.6%</sub> | **5.3 µs**<br><sub>1.02× · 2.8%</sub> |
| register_functions | 100 | **29.6 µs**<br><sub>3.0%</sub> | **13.3 µs**<br><sub>0.449× · 2.3%</sub> | **29.7 µs**<br><sub>1× · 2.4%</sub> | **29.5 µs**<br><sub>0.997× · 2.5%</sub> |
| register_functions | 1,000 | **136 µs**<br><sub>1.8%</sub> | **111 µs**<br><sub>0.814× · 1.2%</sub> | **137 µs**<br><sub>1.01× · 0.9%</sub> | **137 µs**<br><sub>1× · 1.2%</sub> |
| register_variables | 10 | **4.9 µs**<br><sub>8.2%</sub> | **1.7 µs**<br><sub>0.347× · 5.9%</sub> | **4.9 µs**<br><sub>1× · 4.1%</sub> | **4.95 µs**<br><sub>1.01× · 7.1%</sub> |
| register_variables | 100 | **27.4 µs**<br><sub>3.1%</sub> | **10.9 µs**<br><sub>0.399× · 2.8%</sub> | **27.4 µs**<br><sub>1× · 6.9%</sub> | **27.4 µs**<br><sub>1× · 1.3%</sub> |
| register_variables | 1,000 | **115 µs**<br><sub>1.9%</sub> | **89.9 µs**<br><sub>0.784× · 2.8%</sub> | **115 µs**<br><sub>1× · 3.4%</sub> | **115 µs**<br><sub>1.01× · 0.7%</sub> |
| call_by_name | 10 | **51.5 ms**<br><sub>18.4%</sub> | **21.8 ms**<br><sub>0.423× · 1.0%</sub> | **47.5 ms**<br><sub>0.923× · 12.1%</sub> | **44.9 ms**<br><sub>0.872× · 21.0%</sub> |
| call_by_name | 1,000 | **53 ms**<br><sub>11.4%</sub> | **23.6 ms**<br><sub>0.446× · 1.0%</sub> | **48.5 ms**<br><sub>0.915× · 12.1%</sub> | **47.1 ms**<br><sub>0.889× · 20.7%</sub> |
| call_by_name | 10,000 | **53.9 ms**<br><sub>18.4%</sub> | **27.6 ms**<br><sub>0.511× · 1.9%</sub> | **53.7 ms**<br><sub>0.996× · 1.3%</sub> | **52.8 ms**<br><sub>0.98× · 12.9%</sub> |
| engine_reset | 100 | **26.1 µs**<br><sub>2.3%</sub> | **15.9 µs**<br><sub>0.608× · 2.2%</sub> | **28 µs**<br><sub>1.07× · 1.3%</sub> | **26.1 µs**<br><sub>0.998× · 2.5%</sub> |

## Peak memory

The peak memory of each process, the median over the processes of a run: the peak working set on Windows, the peak resident set on Linux. It covers the whole process: the runtime and libraries of the language, the unmeasured warm-up engine, the engine of the run and its workload. Below it: the difference to the process of the C++ runner for the same benchmark and n, which is the harness alone where C++ is "n/a" (the host API benchmarks). Headline variants.

### Core

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| fib_recursive | 15 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| fib_recursive | 20 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| fib_recursive | 25 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **50.5 MiB**<br><sub>+35.3 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| loop | 1,000,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| int_arith | 5,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| mandelbrot | 100 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| math_builtins | 100,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| calls | 100,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| tak | 4 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| tak | 5 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| tak | 6 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **17.0 MiB**<br><sub>+1.9 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| branching | 300,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |

### Data

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| var_global | 1,000,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| var_nested | 25 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| var_lifecycle | 300,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| sieve | 40,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.3 MiB**<br><sub>+0.2 MiB</sub> |
| sort | 1,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| byte_buffer | 100,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **24.6 MiB**<br><sub>+9.5 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| maps | 50,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **29.9 MiB**<br><sub>+14.8 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **19.3 MiB**<br><sub>+4.2 MiB</sub> | **30.6 MiB**<br><sub>+15.5 MiB</sub> |
| strings | 70,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| records | 150,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| binary_trees | 4 | 15.1 MiB | **18.0 MiB**<br><sub>+2.9 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| binary_trees | 8 | 15.1 MiB | **25.7 MiB**<br><sub>+10.5 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| binary_trees | 12 | 15.1 MiB | **26.0 MiB**<br><sub>+10.9 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |

### Interop (script <-> C++)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| script_to_host | 100,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| host_to_script | 100,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| cpp_objects | 150,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | n/a |
| buffer | 400 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| string_exchange | 250,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| array_exchange | 10 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| array_exchange | 1,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> |
| array_exchange | 40,000 | 15.1 MiB | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **21.6 MiB**<br><sub>+6.4 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **15.1 MiB**<br><sub>+0.0 MiB</sub> | **24.9 MiB**<br><sub>+9.8 MiB</sub> |

### Host API (C++ level)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| engine_lifecycle | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| engine_minimal | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | n/a | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| parse_compile | 100 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| parse_compile | 1,000 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **34.0 MiB**<br><sub>+19.5 MiB</sub> | **41.0 MiB**<br><sub>+26.4 MiB</sub> |
| parse_compile | 10,000 | 14.5 MiB | **40.4 MiB**<br><sub>+25.8 MiB</sub> | **90.3 MiB**<br><sub>+75.8 MiB</sub> | **15.3 MiB**<br><sub>+0.8 MiB</sub> | **258.5 MiB**<br><sub>+244.0 MiB</sub> | **304.1 MiB**<br><sub>+289.6 MiB</sub> |
| register_functions | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| register_functions | 100 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| register_functions | 1,000 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| register_variables | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| register_variables | 100 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| register_variables | 1,000 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| call_by_name | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| call_by_name | 1,000 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **17.7 MiB**<br><sub>+3.2 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |
| call_by_name | 10,000 | 14.5 MiB | **27.8 MiB**<br><sub>+13.3 MiB</sub> | **28.9 MiB**<br><sub>+14.4 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **78.4 MiB**<br><sub>+63.9 MiB</sub> | **58.0 MiB**<br><sub>+43.5 MiB</sub> |
| call_by_handle | 10 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | n/a |
| call_by_handle | 1,000 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **17.7 MiB**<br><sub>+3.2 MiB</sub> | n/a |
| call_by_handle | 10,000 | 14.5 MiB | **27.9 MiB**<br><sub>+13.3 MiB</sub> | **28.9 MiB**<br><sub>+14.4 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **78.4 MiB**<br><sub>+63.9 MiB</sub> | n/a |
| engine_reset | 100 | 14.5 MiB | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> | **15.1 MiB**<br><sub>+0.6 MiB</sub> |

---
Generated by `runner/report.py` from [results.json](results.json).
