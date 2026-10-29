# Result set msvc19.44-i7-11700-windows10

> **Preview:** at least one language is at a pre-release version (TeaScript before 0.18.0). The set will be measured again for the release.

| | |
|---|---|
| CPU | 11th Gen Intel(R) Core(TM) i7-11700 @ 2.50GHz, 8 cores, 16 threads |
| Memory | 31.8 GiB |
| OS | Windows 10 Pro 22H2 (Windows 10.0.19045.7807) |
| Virtualization | Hyper-V is enabled: this OS runs in its root partition |
| Compiler | MSVC 19.44.35229, Release, `/DWIN32 /D_WINDOWS /EHsc /O2 /Ob2 /DNDEBUG /arch:AVX2` |
| cpp-script-bench | 0.1.0 (18996a9) |
| AngelScript | 2.38.0 (AS_64BIT_PTR AS_NO_THREADS AS_WIN AS_X64_MSVC) |
| ChaiScript | 7.0.0 (wasm-latest-36-gf635b51e); ChaiScript_Extras (v1.0.1-13-gdd116d7) |
| Lua | 5.5.1; LuaBridge3 (3.0-rc12) |
| Python | 3.14.5 (tags/v3.14.5:5607950, May 10 2026, 10:43:50) [MSC v.1944 64 bit (AMD64)]; pybind11 3.1.0 (v3.1.0) |
| TeaScript | 0.18.0 (v0.17.0-32-gbdd696c) |
| Measured | 2026-10-29 16:14 to 2026-10-29 17:47 UTC |
| Processes | 5 per run, interleaved across the languages; pinned to logical CPU 15, priority high; power: Balanced (381b4222-f694-41f0-9685-ff5bb260df2e) |
| Per process | 3 warm-up and 10 measured calls, time budget 30 s (at least 3 calls) |
| Machine load | other processes used 0.00 logical CPUs in the median, at most 0.31 (processes of at least 1 s) |
| Status | preview |

All numbers compare only within this set. **How to read:** each cell is the median time of one call of `bench( n )` (host API benchmarks: of one sample) over all processes: the median of the medians of the processes. Below it: the factor to C++, and the spread between the processes (slowest minus fastest process median, relative to the median); "budget": the time budget limited the number of calls. The headline variant of each language is shown; the other variants follow below. The rules are in [PLAN.md](../../PLAN.md#2-fairness-rules), the raw data in [results.json](results.json).

## Core

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/core-dark.svg">
  <img alt="Core: median time per call of each language, log scale" src="charts/core-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 15 | **1.9 µs**<br><sub>0.0%</sub> | **38.4 µs**<br><sub>20.2× · 6.2%</sub> | **2.29 ms**<br><sub>1,205× · 3.7%</sub> | **31.4 µs**<br><sub>16.6× · 4.8%</sub> | **64.4 µs**<br><sub>33.9× · 21.8%</sub> | **1.16 ms**<br><sub>609× · 0.8%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 20 | **21.1 µs**<br><sub>3.5%</sub> | **432 µs**<br><sub>20.4× · 6.0%</sub> | **149 ms**<br><sub>7,044× · 0.1%</sub> | **338 µs**<br><sub>16× · 6.7%</sub> | **703 µs**<br><sub>33.2× · 1.3%</sub> | **12.8 ms**<br><sub>604× · 0.8%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 25 | **234 µs**<br><sub>2.3%</sub> | **4.73 ms**<br><sub>20.2× · 4.4%</sub> | **18.1 s**<br><sub>77,185× · 0.1% · budget</sub> | **3.83 ms**<br><sub>16.3× · 7.1%</sub> | **7.88 ms**<br><sub>33.6× · 2.5%</sub> | **142 ms**<br><sub>605× · 1.1%</sub> |
| [loop](../../benchmarks/loop/README.md) | 1,000,000 | **250 µs**<br><sub>2.1%</sub> | **8.36 ms**<br><sub>33.4× · 0.1%</sub> | **443 ms**<br><sub>1,772× · 1.5%</sub> | **4.6 ms**<br><sub>18.4× · 4.9%</sub> | **27.6 ms**<br><sub>110× · 16.7%</sub> | **242 ms**<br><sub>970× · 0.4%</sub> |
| [int_arith](../../benchmarks/int_arith/README.md) | 5,000 | **550 µs**<br><sub>7.3%</sub> | **7.23 ms**<br><sub>13.2× · 7.3%</sub> | **277 ms**<br><sub>504× · 1.6%</sub> | **5.86 ms**<br><sub>10.7× · 2.6%</sub> | **22.3 ms**<br><sub>40.5× · 5.6%</sub> | **267 ms**<br><sub>487× · 1.1%</sub> |
| [mandelbrot](../../benchmarks/mandelbrot/README.md) | 100 | **395 µs**<br><sub>0.1%</sub> | **5.69 ms**<br><sub>14.4× · 0.7%</sub> | **397 ms**<br><sub>1,004× · 1.0%</sub> | **4.85 ms**<br><sub>12.3× · 0.9%</sub> | **20.5 ms**<br><sub>51.7× · 1.0%</sub> | **265 ms**<br><sub>670× · 0.7%</sub> |
| [math_builtins](../../benchmarks/math_builtins/README.md) | 100,000 | **126 µs**<br><sub>2.2%</sub> | **5.07 ms**<br><sub>40.4× · 2.1%</sub> | **139 ms**<br><sub>1,107× · 2.7%</sub> | **3.41 ms**<br><sub>27.2× · 3.4%</sub> | **8.24 ms**<br><sub>65.7× · 9.9%</sub> | **194 ms**<br><sub>1,544× · 0.9%</sub> |
| [calls](../../benchmarks/calls/README.md) | 100,000 | **54.5 µs**<br><sub>4.2%</sub> | **5.08 ms**<br><sub>93.2× · 6.9%</sub> | **212 ms**<br><sub>3,883× · 3.3%</sub> | **4.91 ms**<br><sub>90× · 9.0%</sub> | **12 ms**<br><sub>220× · 4.8%</sub> | **169 ms**<br><sub>3,105× · 1.4%</sub> |
| [tak](../../benchmarks/tak/README.md) | 4 | **1.6 µs**<br><sub>0.0%</sub> | **40.4 µs**<br><sub>25.2× · 6.7%</sub> | **3.61 ms**<br><sub>2,258× · 0.3%</sub> | **29.5 µs**<br><sub>18.4× · 4.9%</sub> | **56.6 µs**<br><sub>35.3× · 16.4%</sub> | **1.38 ms**<br><sub>863× · 1.9%</sub> |
| [tak](../../benchmarks/tak/README.md) | 5 | **9.4 µs**<br><sub>2.1%</sub> | **233 µs**<br><sub>24.8× · 4.1%</sub> | **95 ms**<br><sub>10,102× · 0.3%</sub> | **168 µs**<br><sub>17.9× · 2.3%</sub> | **337 µs**<br><sub>35.9× · 14.7%</sub> | **8.21 ms**<br><sub>873× · 1.7%</sub> |
| [tak](../../benchmarks/tak/README.md) | 6 | **57.7 µs**<br><sub>2.3%</sub> | **1.41 ms**<br><sub>24.5× · 3.0%</sub> | **3.63 s**<br><sub>62,961× · 0.0% · budget</sub> | **1.02 ms**<br><sub>17.8× · 2.2%</sub> | **2.01 ms**<br><sub>34.9× · 4.6%</sub> | **55 ms**<br><sub>953× · 1.6%</sub> |
| [branching](../../benchmarks/branching/README.md) | 300,000 | **493 µs**<br><sub>5.1%</sub> | **9.74 ms**<br><sub>19.7× · 1.2%</sub> | **321 ms**<br><sub>651× · 1.1%</sub> | **4.82 ms**<br><sub>9.77× · 0.1%</sub> | **22 ms**<br><sub>44.7× · 9.4%</sub> | **346 ms**<br><sub>702× · 1.3%</sub> |

## Data

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/data-dark.svg">
  <img alt="Data: median time per call of each language, log scale" src="charts/data-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [var_global](../../benchmarks/var_global/README.md) | 1,000,000 | **333 µs**<br><sub>0.1%</sub> | **10.9 ms**<br><sub>32.7× · 0.1%</sub> | **510 ms**<br><sub>1,532× · 1.5%</sub> | **11.2 ms**<br><sub>33.7× · 20.9%</sub> | **37.6 ms**<br><sub>113× · 5.7%</sub> | **242 ms**<br><sub>728× · 0.2%</sub> |
| [var_nested](../../benchmarks/var_nested/README.md) | 25 | **96.2 µs**<br><sub>1.8%</sub> | **5.15 ms**<br><sub>53.5× · 19.9%</sub> | **368 ms**<br><sub>3,830× · 4.8%</sub> | **3.78 ms**<br><sub>39.3× · 0.9%</sub> | **15.1 ms**<br><sub>157× · 1.3%</sub> | **187 ms**<br><sub>1,946× · 0.5%</sub> |
| [var_lifecycle](../../benchmarks/var_lifecycle/README.md) | 300,000 | **58.1 µs**<br><sub>2.1%</sub> | **3.19 ms**<br><sub>55× · 1.4%</sub> | **368 ms**<br><sub>6,338× · 0.8%</sub> | **2.17 ms**<br><sub>37.3× · 5.5%</sub> | **13.2 ms**<br><sub>228× · 4.0%</sub> | **240 ms**<br><sub>4,134× · 1.3%</sub> |
| [sieve](../../benchmarks/sieve/README.md) | 40,000 | **236 µs**<br><sub>3.5%</sub> | **3.16 ms**<br><sub>13.4× · 0.3%</sub> | **481 ms**<br><sub>2,040× · 1.0%</sub> | **781 µs**<br><sub>3.31× · 0.5%</sub> | **2.82 ms**<br><sub>12× · 10.9%</sub> | **36.8 ms**<br><sub>156× · 1.4%</sub> |
| [sort](../../benchmarks/sort/README.md) | 1,000 | **104 µs**<br><sub>14.7%</sub> | **10 ms**<br><sub>96.7× · 0.1%</sub> | **421 ms**<br><sub>4,049× · 1.2%</sub> | **3.82 ms**<br><sub>36.7× · 0.8%</sub> | **12.8 ms**<br><sub>124× · 18.9%</sub> | **224 ms**<br><sub>2,152× · 3.2%</sub> |
| [byte_buffer](../../benchmarks/byte_buffer/README.md) | 100,000 | **636 µs**<br><sub>0.0%</sub> | **4.41 ms**<br><sub>6.94× · 0.2%</sub> | **490 ms**<br><sub>771× · 1.5%</sub> | **2.28 ms**<br><sub>3.59× · 2.9%</sub> | **14.2 ms**<br><sub>22.4× · 9.6%</sub> | **110 ms**<br><sub>173× · 0.6%</sub> |
| [maps](../../benchmarks/maps/README.md) | 50,000 | **7.78 ms**<br><sub>1.3%</sub> | **59 ms**<br><sub>7.58× · 2.6%</sub> | **404 ms**<br><sub>51.8× · 1.6%</sub> | **15.5 ms**<br><sub>1.99× · 1.8%</sub> | **15.6 ms**<br><sub>2× · 1.5%</sub> | **165 ms**<br><sub>21.2× · 3.9%</sub> |
| [strings](../../benchmarks/strings/README.md) | 70,000 | **3.22 ms**<br><sub>12.6%</sub> | **57.4 ms**<br><sub>17.9× · 0.9%</sub> | **440 ms**<br><sub>137× · 4.0%</sub> | **20.9 ms**<br><sub>6.51× · 0.4%</sub> | **16.7 ms**<br><sub>5.21× · 2.3%</sub> | **119 ms**<br><sub>36.9× · 2.1%</sub> |
| [records](../../benchmarks/records/README.md) | 150,000 | **84.8 µs**<br><sub>2.2%</sub> | **5.49 ms**<br><sub>64.7× · 6.9%</sub> | **428 ms**<br><sub>5,044× · 2.0%</sub> | **5.21 ms**<br><sub>61.4× · 7.0%</sub> | **13.7 ms**<br><sub>162× · 5.8%</sub> | **158 ms**<br><sub>1,861× · 0.7%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 4 | **4.44 ms**<br><sub>4.5%</sub> | **48.4 ms**<br><sub>10.9× · 1.5%</sub> | **539 ms**<br><sub>121× · 0.7%</sub> | **22.3 ms**<br><sub>5.02× · 1.1%</sub> | **19.2 ms**<br><sub>4.32× · 6.6%</sub> | **271 ms**<br><sub>61.1× · 0.5%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 8 | **6.68 ms**<br><sub>0.7%</sub> | **58.2 ms**<br><sub>8.71× · 0.9%</sub> | **937 ms**<br><sub>140× · 0.5%</sub> | **22.4 ms**<br><sub>3.35× · 1.7%</sub> | **19.7 ms**<br><sub>2.95× · 1.3%</sub> | **277 ms**<br><sub>41.4× · 0.6%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 12 | **6.36 ms**<br><sub>2.3%</sub> | **52.4 ms**<br><sub>8.24× · 4.5%</sub> | **7.68 s**<br><sub>1,208× · 0.2% · budget</sub> | **26.6 ms**<br><sub>4.19× · 1.1%</sub> | **27.2 ms**<br><sub>4.28× · 1.5%</sub> | **301 ms**<br><sub>47.4× · 0.6%</sub> |

## Interop (script <-> C++)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/interop-dark.svg">
  <img alt="Interop (script &lt;-&gt; C++): median time per call of each language, log scale" src="charts/interop-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [script_to_host](../../benchmarks/script_to_host/README.md) | 100,000 | **61.1 µs**<br><sub>0.2%</sub> | **7.37 ms**<br><sub>121× · 0.8%</sub> | **171 ms**<br><sub>2,791× · 6.2%</sub> | **9.97 ms**<br><sub>163× · 1.5%</sub> | **48.4 ms**<br><sub>793× · 1.2%</sub> | **94.1 ms**<br><sub>1,540× · 0.5%</sub> |
| [host_to_script](../../benchmarks/host_to_script/README.md) | 100,000 | **61.1 µs**<br><sub>2.0%</sub> | **17.5 ms**<br><sub>286× · 0.9%</sub> | **183 ms**<br><sub>2,990× · 2.0%</sub> | **16.4 ms**<br><sub>268× · 2.0%</sub> | **19.7 ms**<br><sub>323× · 5.7%</sub> | **309 ms**<br><sub>5,056× · 0.6%</sub> |
| [cpp_objects](../../benchmarks/cpp_objects/README.md) | 150,000 | **87.5 µs**<br><sub>0.1%</sub> | **4.98 ms**<br><sub>56.9× · 1.2%</sub> | **491 ms**<br><sub>5,609× · 1.9%</sub> | **107 ms**<br><sub>1,224× · 1.4%</sub> | **260 ms**<br><sub>2,976× · 1.3%</sub> | n/a |
| [buffer](../../benchmarks/buffer/README.md) | 400 | **63.7 µs**<br><sub>1.9%</sub> | **4.98 ms**<br><sub>78.3× · 1.3%</sub> | **314 ms**<br><sub>4,931× · 3.7%</sub> | **20.7 ms**<br><sub>325× · 1.2%</sub> | **18.6 ms**<br><sub>292× · 2.6%</sub> | **153 ms**<br><sub>2,410× · 1.1%</sub> |
| [string_exchange](../../benchmarks/string_exchange/README.md) | 250,000 | **5.27 ms**<br><sub>1.4%</sub> | **21.4 ms**<br><sub>4.06× · 0.7%</sub> | **82.1 ms**<br><sub>15.6× · 7.4%</sub> | **36.4 ms**<br><sub>6.9× · 1.8%</sub> | **20.7 ms**<br><sub>3.92× · 1.6%</sub> | **157 ms**<br><sub>29.8× · 1.8%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 10 | **184 µs**<br><sub>4.7%</sub> | **3.82 ms**<br><sub>20.8× · 1.4%</sub> | **400 ms**<br><sub>2,173× · 0.6%</sub> | **3.18 ms**<br><sub>17.3× · 3.0%</sub> | **2.8 ms**<br><sub>15.2× · 9.1%</sub> | **37.2 ms**<br><sub>203× · 0.6%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 1,000 | **33.2 µs**<br><sub>1.8%</sub> | **1.85 ms**<br><sub>55.7× · 1.2%</sub> | **359 ms**<br><sub>10,805× · 0.7%</sub> | **1.21 ms**<br><sub>36.3× · 4.1%</sub> | **1.62 ms**<br><sub>48.8× · 2.1%</sub> | **28.3 ms**<br><sub>853× · 0.8%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 40,000 | **53.7 µs**<br><sub>0.3%</sub> | **1.97 ms**<br><sub>36.8× · 0.6%</sub> | **359 ms**<br><sub>6,700× · 0.5%</sub> | **1.16 ms**<br><sub>21.5× · 1.5%</sub> | **1.86 ms**<br><sub>34.6× · 4.1%</sub> | **28.6 ms**<br><sub>533× · 0.3%</sub> |

## Host API (C++ level)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/host-dark.svg">
  <img alt="Host API (C++ level): median time per call of each language, log scale" src="charts/host-light.svg">
</picture>

| Benchmark | n | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|
| [engine_lifecycle](../../benchmarks/engine_lifecycle/README.md) | 10 | **4.95 ms**<br><sub>1.4%</sub> | **40.5 ms**<br><sub>0.9%</sub> | **286 µs**<br><sub>8.2%</sub> | **63 ms**<br><sub>1.4%</sub> | **12.6 ms**<br><sub>2.4%</sub> |
| [engine_minimal](../../benchmarks/engine_minimal/README.md) | 10 | **249 µs**<br><sub>5.2%</sub> | **230 µs**<br><sub>2.6%</sub> | **43.8 µs**<br><sub>2.4%</sub> | n/a | **163 µs**<br><sub>0.8%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 100 | **3.92 ms**<br><sub>0.9%</sub> | **4.91 ms**<br><sub>1.7%</sub> | **421 µs**<br><sub>3.4%</sub> | **3.94 ms**<br><sub>1.3%</sub> | **3.11 ms**<br><sub>5.3%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 1,000 | **38.7 ms**<br><sub>0.4%</sub> | **55.6 ms**<br><sub>0.8%</sub> | **4.22 ms**<br><sub>0.6%</sub> | **39.9 ms**<br><sub>1.7%</sub> | **27.6 ms**<br><sub>1.1%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 10,000 | **654 ms**<br><sub>0.2%</sub> | **1.33 s**<br><sub>0.5%</sub> | **40.6 ms**<br><sub>0.7%</sub> | **435 ms**<br><sub>0.9%</sub> | **279 ms**<br><sub>0.7%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 10 | **20.9 µs**<br><sub>1.2%</sub> | **48.5 µs**<br><sub>1.5%</sub> | **2.95 µs**<br><sub>6.8%</sub> | **36 µs**<br><sub>4.7%</sub> | **3.25 µs**<br><sub>21.5%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 100 | **183 µs**<br><sub>2.5%</sub> | **529 µs**<br><sub>1.9%</sub> | **20.2 µs**<br><sub>7.2%</sub> | **256 µs**<br><sub>1.8%</sub> | **41.8 µs**<br><sub>5.9%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 1,000 | **2.65 ms**<br><sub>1.6%</sub> | **12.9 ms**<br><sub>4.7%</sub> | **227 µs**<br><sub>2.2%</sub> | **2.37 ms**<br><sub>1.9%</sub> | **299 µs**<br><sub>4.3%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 10 | **12.8 µs**<br><sub>3.9%</sub> | **2.3 µs**<br><sub>6.5%</sub> | **4.95 µs**<br><sub>4.0%</sub> | **2.6 µs**<br><sub>13.5%</sub> | **2.3 µs**<br><sub>26.1%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 100 | **134 µs**<br><sub>1.0%</sub> | **21.3 µs**<br><sub>3.1%</sub> | **36.2 µs**<br><sub>2.6%</sub> | **18.3 µs**<br><sub>6.0%</sub> | **31.5 µs**<br><sub>6.8%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 1,000 | **1.43 ms**<br><sub>2.8%</sub> | **255 µs**<br><sub>2.1%</sub> | **381 µs**<br><sub>5.8%</sub> | **194 µs**<br><sub>6.5%</sub> | **221 µs**<br><sub>5.8%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10 | **12.1 ms**<br><sub>0.8%</sub> | **285 ms**<br><sub>1.9%</sub> | **11.8 ms**<br><sub>6.3%</sub> | **10.3 ms**<br><sub>12.5%</sub> | **68.3 ms**<br><sub>1.4%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 1,000 | **19 ms**<br><sub>0.3%</sub> | **412 ms**<br><sub>0.7%</sub> | **13.4 ms**<br><sub>2.1%</sub> | **11.6 ms**<br><sub>2.2%</sub> | **69.3 ms**<br><sub>1.2%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10,000 | **23.2 ms**<br><sub>0.4%</sub> | **1.45 s**<br><sub>0.7%</sub> | **15.2 ms**<br><sub>4.0%</sub> | **12.9 ms**<br><sub>3.4%</sub> | **77 ms**<br><sub>1.7%</sub> |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10 | **4.94 ms**<br><sub>1.2%</sub> | **48.1 ms**<br><sub>2.7%</sub> | **4.85 ms**<br><sub>1.9%</sub> | **4.93 ms**<br><sub>1.2%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 1,000 | **5.3 ms**<br><sub>3.0%</sub> | **50.2 ms**<br><sub>3.3%</sub> | **5.02 ms**<br><sub>2.1%</sub> | **5.48 ms**<br><sub>4.1%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10,000 | **5.38 ms**<br><sub>4.6%</sub> | **53 ms**<br><sub>3.5%</sub> | **5.3 ms**<br><sub>2.2%</sub> | **5.54 ms**<br><sub>2.7%</sub> | n/a |
| [engine_reset](../../benchmarks/engine_reset/README.md) | 100 | **66.1 µs**<br><sub>2.1%</sub> | **82.6 µs**<br><sub>0.8%</sub> | **7.5 µs**<br><sub>6.0%</sub> | **65.4 µs**<br><sub>7.6%</sub> | **1.26 ms**<br><sub>2.5%</sub> |

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
| fib_recursive | 15 | **38.4 µs**<br><sub>6.2%</sub> | **41.1 µs**<br><sub>1.07× · 4.0%</sub> | **36.2 µs**<br><sub>0.943× · 6.4%</sub> | **43.4 µs**<br><sub>1.13× · 4.1%</sub> |
| fib_recursive | 20 | **432 µs**<br><sub>6.0%</sub> | **441 µs**<br><sub>1.02× · 0.2%</sub> | **386 µs**<br><sub>0.893× · 5.4%</sub> | **483 µs**<br><sub>1.12× · 3.8%</sub> |
| fib_recursive | 25 | **4.73 ms**<br><sub>4.4%</sub> | **5.04 ms**<br><sub>1.07× · 3.7%</sub> | **4.27 ms**<br><sub>0.904× · 2.2%</sub> | **5.44 ms**<br><sub>1.15× · 8.2%</sub> |
| loop | 1,000,000 | **8.36 ms**<br><sub>0.1%</sub> | **10.9 ms**<br><sub>1.3× · 0.1%</sub> | **7.52 ms**<br><sub>0.9× · 0.0%</sub> | **12.1 ms**<br><sub>1.45× · 0.0%</sub> |
| int_arith | 5,000 | **7.23 ms**<br><sub>7.3%</sub> | **7.59 ms**<br><sub>1.05× · 1.5%</sub> | **6.38 ms**<br><sub>0.882× · 0.5%</sub> | **8.76 ms**<br><sub>1.21× · 0.3%</sub> |
| mandelbrot | 100 | **5.69 ms**<br><sub>0.7%</sub> | **5.97 ms**<br><sub>1.05× · 2.5%</sub> | **5.09 ms**<br><sub>0.895× · 1.1%</sub> | **6.63 ms**<br><sub>1.16× · 0.8%</sub> |
| math_builtins | 100,000 | **5.07 ms**<br><sub>2.1%</sub> | **5.07 ms**<br><sub>1× · 2.2%</sub> | **4.78 ms**<br><sub>0.944× · 21.7%</sub> | **5.45 ms**<br><sub>1.08× · 1.9%</sub> |
| calls | 100,000 | **5.08 ms**<br><sub>6.9%</sub> | **5.35 ms**<br><sub>1.05× · 15.6%</sub> | **4.63 ms**<br><sub>0.912× · 3.4%</sub> | **5.79 ms**<br><sub>1.14× · 3.8%</sub> |
| tak | 4 | **40.4 µs**<br><sub>6.7%</sub> | **41.4 µs**<br><sub>1.03× · 5.0%</sub> | **37.5 µs**<br><sub>0.929× · 5.6%</sub> | **43.9 µs**<br><sub>1.09× · 2.4%</sub> |
| tak | 5 | **233 µs**<br><sub>4.1%</sub> | **244 µs**<br><sub>1.05× · 0.6%</sub> | **219 µs**<br><sub>0.94× · 2.3%</sub> | **258 µs**<br><sub>1.11× · 3.3%</sub> |
| tak | 6 | **1.41 ms**<br><sub>3.0%</sub> | **1.5 ms**<br><sub>1.06× · 3.3%</sub> | **1.35 ms**<br><sub>0.953× · 1.5%</sub> | **1.58 ms**<br><sub>1.12× · 1.2%</sub> |
| branching | 300,000 | **9.74 ms**<br><sub>1.2%</sub> | **11.3 ms**<br><sub>1.16× · 0.7%</sub> | **8.33 ms**<br><sub>0.855× · 0.4%</sub> | **12.7 ms**<br><sub>1.3× · 1.0%</sub> |
| var_global | 1,000,000 | **10.9 ms**<br><sub>0.1%</sub> | **13.4 ms**<br><sub>1.23× · 0.1%</sub> | **10 ms**<br><sub>0.923× · 0.1%</sub> | **14.6 ms**<br><sub>1.35× · 0.1%</sub> |
| var_nested | 25 | **5.15 ms**<br><sub>19.9%</sub> | **6.07 ms**<br><sub>1.18× · 4.1%</sub> | **4.58 ms**<br><sub>0.89× · 3.7%</sub> | **6.68 ms**<br><sub>1.3× · 0.0%</sub> |
| var_lifecycle | 300,000 | **3.19 ms**<br><sub>1.4%</sub> | **5.07 ms**<br><sub>1.59× · 0.0%</sub> | **2.95 ms**<br><sub>0.922× · 0.9%</sub> | **5.45 ms**<br><sub>1.71× · 0.1%</sub> |
| sieve | 40,000 | **3.16 ms**<br><sub>0.3%</sub> | **3.46 ms**<br><sub>1.1× · 0.6%</sub> | **2.93 ms**<br><sub>0.927× · 0.5%</sub> | **3.73 ms**<br><sub>1.18× · 0.8%</sub> |
| sort | 1,000 | **10 ms**<br><sub>0.1%</sub> | **9 ms**<br><sub>0.896× · 1.7%</sub> | **8.49 ms**<br><sub>0.846× · 0.2%</sub> | **10.7 ms**<br><sub>1.07× · 0.6%</sub> |
| byte_buffer | 100,000 | **4.41 ms**<br><sub>0.2%</sub> | **4.76 ms**<br><sub>1.08× · 0.1%</sub> | **3.83 ms**<br><sub>0.867× · 0.5%</sub> | **5.42 ms**<br><sub>1.23× · 0.1%</sub> |
| maps | 50,000 | **59 ms**<br><sub>2.6%</sub> | **57.9 ms**<br><sub>0.98× · 0.9%</sub> | **56.9 ms**<br><sub>0.964× · 1.1%</sub> | **60.2 ms**<br><sub>1.02× · 1.0%</sub> |
| strings | 70,000 | **57.4 ms**<br><sub>0.9%</sub> | **55.8 ms**<br><sub>0.971× · 9.8%</sub> | **55.3 ms**<br><sub>0.963× · 1.0%</sub> | **57.8 ms**<br><sub>1.01× · 1.0%</sub> |
| records | 150,000 | **5.49 ms**<br><sub>6.9%</sub> | **5.45 ms**<br><sub>0.994× · 0.0%</sub> | **4.64 ms**<br><sub>0.845× · 0.1%</sub> | **6.42 ms**<br><sub>1.17× · 0.0%</sub> |
| binary_trees | 4 | **48.4 ms**<br><sub>1.5%</sub> | **57.5 ms**<br><sub>1.19× · 1.0%</sub> | **56.7 ms**<br><sub>1.17× · 0.8%</sub> | **48.9 ms**<br><sub>1.01× · 1.0%</sub> |
| binary_trees | 8 | **58.2 ms**<br><sub>0.9%</sub> | **69.2 ms**<br><sub>1.19× · 2.1%</sub> | **68 ms**<br><sub>1.17× · 2.3%</sub> | **58.9 ms**<br><sub>1.01× · 1.0%</sub> |
| binary_trees | 12 | **52.4 ms**<br><sub>4.5%</sub> | **63.8 ms**<br><sub>1.22× · 4.8%</sub> | **62.7 ms**<br><sub>1.2× · 1.8%</sub> | **53.5 ms**<br><sub>1.02× · 1.7%</sub> |
| script_to_host | 100,000 | **7.37 ms**<br><sub>0.8%</sub> | **7.18 ms**<br><sub>0.975× · 1.0%</sub> | **6.87 ms**<br><sub>0.933× · 1.1%</sub> | **7.78 ms**<br><sub>1.06× · 1.4%</sub> |
| host_to_script | 100,000 | **17.5 ms**<br><sub>0.9%</sub> | **19 ms**<br><sub>1.09× · 0.7%</sub> | **18.5 ms**<br><sub>1.06× · 1.4%</sub> | **17.8 ms**<br><sub>1.02× · 1.1%</sub> |
| cpp_objects | 150,000 | **4.98 ms**<br><sub>1.2%</sub> | **5.29 ms**<br><sub>1.06× · 0.0%</sub> | **4.42 ms**<br><sub>0.887× · 0.0%</sub> | **5.92 ms**<br><sub>1.19× · 25.9%</sub> |
| buffer | 400 | **4.98 ms**<br><sub>1.3%</sub> | **5.05 ms**<br><sub>1.01× · 0.0%</sub> | **4.51 ms**<br><sub>0.906× · 0.0%</sub> | **5.59 ms**<br><sub>1.12× · 1.1%</sub> |
| string_exchange | 250,000 | **21.4 ms**<br><sub>0.7%</sub> | **21.2 ms**<br><sub>0.993× · 1.1%</sub> | **20.9 ms**<br><sub>0.978× · 0.5%</sub> | **21.7 ms**<br><sub>1.01× · 0.7%</sub> |
| array_exchange | 10 | **3.82 ms**<br><sub>1.4%</sub> | **3.87 ms**<br><sub>1.01× · 2.1%</sub> | **3.73 ms**<br><sub>0.978× · 3.5%</sub> | **3.99 ms**<br><sub>1.05× · 1.6%</sub> |
| array_exchange | 1,000 | **1.85 ms**<br><sub>1.2%</sub> | **1.84 ms**<br><sub>0.995× · 0.3%</sub> | **1.71 ms**<br><sub>0.925× · 0.8%</sub> | **1.99 ms**<br><sub>1.08× · 0.6%</sub> |
| array_exchange | 40,000 | **1.97 ms**<br><sub>0.6%</sub> | **1.99 ms**<br><sub>1.01× · 2.7%</sub> | **1.87 ms**<br><sub>0.946× · 2.3%</sub> | **2.13 ms**<br><sub>1.08× · 1.4%</sub> |
| engine_lifecycle | 10 | **4.95 ms**<br><sub>1.4%</sub> | **5.56 ms**<br><sub>1.12× · 0.8%</sub> | **5.55 ms**<br><sub>1.12× · 1.3%</sub> | **4.99 ms**<br><sub>1.01× · 1.3%</sub> |
| engine_minimal | 10 | **249 µs**<br><sub>5.2%</sub> | **301 µs**<br><sub>1.21× · 2.9%</sub> | **301 µs**<br><sub>1.21× · 3.8%</sub> | **245 µs**<br><sub>0.987× · 1.5%</sub> |
| parse_compile | 100 | **3.92 ms**<br><sub>0.9%</sub> | **4.12 ms**<br><sub>1.05× · 1.6%</sub> | **4.09 ms**<br><sub>1.04× · 0.9%</sub> | **3.94 ms**<br><sub>1× · 0.7%</sub> |
| parse_compile | 1,000 | **38.7 ms**<br><sub>0.4%</sub> | **40.4 ms**<br><sub>1.05× · 0.4%</sub> | **40.3 ms**<br><sub>1.04× · 0.6%</sub> | **38.8 ms**<br><sub>1× · 0.3%</sub> |
| parse_compile | 10,000 | **654 ms**<br><sub>0.2%</sub> | **673 ms**<br><sub>1.03× · 0.8%</sub> | **671 ms**<br><sub>1.03× · 0.3%</sub> | **656 ms**<br><sub>1× · 0.7%</sub> |
| register_functions | 10 | **20.9 µs**<br><sub>1.2%</sub> | **23.6 µs**<br><sub>1.13× · 5.7%</sub> | **25.2 µs**<br><sub>1.21× · 9.3%</sub> | **20.4 µs**<br><sub>0.976× · 4.4%</sub> |
| register_functions | 100 | **183 µs**<br><sub>2.5%</sub> | **201 µs**<br><sub>1.1× · 2.8%</sub> | **198 µs**<br><sub>1.08× · 3.9%</sub> | **187 µs**<br><sub>1.02× · 3.0%</sub> |
| register_functions | 1,000 | **2.65 ms**<br><sub>1.6%</sub> | **2.81 ms**<br><sub>1.06× · 1.5%</sub> | **2.82 ms**<br><sub>1.07× · 1.6%</sub> | **2.65 ms**<br><sub>1× · 1.2%</sub> |
| register_variables | 10 | **12.8 µs**<br><sub>3.9%</sub> | **15.2 µs**<br><sub>1.19× · 1.3%</sub> | **14.5 µs**<br><sub>1.13× · 4.5%</sub> | **13.4 µs**<br><sub>1.05× · 7.1%</sub> |
| register_variables | 100 | **134 µs**<br><sub>1.0%</sub> | **137 µs**<br><sub>1.03× · 1.8%</sub> | **136 µs**<br><sub>1.02× · 1.2%</sub> | **134 µs**<br><sub>0.998× · 1.5%</sub> |
| register_variables | 1,000 | **1.43 ms**<br><sub>2.8%</sub> | **1.47 ms**<br><sub>1.03× · 1.2%</sub> | **1.47 ms**<br><sub>1.03× · 1.5%</sub> | **1.43 ms**<br><sub>0.999× · 0.9%</sub> |
| call_by_name | 10 | **12.1 ms**<br><sub>0.8%</sub> | **12.3 ms**<br><sub>1.02× · 1.1%</sub> | **12.2 ms**<br><sub>1.01× · 0.6%</sub> | **12.1 ms**<br><sub>1.01× · 0.6%</sub> |
| call_by_name | 1,000 | **19 ms**<br><sub>0.3%</sub> | **18.9 ms**<br><sub>0.995× · 0.2%</sub> | **18.7 ms**<br><sub>0.987× · 0.6%</sub> | **19.1 ms**<br><sub>1.01× · 1.0%</sub> |
| call_by_name | 10,000 | **23.2 ms**<br><sub>0.4%</sub> | **22.9 ms**<br><sub>0.99× · 0.8%</sub> | **22.8 ms**<br><sub>0.983× · 0.9%</sub> | **23.3 ms**<br><sub>1.01× · 0.7%</sub> |
| call_by_handle | 10 | **4.94 ms**<br><sub>1.2%</sub> | **5.59 ms**<br><sub>1.13× · 1.3%</sub> | **5.51 ms**<br><sub>1.11× · 1.4%</sub> | **5.07 ms**<br><sub>1.03× · 1.3%</sub> |
| call_by_handle | 1,000 | **5.3 ms**<br><sub>3.0%</sub> | **5.84 ms**<br><sub>1.1× · 2.2%</sub> | **5.69 ms**<br><sub>1.07× · 0.8%</sub> | **5.38 ms**<br><sub>1.01× · 2.3%</sub> |
| call_by_handle | 10,000 | **5.38 ms**<br><sub>4.6%</sub> | **5.85 ms**<br><sub>1.09× · 1.4%</sub> | **5.79 ms**<br><sub>1.08× · 2.3%</sub> | **5.49 ms**<br><sub>1.02× · 2.4%</sub> |
| engine_reset | 100 | **66.1 µs**<br><sub>2.1%</sub> | **80.4 µs**<br><sub>1.21× · 1.0%</sub> | **80.4 µs**<br><sub>1.21× · 5.4%</sub> | **66 µs**<br><sub>0.998× · 5.9%</sub> |

### ChaiScript

| Benchmark | n | no-threads (headline) | default |
|---|--:|--:|--:|
| fib_recursive | 15 | **2.29 ms**<br><sub>3.7%</sub> | **2.38 ms**<br><sub>1.04× · 0.5%</sub> |
| fib_recursive | 20 | **149 ms**<br><sub>0.1%</sub> | **150 ms**<br><sub>1.01× · 0.2%</sub> |
| fib_recursive | 25 | **18.1 s**<br><sub>0.1% · budget</sub> | **18.1 s**<br><sub>1× · 0.1% · budget</sub> |
| loop | 1,000,000 | **443 ms**<br><sub>1.5%</sub> | **467 ms**<br><sub>1.05× · 1.4%</sub> |
| int_arith | 5,000 | **277 ms**<br><sub>1.6%</sub> | **297 ms**<br><sub>1.07× · 1.6%</sub> |
| mandelbrot | 100 | **397 ms**<br><sub>1.0%</sub> | **423 ms**<br><sub>1.07× · 1.1%</sub> |
| math_builtins | 100,000 | **139 ms**<br><sub>2.7%</sub> | **134 ms**<br><sub>0.963× · 1.3%</sub> |
| calls | 100,000 | **212 ms**<br><sub>3.3%</sub> | **238 ms**<br><sub>1.13× · 2.6%</sub> |
| tak | 4 | **3.61 ms**<br><sub>0.3%</sub> | **3.7 ms**<br><sub>1.02× · 0.6%</sub> |
| tak | 5 | **95 ms**<br><sub>0.3%</sub> | **95.1 ms**<br><sub>1× · 0.6%</sub> |
| tak | 6 | **3.63 s**<br><sub>0.0% · budget</sub> | **3.62 s**<br><sub>0.997× · 0.1% · budget</sub> |
| branching | 300,000 | **321 ms**<br><sub>1.1%</sub> | **354 ms**<br><sub>1.1× · 2.6%</sub> |
| var_global | 1,000,000 | **510 ms**<br><sub>1.5%</sub> | **540 ms**<br><sub>1.06× · 1.2%</sub> |
| var_nested | 25 | **368 ms**<br><sub>4.8%</sub> | **376 ms**<br><sub>1.02× · 1.1%</sub> |
| var_lifecycle | 300,000 | **368 ms**<br><sub>0.8%</sub> | **385 ms**<br><sub>1.05× · 4.2%</sub> |
| sieve | 40,000 | **481 ms**<br><sub>1.0%</sub> | **516 ms**<br><sub>1.07× · 0.7%</sub> |
| sort | 1,000 | **421 ms**<br><sub>1.2%</sub> | **450 ms**<br><sub>1.07× · 1.3%</sub> |
| byte_buffer | 100,000 | **490 ms**<br><sub>1.5%</sub> | **515 ms**<br><sub>1.05× · 0.5%</sub> |
| maps | 50,000 | **404 ms**<br><sub>1.6%</sub> | **425 ms**<br><sub>1.05× · 0.9%</sub> |
| strings | 70,000 | **440 ms**<br><sub>4.0%</sub> | **459 ms**<br><sub>1.04× · 3.1%</sub> |
| records | 150,000 | **428 ms**<br><sub>2.0%</sub> | **467 ms**<br><sub>1.09× · 2.0%</sub> |
| binary_trees | 4 | **539 ms**<br><sub>0.7%</sub> | **560 ms**<br><sub>1.04× · 0.4%</sub> |
| binary_trees | 8 | **937 ms**<br><sub>0.5%</sub> | **990 ms**<br><sub>1.06× · 0.4%</sub> |
| binary_trees | 12 | **7.68 s**<br><sub>0.2% · budget</sub> | **7.71 s**<br><sub>1× · 0.2% · budget</sub> |
| script_to_host | 100,000 | **171 ms**<br><sub>6.2%</sub> | **167 ms**<br><sub>0.977× · 3.6%</sub> |
| host_to_script | 100,000 | **183 ms**<br><sub>2.0%</sub> | **189 ms**<br><sub>1.03× · 1.6%</sub> |
| cpp_objects | 150,000 | **491 ms**<br><sub>1.9%</sub> | **507 ms**<br><sub>1.03× · 3.1%</sub> |
| buffer | 400 | **314 ms**<br><sub>3.7%</sub> | **321 ms**<br><sub>1.02× · 5.8%</sub> |
| string_exchange | 250,000 | **82.1 ms**<br><sub>7.4%</sub> | **84.3 ms**<br><sub>1.03× · 2.4%</sub> |
| array_exchange | 10 | **400 ms**<br><sub>0.6%</sub> | **413 ms**<br><sub>1.03× · 1.1%</sub> |
| array_exchange | 1,000 | **359 ms**<br><sub>0.7%</sub> | **369 ms**<br><sub>1.03× · 1.1%</sub> |
| array_exchange | 40,000 | **359 ms**<br><sub>0.5%</sub> | **369 ms**<br><sub>1.03× · 0.8%</sub> |
| engine_lifecycle | 10 | **40.5 ms**<br><sub>0.9%</sub> | **41 ms**<br><sub>1.01× · 0.9%</sub> |
| engine_minimal | 10 | **230 µs**<br><sub>2.6%</sub> | **242 µs**<br><sub>1.05× · 2.2%</sub> |
| parse_compile | 100 | **4.91 ms**<br><sub>1.7%</sub> | **4.94 ms**<br><sub>1.01× · 1.2%</sub> |
| parse_compile | 1,000 | **55.6 ms**<br><sub>0.8%</sub> | **56.2 ms**<br><sub>1.01× · 0.5%</sub> |
| parse_compile | 10,000 | **1.33 s**<br><sub>0.5%</sub> | **1.36 s**<br><sub>1.02× · 0.2%</sub> |
| register_functions | 10 | **48.5 µs**<br><sub>1.5%</sub> | **51.8 µs**<br><sub>1.07× · 11.0%</sub> |
| register_functions | 100 | **529 µs**<br><sub>1.9%</sub> | **539 µs**<br><sub>1.02× · 4.5%</sub> |
| register_functions | 1,000 | **12.9 ms**<br><sub>4.7%</sub> | **13.2 ms**<br><sub>1.03× · 0.3%</sub> |
| register_variables | 10 | **2.3 µs**<br><sub>6.5%</sub> | **2.4 µs**<br><sub>1.04× · 0.0%</sub> |
| register_variables | 100 | **21.3 µs**<br><sub>3.1%</sub> | **21.9 µs**<br><sub>1.03× · 3.0%</sub> |
| register_variables | 1,000 | **255 µs**<br><sub>2.1%</sub> | **275 µs**<br><sub>1.08× · 3.8%</sub> |
| call_by_name | 10 | **285 ms**<br><sub>1.9%</sub> | **305 ms**<br><sub>1.07× · 0.7%</sub> |
| call_by_name | 1,000 | **412 ms**<br><sub>0.7%</sub> | **435 ms**<br><sub>1.06× · 0.3%</sub> |
| call_by_name | 10,000 | **1.45 s**<br><sub>0.7%</sub> | **1.55 s**<br><sub>1.07× · 0.3%</sub> |
| call_by_handle | 10 | **48.1 ms**<br><sub>2.7%</sub> | **50.9 ms**<br><sub>1.06× · 1.3%</sub> |
| call_by_handle | 1,000 | **50.2 ms**<br><sub>3.3%</sub> | **53.3 ms**<br><sub>1.06× · 2.9%</sub> |
| call_by_handle | 10,000 | **53 ms**<br><sub>3.5%</sub> | **55.6 ms**<br><sub>1.05× · 3.1%</sub> |
| engine_reset | 100 | **82.6 µs**<br><sub>0.8%</sub> | **84.2 µs**<br><sub>1.02× · 1.2%</sub> |

### Lua

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **9.97 ms**<br><sub>1.5%</sub> | **4.96 ms**<br><sub>0.498× · 1.6%</sub> |
| host_to_script | 100,000 | **16.4 ms**<br><sub>2.0%</sub> | **10.3 ms**<br><sub>0.63× · 3.6%</sub> |
| cpp_objects | 150,000 | **107 ms**<br><sub>1.4%</sub> | **64.6 ms**<br><sub>0.603× · 8.0%</sub> |
| buffer | 400 | **20.7 ms**<br><sub>1.2%</sub> | **9.79 ms**<br><sub>0.473× · 3.1%</sub> |
| string_exchange | 250,000 | **36.4 ms**<br><sub>1.8%</sub> | **30.7 ms**<br><sub>0.845× · 2.8%</sub> |
| array_exchange | 10 | **3.18 ms**<br><sub>3.0%</sub> | **2.56 ms**<br><sub>0.806× · 2.3%</sub> |
| array_exchange | 1,000 | **1.21 ms**<br><sub>4.1%</sub> | **947 µs**<br><sub>0.786× · 3.5%</sub> |
| array_exchange | 40,000 | **1.16 ms**<br><sub>1.5%</sub> | **824 µs**<br><sub>0.713× · 3.2%</sub> |
| register_functions | 10 | **2.95 µs**<br><sub>6.8%</sub> | **800 ns**<br><sub>0.271× · 0.0%</sub> |
| register_functions | 100 | **20.2 µs**<br><sub>7.2%</sub> | **8.95 µs**<br><sub>0.443× · 3.4%</sub> |
| register_functions | 1,000 | **227 µs**<br><sub>2.2%</sub> | **103 µs**<br><sub>0.452× · 4.2%</sub> |
| register_variables | 10 | **4.95 µs**<br><sub>4.0%</sub> | **800 ns**<br><sub>0.162× · 6.2%</sub> |
| register_variables | 100 | **36.2 µs**<br><sub>2.6%</sub> | **8.6 µs**<br><sub>0.238× · 4.1%</sub> |
| register_variables | 1,000 | **381 µs**<br><sub>5.8%</sub> | **104 µs**<br><sub>0.272× · 17.3%</sub> |
| call_by_name | 10 | **11.8 ms**<br><sub>6.3%</sub> | **3.64 ms**<br><sub>0.307× · 9.3%</sub> |
| call_by_name | 1,000 | **13.4 ms**<br><sub>2.1%</sub> | **5.56 ms**<br><sub>0.415× · 2.2%</sub> |
| call_by_name | 10,000 | **15.2 ms**<br><sub>4.0%</sub> | **8.03 ms**<br><sub>0.529× · 2.0%</sub> |
| call_by_handle | 10 | **4.85 ms**<br><sub>1.9%</sub> | **2.88 ms**<br><sub>0.594× · 4.8%</sub> |
| call_by_handle | 1,000 | **5.02 ms**<br><sub>2.1%</sub> | **3.09 ms**<br><sub>0.615× · 3.5%</sub> |
| call_by_handle | 10,000 | **5.3 ms**<br><sub>2.2%</sub> | **3.8 ms**<br><sub>0.717× · 2.6%</sub> |

### Python

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **48.4 ms**<br><sub>1.2%</sub> | **8.41 ms**<br><sub>0.174× · 2.0%</sub> |
| host_to_script | 100,000 | **19.7 ms**<br><sub>5.7%</sub> | **15.6 ms**<br><sub>0.79× · 1.8%</sub> |
| cpp_objects | 150,000 | **260 ms**<br><sub>1.3%</sub> | **31.5 ms**<br><sub>0.121× · 2.7%</sub> |
| buffer | 400 | **18.6 ms**<br><sub>2.6%</sub> | **18.5 ms**<br><sub>0.997× · 5.4%</sub> |
| string_exchange | 250,000 | **20.7 ms**<br><sub>1.6%</sub> | **17 ms**<br><sub>0.822× · 8.0%</sub> |
| array_exchange | 10 | **2.8 ms**<br><sub>9.1%</sub> | **2.1 ms**<br><sub>0.752× · 0.8%</sub> |
| array_exchange | 1,000 | **1.62 ms**<br><sub>2.1%</sub> | **1.35 ms**<br><sub>0.832× · 3.8%</sub> |
| array_exchange | 40,000 | **1.86 ms**<br><sub>4.1%</sub> | **1.54 ms**<br><sub>0.828× · 8.3%</sub> |
| register_functions | 10 | **36 µs**<br><sub>4.7%</sub> | **2.5 µs**<br><sub>0.0694× · 8.0%</sub> |
| register_functions | 100 | **256 µs**<br><sub>1.8%</sub> | **21.8 µs**<br><sub>0.0851× · 3.2%</sub> |
| register_functions | 1,000 | **2.37 ms**<br><sub>1.9%</sub> | **227 µs**<br><sub>0.0957× · 3.9%</sub> |
| register_variables | 10 | **2.6 µs**<br><sub>13.5%</sub> | **1.5 µs**<br><sub>0.577× · 10.0%</sub> |
| register_variables | 100 | **18.3 µs**<br><sub>6.0%</sub> | **12.9 µs**<br><sub>0.703× · 1.6%</sub> |
| register_variables | 1,000 | **194 µs**<br><sub>6.5%</sub> | **155 µs**<br><sub>0.797× · 4.1%</sub> |
| call_by_name | 10 | **10.3 ms**<br><sub>12.5%</sub> | **9.12 ms**<br><sub>0.887× · 11.2%</sub> |
| call_by_name | 1,000 | **11.6 ms**<br><sub>2.2%</sub> | **10.4 ms**<br><sub>0.903× · 4.5%</sub> |
| call_by_name | 10,000 | **12.9 ms**<br><sub>3.4%</sub> | **11.9 ms**<br><sub>0.925× · 9.2%</sub> |
| call_by_handle | 10 | **4.93 ms**<br><sub>1.2%</sub> | **4.02 ms**<br><sub>0.815× · 0.9%</sub> |
| call_by_handle | 1,000 | **5.48 ms**<br><sub>4.1%</sub> | **4.73 ms**<br><sub>0.863× · 15.9%</sub> |
| call_by_handle | 10,000 | **5.54 ms**<br><sub>2.7%</sub> | **4.89 ms**<br><sub>0.882× · 10.9%</sub> |

### TeaScript

| Benchmark | n | vm-O1 (headline) | ast | vm-O0 | vm-O2 |
|---|--:|--:|--:|--:|--:|
| fib_recursive | 15 | **1.16 ms**<br><sub>0.8%</sub> | **1.08 ms**<br><sub>0.934× · 1.1%</sub> | **1.16 ms**<br><sub>1× · 0.9%</sub> | **1.09 ms**<br><sub>0.945× · 0.6%</sub> |
| fib_recursive | 20 | **12.8 ms**<br><sub>0.8%</sub> | **12 ms**<br><sub>0.941× · 1.1%</sub> | **12.8 ms**<br><sub>1.01× · 2.5%</sub> | **12.1 ms**<br><sub>0.947× · 0.8%</sub> |
| fib_recursive | 25 | **142 ms**<br><sub>1.1%</sub> | **137 ms**<br><sub>0.969× · 0.2%</sub> | **142 ms**<br><sub>1× · 1.3%</sub> | **134 ms**<br><sub>0.948× · 1.3%</sub> |
| loop | 1,000,000 | **242 ms**<br><sub>0.4%</sub> | **147 ms**<br><sub>0.607× · 0.3%</sub> | **242 ms**<br><sub>1× · 0.6%</sub> | **229 ms**<br><sub>0.945× · 0.8%</sub> |
| int_arith | 5,000 | **267 ms**<br><sub>1.1%</sub> | **192 ms**<br><sub>0.716× · 4.2%</sub> | **267 ms**<br><sub>1× · 0.5%</sub> | **258 ms**<br><sub>0.966× · 0.4%</sub> |
| mandelbrot | 100 | **265 ms**<br><sub>0.7%</sub> | **221 ms**<br><sub>0.833× · 0.9%</sub> | **265 ms**<br><sub>1× · 0.3%</sub> | **263 ms**<br><sub>0.993× · 0.9%</sub> |
| math_builtins | 100,000 | **194 ms**<br><sub>0.9%</sub> | **178 ms**<br><sub>0.917× · 0.8%</sub> | **193 ms**<br><sub>0.998× · 0.7%</sub> | **188 ms**<br><sub>0.972× · 0.8%</sub> |
| calls | 100,000 | **169 ms**<br><sub>1.4%</sub> | **139 ms**<br><sub>0.823× · 0.7%</sub> | **169 ms**<br><sub>1× · 3.3%</sub> | **162 ms**<br><sub>0.957× · 1.7%</sub> |
| tak | 4 | **1.38 ms**<br><sub>1.9%</sub> | **1.42 ms**<br><sub>1.03× · 1.8%</sub> | **1.38 ms**<br><sub>1× · 4.7%</sub> | **1.31 ms**<br><sub>0.946× · 1.8%</sub> |
| tak | 5 | **8.21 ms**<br><sub>1.7%</sub> | **7.08 ms**<br><sub>0.863× · 3.1%</sub> | **8.18 ms**<br><sub>0.997× · 0.7%</sub> | **7.88 ms**<br><sub>0.96× · 2.4%</sub> |
| tak | 6 | **55 ms**<br><sub>1.6%</sub> | **43 ms**<br><sub>0.782× · 1.9%</sub> | **55.8 ms**<br><sub>1.02× · 3.4%</sub> | **52.7 ms**<br><sub>0.959× · 3.7%</sub> |
| branching | 300,000 | **346 ms**<br><sub>1.3%</sub> | **224 ms**<br><sub>0.647× · 1.7%</sub> | **345 ms**<br><sub>0.997× · 0.6%</sub> | **325 ms**<br><sub>0.939× · 0.3%</sub> |
| var_global | 1,000,000 | **242 ms**<br><sub>0.2%</sub> | **147 ms**<br><sub>0.608× · 1.6%</sub> | **242 ms**<br><sub>0.999× · 0.4%</sub> | **229 ms**<br><sub>0.944× · 0.4%</sub> |
| var_nested | 25 | **187 ms**<br><sub>0.5%</sub> | **112 ms**<br><sub>0.598× · 0.3%</sub> | **187 ms**<br><sub>1× · 1.6%</sub> | **182 ms**<br><sub>0.971× · 1.4%</sub> |
| var_lifecycle | 300,000 | **240 ms**<br><sub>1.3%</sub> | **147 ms**<br><sub>0.613× · 0.6%</sub> | **243 ms**<br><sub>1.01× · 2.5%</sub> | **241 ms**<br><sub>1× · 3.3%</sub> |
| sieve | 40,000 | **36.8 ms**<br><sub>1.4%</sub> | **45 ms**<br><sub>1.22× · 0.7%</sub> | **36.9 ms**<br><sub>1× · 1.1%</sub> | **34.3 ms**<br><sub>0.932× · 1.9%</sub> |
| sort | 1,000 | **224 ms**<br><sub>3.2%</sub> | **232 ms**<br><sub>1.04× · 0.9%</sub> | **227 ms**<br><sub>1.01× · 3.6%</sub> | **224 ms**<br><sub>1× · 1.9%</sub> |
| byte_buffer | 100,000 | **110 ms**<br><sub>0.6%</sub> | **98.8 ms**<br><sub>0.895× · 0.6%</sub> | **110 ms**<br><sub>1× · 0.9%</sub> | **107 ms**<br><sub>0.97× · 0.4%</sub> |
| maps | 50,000 | **165 ms**<br><sub>3.9%</sub> | **180 ms**<br><sub>1.09× · 0.4%</sub> | **165 ms**<br><sub>0.999× · 0.3%</sub> | **160 ms**<br><sub>0.972× · 1.0%</sub> |
| strings | 70,000 | **119 ms**<br><sub>2.1%</sub> | **89.9 ms**<br><sub>0.757× · 1.2%</sub> | **120 ms**<br><sub>1.01× · 2.2%</sub> | **117 ms**<br><sub>0.985× · 2.9%</sub> |
| records | 150,000 | **158 ms**<br><sub>0.7%</sub> | **107 ms**<br><sub>0.675× · 2.9%</sub> | **158 ms**<br><sub>0.999× · 0.6%</sub> | **155 ms**<br><sub>0.982× · 0.5%</sub> |
| binary_trees | 4 | **271 ms**<br><sub>0.5%</sub> | **262 ms**<br><sub>0.965× · 1.1%</sub> | **272 ms**<br><sub>1× · 1.0%</sub> | **265 ms**<br><sub>0.975× · 1.0%</sub> |
| binary_trees | 8 | **277 ms**<br><sub>0.6%</sub> | **262 ms**<br><sub>0.946× · 0.7%</sub> | **276 ms**<br><sub>0.997× · 0.9%</sub> | **268 ms**<br><sub>0.968× · 0.5%</sub> |
| binary_trees | 12 | **301 ms**<br><sub>0.6%</sub> | **286 ms**<br><sub>0.95× · 0.1%</sub> | **301 ms**<br><sub>0.998× · 0.6%</sub> | **294 ms**<br><sub>0.976× · 0.2%</sub> |
| script_to_host | 100,000 | **94.1 ms**<br><sub>0.5%</sub> | **81.8 ms**<br><sub>0.87× · 1.0%</sub> | **94 ms**<br><sub>0.999× · 0.7%</sub> | **92.9 ms**<br><sub>0.988× · 1.6%</sub> |
| host_to_script | 100,000 | **309 ms**<br><sub>0.6%</sub> | **270 ms**<br><sub>0.874× · 0.4%</sub> | **311 ms**<br><sub>1.01× · 0.6%</sub> | **305 ms**<br><sub>0.988× · 0.5%</sub> |
| buffer | 400 | **153 ms**<br><sub>1.1%</sub> | **114 ms**<br><sub>0.742× · 2.3%</sub> | **153 ms**<br><sub>1× · 1.1%</sub> | **151 ms**<br><sub>0.986× · 0.8%</sub> |
| string_exchange | 250,000 | **157 ms**<br><sub>1.8%</sub> | **73.6 ms**<br><sub>0.469× · 1.5%</sub> | **157 ms**<br><sub>1× · 0.9%</sub> | **152 ms**<br><sub>0.972× · 1.6%</sub> |
| array_exchange | 10 | **37.2 ms**<br><sub>0.6%</sub> | **35.9 ms**<br><sub>0.965× · 1.6%</sub> | **37.1 ms**<br><sub>0.996× · 1.1%</sub> | **36.5 ms**<br><sub>0.979× · 1.3%</sub> |
| array_exchange | 1,000 | **28.3 ms**<br><sub>0.8%</sub> | **29.9 ms**<br><sub>1.06× · 1.6%</sub> | **28.6 ms**<br><sub>1.01× · 0.9%</sub> | **27.3 ms**<br><sub>0.963× · 2.0%</sub> |
| array_exchange | 40,000 | **28.6 ms**<br><sub>0.3%</sub> | **30.7 ms**<br><sub>1.07× · 0.5%</sub> | **28.7 ms**<br><sub>1× · 0.4%</sub> | **27.9 ms**<br><sub>0.975× · 1.5%</sub> |
| engine_lifecycle | 10 | **12.6 ms**<br><sub>2.4%</sub> | **8.27 ms**<br><sub>0.655× · 1.3%</sub> | **14 ms**<br><sub>1.11× · 2.1%</sub> | **11.9 ms**<br><sub>0.942× · 1.5%</sub> |
| engine_minimal | 10 | **163 µs**<br><sub>0.8%</sub> | **157 µs**<br><sub>0.96× · 7.3%</sub> | **157 µs**<br><sub>0.964× · 4.3%</sub> | **157 µs**<br><sub>0.961× · 3.2%</sub> |
| parse_compile | 100 | **3.11 ms**<br><sub>5.3%</sub> | **1.78 ms**<br><sub>0.572× · 1.4%</sub> | **3.48 ms**<br><sub>1.12× · 3.0%</sub> | **2.86 ms**<br><sub>0.921× · 1.4%</sub> |
| parse_compile | 1,000 | **27.6 ms**<br><sub>1.1%</sub> | **17.3 ms**<br><sub>0.626× · 0.7%</sub> | **31 ms**<br><sub>1.12× · 0.6%</sub> | **27.7 ms**<br><sub>1.01× · 0.6%</sub> |
| parse_compile | 10,000 | **279 ms**<br><sub>0.7%</sub> | **171 ms**<br><sub>0.613× · 0.5%</sub> | **320 ms**<br><sub>1.14× · 1.2%</sub> | **280 ms**<br><sub>1× · 0.6%</sub> |
| register_functions | 10 | **3.25 µs**<br><sub>21.5%</sub> | **3.05 µs**<br><sub>0.938× · 41.0%</sub> | **4.2 µs**<br><sub>1.29× · 8.3%</sub> | **3.3 µs**<br><sub>1.02× · 6.1%</sub> |
| register_functions | 100 | **41.8 µs**<br><sub>5.9%</sub> | **42.5 µs**<br><sub>1.02× · 2.9%</sub> | **39.9 µs**<br><sub>0.955× · 4.5%</sub> | **42.1 µs**<br><sub>1.01× · 3.9%</sub> |
| register_functions | 1,000 | **299 µs**<br><sub>4.3%</sub> | **311 µs**<br><sub>1.04× · 6.2%</sub> | **298 µs**<br><sub>0.999× · 3.4%</sub> | **294 µs**<br><sub>0.984× · 4.6%</sub> |
| register_variables | 10 | **2.3 µs**<br><sub>26.1%</sub> | **2.4 µs**<br><sub>1.04× · 8.3%</sub> | **3.4 µs**<br><sub>1.48× · 7.4%</sub> | **2.55 µs**<br><sub>1.11× · 3.9%</sub> |
| register_variables | 100 | **31.5 µs**<br><sub>6.8%</sub> | **31.4 µs**<br><sub>0.995× · 3.8%</sub> | **30.3 µs**<br><sub>0.962× · 5.8%</sub> | **31.3 µs**<br><sub>0.992× · 3.8%</sub> |
| register_variables | 1,000 | **221 µs**<br><sub>5.8%</sub> | **229 µs**<br><sub>1.04× · 3.7%</sub> | **221 µs**<br><sub>1× · 3.7%</sub> | **220 µs**<br><sub>0.996× · 2.3%</sub> |
| call_by_name | 10 | **68.3 ms**<br><sub>1.4%</sub> | **32.2 ms**<br><sub>0.472× · 1.1%</sub> | **68.2 ms**<br><sub>0.999× · 1.9%</sub> | **66.7 ms**<br><sub>0.978× · 1.0%</sub> |
| call_by_name | 1,000 | **69.3 ms**<br><sub>1.2%</sub> | **35.9 ms**<br><sub>0.518× · 0.6%</sub> | **69.5 ms**<br><sub>1× · 3.3%</sub> | **67.9 ms**<br><sub>0.98× · 2.6%</sub> |
| call_by_name | 10,000 | **77 ms**<br><sub>1.7%</sub> | **49.6 ms**<br><sub>0.644× · 1.4%</sub> | **76.3 ms**<br><sub>0.991× · 1.2%</sub> | **74.6 ms**<br><sub>0.969× · 0.9%</sub> |
| engine_reset | 100 | **1.26 ms**<br><sub>2.5%</sub> | **791 µs**<br><sub>0.627× · 1.5%</sub> | **1.37 ms**<br><sub>1.08× · 6.0%</sub> | **1.18 ms**<br><sub>0.937× · 2.8%</sub> |

## Peak memory

The peak memory of each process, the median over the processes of a run: the peak working set on Windows, the peak resident set on Linux. It covers the whole process: the runtime and libraries of the language, the unmeasured warm-up engine, the engine of the run and its workload. Below it: the difference to the process of the C++ runner for the same benchmark and n, which is the harness alone where C++ is "n/a" (the host API benchmarks). Headline variants.

### Core

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| fib_recursive | 15 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.4 MiB**<br><sub>+2.2 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| fib_recursive | 20 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **10.5 MiB**<br><sub>+6.3 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| fib_recursive | 25 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **51.2 MiB**<br><sub>+47.0 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| loop | 1,000,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| int_arith | 5,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| mandelbrot | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| math_builtins | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| calls | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| tak | 4 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.3 MiB**<br><sub>+2.2 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| tak | 5 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **8.2 MiB**<br><sub>+4.0 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| tak | 6 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **17.2 MiB**<br><sub>+13.0 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| branching | 300,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |

### Data

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| var_global | 1,000,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| var_nested | 25 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| var_lifecycle | 300,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| sieve | 40,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **13.5 MiB**<br><sub>+9.4 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **10.4 MiB**<br><sub>+6.2 MiB</sub> | **15.1 MiB**<br><sub>+10.9 MiB</sub> |
| sort | 1,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.3 MiB**<br><sub>+2.1 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| byte_buffer | 100,000 | 4.3 MiB | **5.2 MiB**<br><sub>+0.9 MiB</sub> | **24.8 MiB**<br><sub>+20.5 MiB</sub> | **5.2 MiB**<br><sub>+0.9 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| maps | 50,000 | 9.9 MiB | **13.1 MiB**<br><sub>+3.2 MiB</sub> | **30.3 MiB**<br><sub>+20.3 MiB</sub> | **10.2 MiB**<br><sub>+0.2 MiB</sub> | **17.8 MiB**<br><sub>+7.9 MiB</sub> | **28.9 MiB**<br><sub>+19.0 MiB</sub> |
| strings | 70,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| records | 150,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.9 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| binary_trees | 4 | 4.2 MiB | **18.1 MiB**<br><sub>+13.9 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| binary_trees | 8 | 4.2 MiB | **26.0 MiB**<br><sub>+21.8 MiB</sub> | **6.9 MiB**<br><sub>+2.7 MiB</sub> | **4.7 MiB**<br><sub>+0.5 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| binary_trees | 12 | 4.2 MiB | **26.4 MiB**<br><sub>+22.1 MiB</sub> | **13.2 MiB**<br><sub>+8.9 MiB</sub> | **7.4 MiB**<br><sub>+3.2 MiB</sub> | **11.7 MiB**<br><sub>+7.5 MiB</sub> | **11.0 MiB**<br><sub>+6.7 MiB</sub> |

### Interop (script <-> C++)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| script_to_host | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+6.0 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| host_to_script | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.7 MiB**<br><sub>+1.5 MiB</sub> |
| cpp_objects | 150,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.7 MiB**<br><sub>+6.5 MiB</sub> | n/a |
| buffer | 400 | 4.8 MiB | **6.8 MiB**<br><sub>+2.0 MiB</sub> | **7.3 MiB**<br><sub>+2.5 MiB</sub> | **5.7 MiB**<br><sub>+0.9 MiB</sub> | **11.9 MiB**<br><sub>+7.1 MiB</sub> | **6.5 MiB**<br><sub>+1.7 MiB</sub> |
| string_exchange | 250,000 | 4.2 MiB | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **9.8 MiB**<br><sub>+5.6 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| array_exchange | 10 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.3 MiB**<br><sub>+2.1 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.9 MiB**<br><sub>+1.8 MiB</sub> |
| array_exchange | 1,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.6 MiB**<br><sub>+2.4 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> |
| array_exchange | 40,000 | 4.3 MiB | **5.7 MiB**<br><sub>+1.4 MiB</sub> | **21.4 MiB**<br><sub>+17.2 MiB</sub> | **5.4 MiB**<br><sub>+1.2 MiB</sub> | **11.4 MiB**<br><sub>+7.1 MiB</sub> | **24.2 MiB**<br><sub>+20.0 MiB</sub> |

### Host API (C++ level)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| engine_lifecycle | 10 | 4.2 MiB | **4.8 MiB**<br><sub>+0.6 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **10.6 MiB**<br><sub>+6.4 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> |
| engine_minimal | 10 | 4.2 MiB | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **4.3 MiB**<br><sub>+0.1 MiB</sub> | n/a | **4.6 MiB**<br><sub>+0.4 MiB</sub> |
| parse_compile | 100 | 4.2 MiB | **5.7 MiB**<br><sub>+1.5 MiB</sub> | **7.0 MiB**<br><sub>+2.9 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **13.6 MiB**<br><sub>+9.4 MiB</sub> | **8.6 MiB**<br><sub>+4.4 MiB</sub> |
| parse_compile | 1,000 | 4.2 MiB | **8.9 MiB**<br><sub>+4.7 MiB</sub> | **14.6 MiB**<br><sub>+10.5 MiB</sub> | **5.7 MiB**<br><sub>+1.6 MiB</sub> | **37.0 MiB**<br><sub>+32.8 MiB</sub> | **31.2 MiB**<br><sub>+27.0 MiB</sub> |
| parse_compile | 10,000 | 4.2 MiB | **41.6 MiB**<br><sub>+37.4 MiB</sub> | **92.4 MiB**<br><sub>+88.3 MiB</sub> | **15.5 MiB**<br><sub>+11.3 MiB</sub> | **269.9 MiB**<br><sub>+265.7 MiB</sub> | **263.3 MiB**<br><sub>+259.2 MiB</sub> |
| register_functions | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.1 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **11.6 MiB**<br><sub>+7.4 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_functions | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.2 MiB**<br><sub>+2.1 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **11.5 MiB**<br><sub>+7.3 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_functions | 1,000 | 4.2 MiB | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **7.2 MiB**<br><sub>+3.1 MiB</sub> | **4.8 MiB**<br><sub>+0.6 MiB</sub> | **12.0 MiB**<br><sub>+7.8 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_variables | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.1 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **11.5 MiB**<br><sub>+7.3 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_variables | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.1 MiB**<br><sub>+2.0 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **11.5 MiB**<br><sub>+7.3 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_variables | 1,000 | 4.2 MiB | **5.4 MiB**<br><sub>+1.2 MiB</sub> | **6.2 MiB**<br><sub>+2.1 MiB</sub> | **4.9 MiB**<br><sub>+0.7 MiB</sub> | **11.7 MiB**<br><sub>+7.5 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> |
| call_by_name | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.9 MiB</sub> | **5.7 MiB**<br><sub>+1.5 MiB</sub> |
| call_by_name | 1,000 | 4.2 MiB | **7.3 MiB**<br><sub>+3.1 MiB</sub> | **8.3 MiB**<br><sub>+4.1 MiB</sub> | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **17.5 MiB**<br><sub>+13.3 MiB</sub> | **11.7 MiB**<br><sub>+7.5 MiB</sub> |
| call_by_name | 10,000 | 4.2 MiB | **28.8 MiB**<br><sub>+24.6 MiB</sub> | **29.9 MiB**<br><sub>+25.8 MiB</sub> | **10.9 MiB**<br><sub>+6.8 MiB</sub> | **82.2 MiB**<br><sub>+78.1 MiB</sub> | **55.4 MiB**<br><sub>+51.2 MiB</sub> |
| call_by_handle | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | n/a |
| call_by_handle | 1,000 | 4.2 MiB | **7.3 MiB**<br><sub>+3.1 MiB</sub> | **8.4 MiB**<br><sub>+4.2 MiB</sub> | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **17.6 MiB**<br><sub>+13.4 MiB</sub> | n/a |
| call_by_handle | 10,000 | 4.2 MiB | **28.8 MiB**<br><sub>+24.6 MiB</sub> | **30.9 MiB**<br><sub>+26.7 MiB</sub> | **11.4 MiB**<br><sub>+7.2 MiB</sub> | **82.3 MiB**<br><sub>+78.2 MiB</sub> | n/a |
| engine_reset | 100 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.2 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **10.5 MiB**<br><sub>+6.3 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> |

---
Generated by `runner/report.py` from [results.json](results.json).
