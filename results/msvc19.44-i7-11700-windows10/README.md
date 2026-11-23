# Result set msvc19.44-i7-11700-windows10

> **Preview:** at least one language is at a pre-release version (TeaScript before 0.18.0). The set will be measured again for the release.

| | |
|---|---|
| CPU | 11th Gen Intel(R) Core(TM) i7-11700 @ 2.50GHz, 8 cores, 16 threads |
| Memory | 31.8 GiB |
| OS | Windows 10 Pro 22H2 (Windows 10.0.19045.7846) |
| Virtualization | Hyper-V is enabled: this OS runs in its root partition |
| Compiler | MSVC 19.44.35229, Release, `/DWIN32 /D_WINDOWS /EHsc /O2 /Ob2 /DNDEBUG /arch:AVX2` |
| cpp-script-bench | 0.1.0 (19b31d8) |
| AngelScript | 2.38.0 (AS_64BIT_PTR AS_NO_THREADS AS_WIN AS_X64_MSVC) |
| ChaiScript | 7.0.0 (wasm-latest-36-gf635b51e); ChaiScript_Extras (v1.0.1-13-gdd116d7) |
| Lua | 5.5.1; LuaBridge3 (3.0-rc12) |
| Python | 3.14.10 (tags/v3.14.10:b703b2c, Oct 25 2026, 17:36:52) [MSC v.1944 64 bit (AMD64)]; pybind11 3.1.0 (v3.1.0) |
| TeaScript | 0.18.0 (v0.17.0-45-g63f3c06) |
| Measured | 2026-11-22 15:32 to 2026-11-22 17:04 UTC |
| Processes | 5 per run, interleaved across the languages; pinned to logical CPU 15, priority high; power: Balanced (381b4222-f694-41f0-9685-ff5bb260df2e) |
| Per process | 3 warm-up and 10 measured calls, time budget 30 s (at least 3 calls) |
| Machine load | other processes used 0.00 logical CPUs in the median, at most 3.47 (processes of at least 1 s) |
| Status | preview |

All numbers compare only within this set. **How to read:** each cell is the median time of one call of `bench( n )` (host API benchmarks: of one sample) over all processes: the median of the medians of the processes. Below it: the factor to C++, and the spread between the processes (slowest minus fastest process median, relative to the median); "budget": the time budget limited the number of calls. The headline variant of each language is shown; the other variants follow below. The rules are in [PLAN.md](../../PLAN.md#2-fairness-rules), the raw data in [results.json](results.json).

## Core

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/core-dark.svg">
  <img alt="Core: median time per call of each language, log scale" src="charts/core-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 15 | **1.9 µs**<br><sub>5.3%</sub> | **39.1 µs**<br><sub>20.6× · 9.0%</sub> | **2.27 ms**<br><sub>1,196× · 0.6%</sub> | **31.4 µs**<br><sub>16.6× · 4.1%</sub> | **65.4 µs**<br><sub>34.4× · 12.2%</sub> | **1.11 ms**<br><sub>586× · 0.7%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 20 | **21.1 µs**<br><sub>3.1%</sub> | **428 µs**<br><sub>20.3× · 8.6%</sub> | **149 ms**<br><sub>7,047× · 0.2%</sub> | **339 µs**<br><sub>16× · 3.0%</sub> | **717 µs**<br><sub>33.9× · 2.2%</sub> | **12.5 ms**<br><sub>589× · 1.6%</sub> |
| [fib_recursive](../../benchmarks/fib_recursive/README.md) | 25 | **234 µs**<br><sub>2.1%</sub> | **4.69 ms**<br><sub>20× · 5.5%</sub> | **18.1 s**<br><sub>77,316× · 0.1% · budget</sub> | **3.75 ms**<br><sub>16× · 1.2%</sub> | **7.9 ms**<br><sub>33.7× · 1.2%</sub> | **139 ms**<br><sub>593× · 4.1%</sub> |
| [loop](../../benchmarks/loop/README.md) | 1,000,000 | **250 µs**<br><sub>0.0%</sub> | **8.36 ms**<br><sub>33.4× · 0.0%</sub> | **448 ms**<br><sub>1,794× · 0.8%</sub> | **4.44 ms**<br><sub>17.8× · 4.5%</sub> | **27.1 ms**<br><sub>109× · 6.2%</sub> | **220 ms**<br><sub>881× · 0.4%</sub> |
| [int_arith](../../benchmarks/int_arith/README.md) | 5,000 | **547 µs**<br><sub>2.4%</sub> | **7.23 ms**<br><sub>13.2× · 0.2%</sub> | **278 ms**<br><sub>508× · 1.7%</sub> | **5.85 ms**<br><sub>10.7× · 0.4%</sub> | **22.4 ms**<br><sub>41× · 8.1%</sub> | **247 ms**<br><sub>452× · 0.7%</sub> |
| [mandelbrot](../../benchmarks/mandelbrot/README.md) | 100 | **396 µs**<br><sub>0.1%</sub> | **5.72 ms**<br><sub>14.5× · 0.8%</sub> | **404 ms**<br><sub>1,021× · 2.8%</sub> | **4.85 ms**<br><sub>12.3× · 0.1%</sub> | **20.8 ms**<br><sub>52.7× · 3.8%</sub> | **240 ms**<br><sub>608× · 0.7%</sub> |
| [math_builtins](../../benchmarks/math_builtins/README.md) | 100,000 | **126 µs**<br><sub>10.9%</sub> | **5.21 ms**<br><sub>41.5× · 11.1%</sub> | **141 ms**<br><sub>1,127× · 4.0%</sub> | **3.4 ms**<br><sub>27.1× · 85.4%</sub> | **8.02 ms**<br><sub>63.9× · 50.2%</sub> | **180 ms**<br><sub>1,431× · 3.6%</sub> |
| [calls](../../benchmarks/calls/README.md) | 100,000 | **54.5 µs**<br><sub>4.4%</sub> | **5.03 ms**<br><sub>92.3× · 2.9%</sub> | **215 ms**<br><sub>3,948× · 5.8%</sub> | **4.95 ms**<br><sub>90.8× · 7.7%</sub> | **12 ms**<br><sub>220× · 8.9%</sub> | **164 ms**<br><sub>3,012× · 2.8%</sub> |
| [tak](../../benchmarks/tak/README.md) | 4 | **1.6 µs**<br><sub>6.3%</sub> | **39.7 µs**<br><sub>24.8× · 8.3%</sub> | **3.62 ms**<br><sub>2,262× · 4.8%</sub> | **29.1 µs**<br><sub>18.2× · 5.7%</sub> | **57.1 µs**<br><sub>35.7× · 6.1%</sub> | **1.33 ms**<br><sub>830× · 7.4%</sub> |
| [tak](../../benchmarks/tak/README.md) | 5 | **9.4 µs**<br><sub>9.6%</sub> | **233 µs**<br><sub>24.8× · 2.9%</sub> | **95.1 ms**<br><sub>10,113× · 7.5%</sub> | **171 µs**<br><sub>18.2× · 9.7%</sub> | **342 µs**<br><sub>36.4× · 14.1%</sub> | **8.86 ms**<br><sub>943× · 3.0%</sub> |
| [tak](../../benchmarks/tak/README.md) | 6 | **57.7 µs**<br><sub>4.2%</sub> | **1.43 ms**<br><sub>24.8× · 3.8%</sub> | **3.64 s**<br><sub>63,010× · 5.1% · budget</sub> | **1.03 ms**<br><sub>17.8× · 4.3%</sub> | **2.03 ms**<br><sub>35.1× · 2.6%</sub> | **52.1 ms**<br><sub>903× · 5.2%</sub> |
| [branching](../../benchmarks/branching/README.md) | 300,000 | **493 µs**<br><sub>8.6%</sub> | **9.63 ms**<br><sub>19.5× · 4.4%</sub> | **322 ms**<br><sub>652× · 5.4%</sub> | **4.82 ms**<br><sub>9.78× · 4.4%</sub> | **23.3 ms**<br><sub>47.3× · 6.2%</sub> | **321 ms**<br><sub>650× · 2.9%</sub> |

## Data

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/data-dark.svg">
  <img alt="Data: median time per call of each language, log scale" src="charts/data-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [var_global](../../benchmarks/var_global/README.md) | 1,000,000 | **333 µs**<br><sub>4.4%</sub> | **10.9 ms**<br><sub>32.7× · 3.6%</sub> | **513 ms**<br><sub>1,542× · 3.4%</sub> | **11.1 ms**<br><sub>33.5× · 18.3%</sub> | **39.2 ms**<br><sub>118× · 7.0%</sub> | **220 ms**<br><sub>662× · 2.8%</sub> |
| [var_nested](../../benchmarks/var_nested/README.md) | 25 | **96.3 µs**<br><sub>8.7%</sub> | **5.02 ms**<br><sub>52.2× · 3.5%</sub> | **361 ms**<br><sub>3,750× · 3.0%</sub> | **3.77 ms**<br><sub>39.2× · 0.8%</sub> | **15.2 ms**<br><sub>158× · 5.9%</sub> | **171 ms**<br><sub>1,772× · 2.3%</sub> |
| [var_lifecycle](../../benchmarks/var_lifecycle/README.md) | 300,000 | **58.1 µs**<br><sub>4.3%</sub> | **3.19 ms**<br><sub>55× · 0.2%</sub> | **371 ms**<br><sub>6,390× · 2.4%</sub> | **2.11 ms**<br><sub>36.4× · 5.0%</sub> | **13.9 ms**<br><sub>240× · 10.8%</sub> | **220 ms**<br><sub>3,787× · 1.9%</sub> |
| [sieve](../../benchmarks/sieve/README.md) | 40,000 | **237 µs**<br><sub>2.7%</sub> | **3.16 ms**<br><sub>13.4× · 2.7%</sub> | **485 ms**<br><sub>2,051× · 4.0%</sub> | **781 µs**<br><sub>3.3× · 2.4%</sub> | **2.91 ms**<br><sub>12.3× · 7.3%</sub> | **34.9 ms**<br><sub>147× · 11.6%</sub> |
| [sort](../../benchmarks/sort/README.md) | 1,000 | **104 µs**<br><sub>15.2%</sub> | **10 ms**<br><sub>96.7× · 3.2%</sub> | **422 ms**<br><sub>4,065× · 4.4%</sub> | **3.83 ms**<br><sub>36.9× · 3.5%</sub> | **12.4 ms**<br><sub>119× · 5.9%</sub> | **206 ms**<br><sub>1,987× · 4.2%</sub> |
| [byte_buffer](../../benchmarks/byte_buffer/README.md) | 100,000 | **636 µs**<br><sub>2.1%</sub> | **4.41 ms**<br><sub>6.93× · 2.1%</sub> | **494 ms**<br><sub>777× · 4.4%</sub> | **2.27 ms**<br><sub>3.58× · 3.1%</sub> | **14.1 ms**<br><sub>22.2× · 4.1%</sub> | **102 ms**<br><sub>160× · 3.2%</sub> |
| [maps](../../benchmarks/maps/README.md) | 50,000 | **7.8 ms**<br><sub>1.9%</sub> | **59.7 ms**<br><sub>7.65× · 1.8%</sub> | **402 ms**<br><sub>51.6× · 5.7%</sub> | **15.6 ms**<br><sub>2.01× · 3.2%</sub> | **15.3 ms**<br><sub>1.96× · 1.6%</sub> | **160 ms**<br><sub>20.5× · 2.9%</sub> |
| [strings](../../benchmarks/strings/README.md) | 70,000 | **3.18 ms**<br><sub>9.0%</sub> | **57.5 ms**<br><sub>18.1× · 12.6%</sub> | **444 ms**<br><sub>139× · 5.3%</sub> | **20.9 ms**<br><sub>6.56× · 9.4%</sub> | **16.7 ms**<br><sub>5.24× · 7.7%</sub> | **111 ms**<br><sub>34.7× · 9.9%</sub> |
| [records](../../benchmarks/records/README.md) | 150,000 | **84.8 µs**<br><sub>4.5%</sub> | **5.85 ms**<br><sub>69× · 7.1%</sub> | **436 ms**<br><sub>5,144× · 3.3%</sub> | **5.21 ms**<br><sub>61.5× · 2.4%</sub> | **13.6 ms**<br><sub>161× · 7.0%</sub> | **155 ms**<br><sub>1,827× · 1.1%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 4 | **4.41 ms**<br><sub>9.4%</sub> | **48.6 ms**<br><sub>11× · 2.2%</sub> | **541 ms**<br><sub>123× · 0.6%</sub> | **22.3 ms**<br><sub>5.05× · 2.4%</sub> | **19.2 ms**<br><sub>4.35× · 5.2%</sub> | **271 ms**<br><sub>61.3× · 1.1%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 8 | **6.63 ms**<br><sub>2.1%</sub> | **57.9 ms**<br><sub>8.73× · 4.0%</sub> | **937 ms**<br><sub>141× · 1.4%</sub> | **22.4 ms**<br><sub>3.38× · 1.3%</sub> | **19.6 ms**<br><sub>2.95× · 1.5%</sub> | **271 ms**<br><sub>40.8× · 0.4%</sub> |
| [binary_trees](../../benchmarks/binary_trees/README.md) | 12 | **6.42 ms**<br><sub>4.1%</sub> | **52.7 ms**<br><sub>8.22× · 1.8%</sub> | **7.69 s**<br><sub>1,199× · 0.4% · budget</sub> | **26.5 ms**<br><sub>4.14× · 1.2%</sub> | **27.1 ms**<br><sub>4.22× · 0.7%</sub> | **296 ms**<br><sub>46.2× · 0.6%</sub> |

## Interop (script <-> C++)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/interop-dark.svg">
  <img alt="Interop (script &lt;-&gt; C++): median time per call of each language, log scale" src="charts/interop-light.svg">
</picture>

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| [script_to_host](../../benchmarks/script_to_host/README.md) | 100,000 | **61.1 µs**<br><sub>2.1%</sub> | **7.37 ms**<br><sub>121× · 1.1%</sub> | **169 ms**<br><sub>2,774× · 4.8%</sub> | **10 ms**<br><sub>164× · 2.1%</sub> | **48.2 ms**<br><sub>789× · 1.4%</sub> | **90.7 ms**<br><sub>1,486× · 1.2%</sub> |
| [host_to_script](../../benchmarks/host_to_script/README.md) | 100,000 | **61.1 µs**<br><sub>0.2%</sub> | **17.7 ms**<br><sub>289× · 1.7%</sub> | **187 ms**<br><sub>3,056× · 6.3%</sub> | **16.1 ms**<br><sub>264× · 1.9%</sub> | **20.2 ms**<br><sub>331× · 3.1%</sub> | **225 ms**<br><sub>3,678× · 0.7%</sub> |
| [cpp_objects](../../benchmarks/cpp_objects/README.md) | 150,000 | **87.5 µs**<br><sub>2.2%</sub> | **4.98 ms**<br><sub>57× · 0.7%</sub> | **496 ms**<br><sub>5,675× · 3.5%</sub> | **107 ms**<br><sub>1,229× · 2.1%</sub> | **258 ms**<br><sub>2,949× · 0.5%</sub> | n/a |
| [buffer](../../benchmarks/buffer/README.md) | 400 | **62.9 µs**<br><sub>0.6%</sub> | **4.98 ms**<br><sub>79.2× · 2.8%</sub> | **317 ms**<br><sub>5,033× · 1.8%</sub> | **20.9 ms**<br><sub>332× · 1.7%</sub> | **17.9 ms**<br><sub>285× · 2.8%</sub> | **148 ms**<br><sub>2,360× · 0.8%</sub> |
| [string_exchange](../../benchmarks/string_exchange/README.md) | 250,000 | **5.25 ms**<br><sub>1.8%</sub> | **21.4 ms**<br><sub>4.07× · 1.4%</sub> | **78 ms**<br><sub>14.8× · 2.9%</sub> | **36.6 ms**<br><sub>6.96× · 0.9%</sub> | **20.4 ms**<br><sub>3.88× · 7.8%</sub> | **159 ms**<br><sub>30.2× · 0.7%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 10 | **190 µs**<br><sub>7.5%</sub> | **3.83 ms**<br><sub>20.2× · 1.2%</sub> | **400 ms**<br><sub>2,109× · 0.5%</sub> | **3.22 ms**<br><sub>16.9× · 4.8%</sub> | **2.76 ms**<br><sub>14.5× · 7.7%</sub> | **30.2 ms**<br><sub>159× · 14.9%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 1,000 | **33.2 µs**<br><sub>0.3%</sub> | **1.85 ms**<br><sub>55.8× · 0.7%</sub> | **358 ms**<br><sub>10,777× · 0.9%</sub> | **1.21 ms**<br><sub>36.5× · 3.9%</sub> | **1.6 ms**<br><sub>48.2× · 2.3%</sub> | **24.4 ms**<br><sub>735× · 0.6%</sub> |
| [array_exchange](../../benchmarks/array_exchange/README.md) | 40,000 | **53.6 µs**<br><sub>0.2%</sub> | **1.98 ms**<br><sub>37× · 2.2%</sub> | **360 ms**<br><sub>6,717× · 0.6%</sub> | **1.15 ms**<br><sub>21.4× · 1.6%</sub> | **1.8 ms**<br><sub>33.5× · 2.1%</sub> | **25.6 ms**<br><sub>478× · 0.2%</sub> |

## Host API (C++ level)

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="charts/host-dark.svg">
  <img alt="Host API (C++ level): median time per call of each language, log scale" src="charts/host-light.svg">
</picture>

| Benchmark | n | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|
| [engine_lifecycle](../../benchmarks/engine_lifecycle/README.md) | 10 | **4.99 ms**<br><sub>1.2%</sub> | **40.5 ms**<br><sub>0.9%</sub> | **288 µs**<br><sub>1.8%</sub> | **63.3 ms**<br><sub>0.7%</sub> | **12.8 ms**<br><sub>0.8%</sub> |
| [engine_minimal](../../benchmarks/engine_minimal/README.md) | 10 | **249 µs**<br><sub>1.6%</sub> | **230 µs**<br><sub>3.1%</sub> | **43.3 µs**<br><sub>6.2%</sub> | n/a | **163 µs**<br><sub>5.3%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 100 | **3.88 ms**<br><sub>2.4%</sub> | **4.91 ms**<br><sub>0.5%</sub> | **423 µs**<br><sub>4.6%</sub> | **3.99 ms**<br><sub>2.1%</sub> | **3.11 ms**<br><sub>4.5%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 1,000 | **38.5 ms**<br><sub>0.2%</sub> | **55.9 ms**<br><sub>0.5%</sub> | **4.22 ms**<br><sub>1.0%</sub> | **39.8 ms**<br><sub>1.2%</sub> | **28 ms**<br><sub>1.0%</sub> |
| [parse_compile](../../benchmarks/parse_compile/README.md) | 10,000 | **653 ms**<br><sub>0.3%</sub> | **1.33 s**<br><sub>0.3%</sub> | **40.7 ms**<br><sub>0.6%</sub> | **437 ms**<br><sub>0.2%</sub> | **284 ms**<br><sub>0.4%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 10 | **21.1 µs**<br><sub>21.8%</sub> | **48.6 µs**<br><sub>0.8%</sub> | **3 µs**<br><sub>3.3%</sub> | **34.9 µs**<br><sub>9.2%</sub> | **3.5 µs**<br><sub>12.9%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 100 | **185 µs**<br><sub>4.1%</sub> | **527 µs**<br><sub>1.5%</sub> | **20.1 µs**<br><sub>2.2%</sub> | **248 µs**<br><sub>3.0%</sub> | **42.5 µs**<br><sub>3.1%</sub> |
| [register_functions](../../benchmarks/register_functions/README.md) | 1,000 | **2.67 ms**<br><sub>1.1%</sub> | **12.8 ms**<br><sub>0.3%</sub> | **227 µs**<br><sub>1.3%</sub> | **2.3 ms**<br><sub>1.5%</sub> | **297 µs**<br><sub>4.2%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 10 | **12.9 µs**<br><sub>3.9%</sub> | **2.25 µs**<br><sub>4.4%</sub> | **4.95 µs**<br><sub>4.0%</sub> | **2.55 µs**<br><sub>15.7%</sub> | **2.4 µs**<br><sub>12.5%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 100 | **134 µs**<br><sub>2.3%</sub> | **21.2 µs**<br><sub>2.1%</sub> | **36 µs**<br><sub>1.2%</sub> | **17.9 µs**<br><sub>4.2%</sub> | **31.5 µs**<br><sub>1.9%</sub> |
| [register_variables](../../benchmarks/register_variables/README.md) | 1,000 | **1.43 ms**<br><sub>0.7%</sub> | **251 µs**<br><sub>1.8%</sub> | **376 µs**<br><sub>2.9%</sub> | **193 µs**<br><sub>4.1%</sub> | **219 µs**<br><sub>2.6%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10 | **12.1 ms**<br><sub>1.7%</sub> | **288 ms**<br><sub>1.1%</sub> | **12.1 ms**<br><sub>4.1%</sub> | **10.1 ms**<br><sub>8.0%</sub> | **68 ms**<br><sub>3.0%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 1,000 | **19 ms**<br><sub>1.0%</sub> | **413 ms**<br><sub>1.1%</sub> | **13.5 ms**<br><sub>2.6%</sub> | **11.3 ms**<br><sub>4.0%</sub> | **69.2 ms**<br><sub>2.1%</sub> |
| [call_by_name](../../benchmarks/call_by_name/README.md) | 10,000 | **23.2 ms**<br><sub>0.5%</sub> | **1.45 s**<br><sub>0.5%</sub> | **15.3 ms**<br><sub>5.1%</sub> | **12.6 ms**<br><sub>0.7%</sub> | **76.5 ms**<br><sub>0.8%</sub> |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10 | **4.97 ms**<br><sub>0.8%</sub> | **48.1 ms**<br><sub>1.8%</sub> | **4.91 ms**<br><sub>6.8%</sub> | **5.02 ms**<br><sub>3.2%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 1,000 | **5.24 ms**<br><sub>1.9%</sub> | **50.6 ms**<br><sub>3.2%</sub> | **5.04 ms**<br><sub>2.2%</sub> | **5.33 ms**<br><sub>1.7%</sub> | n/a |
| [call_by_handle](../../benchmarks/call_by_handle/README.md) | 10,000 | **5.36 ms**<br><sub>2.2%</sub> | **52.3 ms**<br><sub>1.1%</sub> | **5.33 ms**<br><sub>0.8%</sub> | **5.54 ms**<br><sub>2.0%</sub> | n/a |
| [engine_reset](../../benchmarks/engine_reset/README.md) | 100 | **65.4 µs**<br><sub>0.9%</sub> | **83.2 µs**<br><sub>2.1%</sub> | **7.55 µs**<br><sub>2.6%</sub> | **63.3 µs**<br><sub>11.8%</sub> | **30.7 µs**<br><sub>2.9%</sub> |

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
| fib_recursive | 15 | **39.1 µs**<br><sub>9.0%</sub> | **41.5 µs**<br><sub>1.06× · 4.5%</sub> | **35.6 µs**<br><sub>0.91× · 5.5%</sub> | **43.8 µs**<br><sub>1.12× · 5.0%</sub> |
| fib_recursive | 20 | **428 µs**<br><sub>8.6%</sub> | **441 µs**<br><sub>1.03× · 1.3%</sub> | **386 µs**<br><sub>0.901× · 4.2%</sub> | **477 µs**<br><sub>1.11× · 3.0%</sub> |
| fib_recursive | 25 | **4.69 ms**<br><sub>5.5%</sub> | **4.91 ms**<br><sub>1.05× · 3.0%</sub> | **4.26 ms**<br><sub>0.908× · 3.5%</sub> | **5.42 ms**<br><sub>1.15× · 2.4%</sub> |
| loop | 1,000,000 | **8.36 ms**<br><sub>0.0%</sub> | **10.9 ms**<br><sub>1.3× · 0.1%</sub> | **7.52 ms**<br><sub>0.9× · 0.1%</sub> | **12.1 ms**<br><sub>1.45× · 0.1%</sub> |
| int_arith | 5,000 | **7.23 ms**<br><sub>0.2%</sub> | **7.58 ms**<br><sub>1.05× · 0.3%</sub> | **6.39 ms**<br><sub>0.883× · 0.2%</sub> | **8.76 ms**<br><sub>1.21× · 0.2%</sub> |
| mandelbrot | 100 | **5.72 ms**<br><sub>0.8%</sub> | **5.97 ms**<br><sub>1.04× · 0.8%</sub> | **5.09 ms**<br><sub>0.89× · 0.4%</sub> | **6.63 ms**<br><sub>1.16× · 0.9%</sub> |
| math_builtins | 100,000 | **5.21 ms**<br><sub>11.1%</sub> | **5.05 ms**<br><sub>0.969× · 39.6%</sub> | **4.74 ms**<br><sub>0.911× · 121.7%</sub> | **5.46 ms**<br><sub>1.05× · 6.5%</sub> |
| calls | 100,000 | **5.03 ms**<br><sub>2.9%</sub> | **5.27 ms**<br><sub>1.05× · 4.0%</sub> | **4.58 ms**<br><sub>0.91× · 7.0%</sub> | **5.79 ms**<br><sub>1.15× · 9.0%</sub> |
| tak | 4 | **39.7 µs**<br><sub>8.3%</sub> | **41.8 µs**<br><sub>1.05× · 9.0%</sub> | **38 µs**<br><sub>0.958× · 6.8%</sub> | **43.4 µs**<br><sub>1.09× · 7.8%</sub> |
| tak | 5 | **233 µs**<br><sub>2.9%</sub> | **246 µs**<br><sub>1.05× · 7.9%</sub> | **225 µs**<br><sub>0.964× · 6.9%</sub> | **258 µs**<br><sub>1.1× · 4.9%</sub> |
| tak | 6 | **1.43 ms**<br><sub>3.8%</sub> | **1.5 ms**<br><sub>1.05× · 8.1%</sub> | **1.37 ms**<br><sub>0.96× · 4.5%</sub> | **1.59 ms**<br><sub>1.12× · 4.1%</sub> |
| branching | 300,000 | **9.63 ms**<br><sub>4.4%</sub> | **11.3 ms**<br><sub>1.17× · 4.4%</sub> | **8.36 ms**<br><sub>0.868× · 2.9%</sub> | **12.7 ms**<br><sub>1.32× · 3.2%</sub> |
| var_global | 1,000,000 | **10.9 ms**<br><sub>3.6%</sub> | **13.4 ms**<br><sub>1.23× · 4.6%</sub> | **10 ms**<br><sub>0.923× · 3.3%</sub> | **14.6 ms**<br><sub>1.35× · 4.4%</sub> |
| var_nested | 25 | **5.02 ms**<br><sub>3.5%</sub> | **6.07 ms**<br><sub>1.21× · 1.4%</sub> | **4.61 ms**<br><sub>0.917× · 5.5%</sub> | **6.68 ms**<br><sub>1.33× · 0.2%</sub> |
| var_lifecycle | 300,000 | **3.19 ms**<br><sub>0.2%</sub> | **5.07 ms**<br><sub>1.59× · 0.4%</sub> | **2.94 ms**<br><sub>0.922× · 0.2%</sub> | **5.45 ms**<br><sub>1.71× · 0.0%</sub> |
| sieve | 40,000 | **3.16 ms**<br><sub>2.7%</sub> | **3.46 ms**<br><sub>1.09× · 3.2%</sub> | **2.93 ms**<br><sub>0.926× · 1.3%</sub> | **3.72 ms**<br><sub>1.18× · 2.6%</sub> |
| sort | 1,000 | **10 ms**<br><sub>3.2%</sub> | **8.99 ms**<br><sub>0.895× · 3.1%</sub> | **8.5 ms**<br><sub>0.846× · 2.8%</sub> | **10.7 ms**<br><sub>1.06× · 3.3%</sub> |
| byte_buffer | 100,000 | **4.41 ms**<br><sub>2.1%</sub> | **4.76 ms**<br><sub>1.08× · 2.2%</sub> | **3.82 ms**<br><sub>0.867× · 2.8%</sub> | **5.42 ms**<br><sub>1.23× · 1.4%</sub> |
| maps | 50,000 | **59.7 ms**<br><sub>1.8%</sub> | **58.2 ms**<br><sub>0.976× · 5.3%</sub> | **57.2 ms**<br><sub>0.959× · 5.7%</sub> | **59.8 ms**<br><sub>1× · 2.0%</sub> |
| strings | 70,000 | **57.5 ms**<br><sub>12.6%</sub> | **55.7 ms**<br><sub>0.969× · 18.2%</sub> | **55.4 ms**<br><sub>0.964× · 3.3%</sub> | **57.9 ms**<br><sub>1.01× · 0.9%</sub> |
| records | 150,000 | **5.85 ms**<br><sub>7.1%</sub> | **5.45 ms**<br><sub>0.931× · 0.9%</sub> | **4.64 ms**<br><sub>0.792× · 1.1%</sub> | **6.42 ms**<br><sub>1.1× · 7.7%</sub> |
| binary_trees | 4 | **48.6 ms**<br><sub>2.2%</sub> | **57.5 ms**<br><sub>1.18× · 1.3%</sub> | **57.1 ms**<br><sub>1.17× · 5.7%</sub> | **48.8 ms**<br><sub>1× · 0.9%</sub> |
| binary_trees | 8 | **57.9 ms**<br><sub>4.0%</sub> | **68.4 ms**<br><sub>1.18× · 3.4%</sub> | **67.9 ms**<br><sub>1.17× · 2.6%</sub> | **59.1 ms**<br><sub>1.02× · 3.1%</sub> |
| binary_trees | 12 | **52.7 ms**<br><sub>1.8%</sub> | **63.8 ms**<br><sub>1.21× · 1.0%</sub> | **62.5 ms**<br><sub>1.19× · 1.1%</sub> | **53.6 ms**<br><sub>1.02× · 1.6%</sub> |
| script_to_host | 100,000 | **7.37 ms**<br><sub>1.1%</sub> | **7.22 ms**<br><sub>0.98× · 0.8%</sub> | **6.85 ms**<br><sub>0.929× · 0.7%</sub> | **7.72 ms**<br><sub>1.05× · 0.8%</sub> |
| host_to_script | 100,000 | **17.7 ms**<br><sub>1.7%</sub> | **19 ms**<br><sub>1.07× · 1.0%</sub> | **18.6 ms**<br><sub>1.05× · 0.8%</sub> | **17.9 ms**<br><sub>1.02× · 1.8%</sub> |
| cpp_objects | 150,000 | **4.98 ms**<br><sub>0.7%</sub> | **5.29 ms**<br><sub>1.06× · 0.4%</sub> | **4.42 ms**<br><sub>0.887× · 0.0%</sub> | **5.92 ms**<br><sub>1.19× · 0.1%</sub> |
| buffer | 400 | **4.98 ms**<br><sub>2.8%</sub> | **5.05 ms**<br><sub>1.01× · 0.0%</sub> | **4.52 ms**<br><sub>0.907× · 3.4%</sub> | **5.59 ms**<br><sub>1.12× · 2.8%</sub> |
| string_exchange | 250,000 | **21.4 ms**<br><sub>1.4%</sub> | **21.2 ms**<br><sub>0.993× · 0.7%</sub> | **20.9 ms**<br><sub>0.978× · 0.5%</sub> | **21.7 ms**<br><sub>1.01× · 0.7%</sub> |
| array_exchange | 10 | **3.83 ms**<br><sub>1.2%</sub> | **3.86 ms**<br><sub>1.01× · 1.6%</sub> | **3.71 ms**<br><sub>0.969× · 1.7%</sub> | **4.01 ms**<br><sub>1.05× · 1.2%</sub> |
| array_exchange | 1,000 | **1.85 ms**<br><sub>0.7%</sub> | **1.83 ms**<br><sub>0.991× · 0.6%</sub> | **1.71 ms**<br><sub>0.921× · 0.8%</sub> | **2 ms**<br><sub>1.08× · 0.7%</sub> |
| array_exchange | 40,000 | **1.98 ms**<br><sub>2.2%</sub> | **1.98 ms**<br><sub>0.998× · 2.1%</sub> | **1.88 ms**<br><sub>0.946× · 1.9%</sub> | **2.13 ms**<br><sub>1.07× · 0.1%</sub> |
| engine_lifecycle | 10 | **4.99 ms**<br><sub>1.2%</sub> | **5.68 ms**<br><sub>1.14× · 1.6%</sub> | **5.65 ms**<br><sub>1.13× · 0.7%</sub> | **5.02 ms**<br><sub>1.01× · 1.8%</sub> |
| engine_minimal | 10 | **249 µs**<br><sub>1.6%</sub> | **302 µs**<br><sub>1.21× · 2.9%</sub> | **302 µs**<br><sub>1.21× · 2.5%</sub> | **246 µs**<br><sub>0.988× · 1.8%</sub> |
| parse_compile | 100 | **3.88 ms**<br><sub>2.4%</sub> | **4.14 ms**<br><sub>1.07× · 1.8%</sub> | **4.11 ms**<br><sub>1.06× · 0.5%</sub> | **3.91 ms**<br><sub>1.01× · 0.6%</sub> |
| parse_compile | 1,000 | **38.5 ms**<br><sub>0.2%</sub> | **40.6 ms**<br><sub>1.05× · 0.5%</sub> | **40.4 ms**<br><sub>1.05× · 1.0%</sub> | **38.6 ms**<br><sub>1× · 0.8%</sub> |
| parse_compile | 10,000 | **653 ms**<br><sub>0.3%</sub> | **679 ms**<br><sub>1.04× · 0.6%</sub> | **674 ms**<br><sub>1.03× · 0.4%</sub> | **654 ms**<br><sub>1× · 0.2%</sub> |
| register_functions | 10 | **21.1 µs**<br><sub>21.8%</sub> | **23.8 µs**<br><sub>1.13× · 3.2%</sub> | **25.2 µs**<br><sub>1.19× · 5.8%</sub> | **20.4 µs**<br><sub>0.964× · 2.7%</sub> |
| register_functions | 100 | **185 µs**<br><sub>4.1%</sub> | **201 µs**<br><sub>1.09× · 1.7%</sub> | **198 µs**<br><sub>1.07× · 2.0%</sub> | **186 µs**<br><sub>1.01× · 0.9%</sub> |
| register_functions | 1,000 | **2.67 ms**<br><sub>1.1%</sub> | **2.82 ms**<br><sub>1.06× · 1.0%</sub> | **2.84 ms**<br><sub>1.06× · 1.5%</sub> | **2.66 ms**<br><sub>0.997× · 1.0%</sub> |
| register_variables | 10 | **12.9 µs**<br><sub>3.9%</sub> | **15.4 µs**<br><sub>1.19× · 1.6%</sub> | **14.7 µs**<br><sub>1.14× · 3.1%</sub> | **13.3 µs**<br><sub>1.03× · 1.9%</sub> |
| register_variables | 100 | **134 µs**<br><sub>2.3%</sub> | **137 µs**<br><sub>1.02× · 1.7%</sub> | **137 µs**<br><sub>1.02× · 1.8%</sub> | **134 µs**<br><sub>0.999× · 0.8%</sub> |
| register_variables | 1,000 | **1.43 ms**<br><sub>0.7%</sub> | **1.47 ms**<br><sub>1.03× · 0.9%</sub> | **1.48 ms**<br><sub>1.03× · 0.6%</sub> | **1.44 ms**<br><sub>1× · 1.6%</sub> |
| call_by_name | 10 | **12.1 ms**<br><sub>1.7%</sub> | **12.4 ms**<br><sub>1.03× · 1.6%</sub> | **12.3 ms**<br><sub>1.01× · 0.6%</sub> | **12.1 ms**<br><sub>1.01× · 2.1%</sub> |
| call_by_name | 1,000 | **19 ms**<br><sub>1.0%</sub> | **18.8 ms**<br><sub>0.992× · 0.6%</sub> | **18.7 ms**<br><sub>0.988× · 0.8%</sub> | **19.3 ms**<br><sub>1.02× · 1.6%</sub> |
| call_by_name | 10,000 | **23.2 ms**<br><sub>0.5%</sub> | **22.9 ms**<br><sub>0.99× · 0.7%</sub> | **22.7 ms**<br><sub>0.981× · 0.7%</sub> | **23.3 ms**<br><sub>1.01× · 0.9%</sub> |
| call_by_handle | 10 | **4.97 ms**<br><sub>0.8%</sub> | **5.61 ms**<br><sub>1.13× · 0.8%</sub> | **5.49 ms**<br><sub>1.11× · 1.1%</sub> | **5.06 ms**<br><sub>1.02× · 1.4%</sub> |
| call_by_handle | 1,000 | **5.24 ms**<br><sub>1.9%</sub> | **5.82 ms**<br><sub>1.11× · 1.7%</sub> | **5.73 ms**<br><sub>1.09× · 1.0%</sub> | **5.39 ms**<br><sub>1.03× · 1.5%</sub> |
| call_by_handle | 10,000 | **5.36 ms**<br><sub>2.2%</sub> | **5.85 ms**<br><sub>1.09× · 1.7%</sub> | **5.8 ms**<br><sub>1.08× · 1.6%</sub> | **5.47 ms**<br><sub>1.02× · 2.7%</sub> |
| engine_reset | 100 | **65.4 µs**<br><sub>0.9%</sub> | **82.9 µs**<br><sub>1.27× · 1.0%</sub> | **83.2 µs**<br><sub>1.27× · 1.3%</sub> | **65.1 µs**<br><sub>0.995× · 4.9%</sub> |

### ChaiScript

| Benchmark | n | no-threads (headline) | default |
|---|--:|--:|--:|
| fib_recursive | 15 | **2.27 ms**<br><sub>0.6%</sub> | **2.4 ms**<br><sub>1.06× · 1.2%</sub> |
| fib_recursive | 20 | **149 ms**<br><sub>0.2%</sub> | **150 ms**<br><sub>1.01× · 0.1%</sub> |
| fib_recursive | 25 | **18.1 s**<br><sub>0.1% · budget</sub> | **18.1 s**<br><sub>1× · 0.1% · budget</sub> |
| loop | 1,000,000 | **448 ms**<br><sub>0.8%</sub> | **469 ms**<br><sub>1.05× · 0.4%</sub> |
| int_arith | 5,000 | **278 ms**<br><sub>1.7%</sub> | **297 ms**<br><sub>1.07× · 1.2%</sub> |
| mandelbrot | 100 | **404 ms**<br><sub>2.8%</sub> | **425 ms**<br><sub>1.05× · 1.6%</sub> |
| math_builtins | 100,000 | **141 ms**<br><sub>4.0%</sub> | **133 ms**<br><sub>0.943× · 6.2%</sub> |
| calls | 100,000 | **215 ms**<br><sub>5.8%</sub> | **240 ms**<br><sub>1.12× · 5.8%</sub> |
| tak | 4 | **3.62 ms**<br><sub>4.8%</sub> | **3.7 ms**<br><sub>1.02× · 2.8%</sub> |
| tak | 5 | **95.1 ms**<br><sub>7.5%</sub> | **95.1 ms**<br><sub>1× · 9.5%</sub> |
| tak | 6 | **3.64 s**<br><sub>5.1% · budget</sub> | **3.63 s**<br><sub>0.997× · 5.8% · budget</sub> |
| branching | 300,000 | **322 ms**<br><sub>5.4%</sub> | **355 ms**<br><sub>1.11× · 3.8%</sub> |
| var_global | 1,000,000 | **513 ms**<br><sub>3.4%</sub> | **542 ms**<br><sub>1.06× · 3.6%</sub> |
| var_nested | 25 | **361 ms**<br><sub>3.0%</sub> | **379 ms**<br><sub>1.05× · 3.4%</sub> |
| var_lifecycle | 300,000 | **371 ms**<br><sub>2.4%</sub> | **389 ms**<br><sub>1.05× · 2.0%</sub> |
| sieve | 40,000 | **485 ms**<br><sub>4.0%</sub> | **517 ms**<br><sub>1.06× · 4.1%</sub> |
| sort | 1,000 | **422 ms**<br><sub>4.4%</sub> | **447 ms**<br><sub>1.06× · 3.2%</sub> |
| byte_buffer | 100,000 | **494 ms**<br><sub>4.4%</sub> | **516 ms**<br><sub>1.04× · 1.1%</sub> |
| maps | 50,000 | **402 ms**<br><sub>5.7%</sub> | **426 ms**<br><sub>1.06× · 1.7%</sub> |
| strings | 70,000 | **444 ms**<br><sub>5.3%</sub> | **463 ms**<br><sub>1.04× · 9.8%</sub> |
| records | 150,000 | **436 ms**<br><sub>3.3%</sub> | **466 ms**<br><sub>1.07× · 10.0%</sub> |
| binary_trees | 4 | **541 ms**<br><sub>0.6%</sub> | **562 ms**<br><sub>1.04× · 1.0%</sub> |
| binary_trees | 8 | **937 ms**<br><sub>1.4%</sub> | **991 ms**<br><sub>1.06× · 1.0%</sub> |
| binary_trees | 12 | **7.69 s**<br><sub>0.4% · budget</sub> | **7.71 s**<br><sub>1× · 0.1% · budget</sub> |
| script_to_host | 100,000 | **169 ms**<br><sub>4.8%</sub> | **166 ms**<br><sub>0.983× · 6.0%</sub> |
| host_to_script | 100,000 | **187 ms**<br><sub>6.3%</sub> | **192 ms**<br><sub>1.03× · 2.4%</sub> |
| cpp_objects | 150,000 | **496 ms**<br><sub>3.5%</sub> | **501 ms**<br><sub>1.01× · 2.3%</sub> |
| buffer | 400 | **317 ms**<br><sub>1.8%</sub> | **317 ms**<br><sub>1× · 4.7%</sub> |
| string_exchange | 250,000 | **78 ms**<br><sub>2.9%</sub> | **84.2 ms**<br><sub>1.08× · 2.5%</sub> |
| array_exchange | 10 | **400 ms**<br><sub>0.5%</sub> | **412 ms**<br><sub>1.03× · 0.7%</sub> |
| array_exchange | 1,000 | **358 ms**<br><sub>0.9%</sub> | **368 ms**<br><sub>1.03× · 0.8%</sub> |
| array_exchange | 40,000 | **360 ms**<br><sub>0.6%</sub> | **370 ms**<br><sub>1.03× · 0.9%</sub> |
| engine_lifecycle | 10 | **40.5 ms**<br><sub>0.9%</sub> | **41.2 ms**<br><sub>1.02× · 0.8%</sub> |
| engine_minimal | 10 | **230 µs**<br><sub>3.1%</sub> | **240 µs**<br><sub>1.04× · 2.5%</sub> |
| parse_compile | 100 | **4.91 ms**<br><sub>0.5%</sub> | **4.96 ms**<br><sub>1.01× · 0.9%</sub> |
| parse_compile | 1,000 | **55.9 ms**<br><sub>0.5%</sub> | **56.3 ms**<br><sub>1.01× · 0.5%</sub> |
| parse_compile | 10,000 | **1.33 s**<br><sub>0.3%</sub> | **1.36 s**<br><sub>1.02× · 0.2%</sub> |
| register_functions | 10 | **48.6 µs**<br><sub>0.8%</sub> | **52 µs**<br><sub>1.07× · 3.9%</sub> |
| register_functions | 100 | **527 µs**<br><sub>1.5%</sub> | **540 µs**<br><sub>1.02× · 1.1%</sub> |
| register_functions | 1,000 | **12.8 ms**<br><sub>0.3%</sub> | **13.2 ms**<br><sub>1.03× · 0.2%</sub> |
| register_variables | 10 | **2.25 µs**<br><sub>4.4%</sub> | **2.4 µs**<br><sub>1.07× · 6.3%</sub> |
| register_variables | 100 | **21.2 µs**<br><sub>2.1%</sub> | **21.9 µs**<br><sub>1.03× · 0.5%</sub> |
| register_variables | 1,000 | **251 µs**<br><sub>1.8%</sub> | **269 µs**<br><sub>1.07× · 0.5%</sub> |
| call_by_name | 10 | **288 ms**<br><sub>1.1%</sub> | **304 ms**<br><sub>1.05× · 1.5%</sub> |
| call_by_name | 1,000 | **413 ms**<br><sub>1.1%</sub> | **437 ms**<br><sub>1.06× · 0.7%</sub> |
| call_by_name | 10,000 | **1.45 s**<br><sub>0.5%</sub> | **1.55 s**<br><sub>1.07× · 0.6%</sub> |
| call_by_handle | 10 | **48.1 ms**<br><sub>1.8%</sub> | **51.1 ms**<br><sub>1.06× · 1.6%</sub> |
| call_by_handle | 1,000 | **50.6 ms**<br><sub>3.2%</sub> | **53.4 ms**<br><sub>1.05× · 2.5%</sub> |
| call_by_handle | 10,000 | **52.3 ms**<br><sub>1.1%</sub> | **54.8 ms**<br><sub>1.05× · 1.5%</sub> |
| engine_reset | 100 | **83.2 µs**<br><sub>2.1%</sub> | **84.4 µs**<br><sub>1.01× · 0.5%</sub> |

### Lua

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **10 ms**<br><sub>2.1%</sub> | **4.97 ms**<br><sub>0.496× · 2.1%</sub> |
| host_to_script | 100,000 | **16.1 ms**<br><sub>1.9%</sub> | **10.5 ms**<br><sub>0.651× · 1.9%</sub> |
| cpp_objects | 150,000 | **107 ms**<br><sub>2.1%</sub> | **64.9 ms**<br><sub>0.604× · 7.4%</sub> |
| buffer | 400 | **20.9 ms**<br><sub>1.7%</sub> | **9.85 ms**<br><sub>0.472× · 1.2%</sub> |
| string_exchange | 250,000 | **36.6 ms**<br><sub>0.9%</sub> | **30.5 ms**<br><sub>0.834× · 1.5%</sub> |
| array_exchange | 10 | **3.22 ms**<br><sub>4.8%</sub> | **2.57 ms**<br><sub>0.798× · 3.7%</sub> |
| array_exchange | 1,000 | **1.21 ms**<br><sub>3.9%</sub> | **954 µs**<br><sub>0.788× · 7.1%</sub> |
| array_exchange | 40,000 | **1.15 ms**<br><sub>1.6%</sub> | **834 µs**<br><sub>0.727× · 3.0%</sub> |
| register_functions | 10 | **3 µs**<br><sub>3.3%</sub> | **800 ns**<br><sub>0.267× · 0.0%</sub> |
| register_functions | 100 | **20.1 µs**<br><sub>2.2%</sub> | **8.85 µs**<br><sub>0.439× · 4.0%</sub> |
| register_functions | 1,000 | **227 µs**<br><sub>1.3%</sub> | **105 µs**<br><sub>0.462× · 5.6%</sub> |
| register_variables | 10 | **4.95 µs**<br><sub>4.0%</sub> | **800 ns**<br><sub>0.162× · 6.3%</sub> |
| register_variables | 100 | **36 µs**<br><sub>1.2%</sub> | **8.6 µs**<br><sub>0.239× · 4.1%</sub> |
| register_variables | 1,000 | **376 µs**<br><sub>2.9%</sub> | **100 µs**<br><sub>0.267× · 3.8%</sub> |
| call_by_name | 10 | **12.1 ms**<br><sub>4.1%</sub> | **3.61 ms**<br><sub>0.299× · 13.8%</sub> |
| call_by_name | 1,000 | **13.5 ms**<br><sub>2.6%</sub> | **5.63 ms**<br><sub>0.418× · 2.0%</sub> |
| call_by_name | 10,000 | **15.3 ms**<br><sub>5.1%</sub> | **8.06 ms**<br><sub>0.528× · 1.9%</sub> |
| call_by_handle | 10 | **4.91 ms**<br><sub>6.8%</sub> | **2.94 ms**<br><sub>0.599× · 6.0%</sub> |
| call_by_handle | 1,000 | **5.04 ms**<br><sub>2.2%</sub> | **3.11 ms**<br><sub>0.616× · 2.8%</sub> |
| call_by_handle | 10,000 | **5.33 ms**<br><sub>0.8%</sub> | **3.81 ms**<br><sub>0.714× · 3.5%</sub> |

### Python

| Benchmark | n | (no name) (headline) | c-api |
|---|--:|--:|--:|
| script_to_host | 100,000 | **48.2 ms**<br><sub>1.4%</sub> | **8.49 ms**<br><sub>0.176× · 3.8%</sub> |
| host_to_script | 100,000 | **20.2 ms**<br><sub>3.1%</sub> | **15.9 ms**<br><sub>0.787× · 3.4%</sub> |
| cpp_objects | 150,000 | **258 ms**<br><sub>0.5%</sub> | **33.3 ms**<br><sub>0.129× · 10.6%</sub> |
| buffer | 400 | **17.9 ms**<br><sub>2.8%</sub> | **17.9 ms**<br><sub>0.999× · 6.5%</sub> |
| string_exchange | 250,000 | **20.4 ms**<br><sub>7.8%</sub> | **17 ms**<br><sub>0.834× · 9.1%</sub> |
| array_exchange | 10 | **2.76 ms**<br><sub>7.7%</sub> | **2.12 ms**<br><sub>0.768× · 1.2%</sub> |
| array_exchange | 1,000 | **1.6 ms**<br><sub>2.3%</sub> | **1.35 ms**<br><sub>0.843× · 1.7%</sub> |
| array_exchange | 40,000 | **1.8 ms**<br><sub>2.1%</sub> | **1.54 ms**<br><sub>0.855× · 6.2%</sub> |
| register_functions | 10 | **34.9 µs**<br><sub>9.2%</sub> | **2.7 µs**<br><sub>0.0775× · 25.9%</sub> |
| register_functions | 100 | **248 µs**<br><sub>3.0%</sub> | **22 µs**<br><sub>0.0886× · 3.6%</sub> |
| register_functions | 1,000 | **2.3 ms**<br><sub>1.5%</sub> | **225 µs**<br><sub>0.0976× · 3.6%</sub> |
| register_variables | 10 | **2.55 µs**<br><sub>15.7%</sub> | **1.5 µs**<br><sub>0.588× · 6.7%</sub> |
| register_variables | 100 | **17.9 µs**<br><sub>4.2%</sub> | **12.8 µs**<br><sub>0.715× · 4.7%</sub> |
| register_variables | 1,000 | **193 µs**<br><sub>4.1%</sub> | **154 µs**<br><sub>0.798× · 4.0%</sub> |
| call_by_name | 10 | **10.1 ms**<br><sub>8.0%</sub> | **9.32 ms**<br><sub>0.925× · 8.9%</sub> |
| call_by_name | 1,000 | **11.3 ms**<br><sub>4.0%</sub> | **10.6 ms**<br><sub>0.936× · 3.0%</sub> |
| call_by_name | 10,000 | **12.6 ms**<br><sub>0.7%</sub> | **12 ms**<br><sub>0.95× · 4.1%</sub> |
| call_by_handle | 10 | **5.02 ms**<br><sub>3.2%</sub> | **3.98 ms**<br><sub>0.792× · 2.2%</sub> |
| call_by_handle | 1,000 | **5.33 ms**<br><sub>1.7%</sub> | **4.61 ms**<br><sub>0.864× · 3.9%</sub> |
| call_by_handle | 10,000 | **5.54 ms**<br><sub>2.0%</sub> | **4.8 ms**<br><sub>0.866× · 2.6%</sub> |

### TeaScript

| Benchmark | n | vm-O1 (headline) | ast | vm-O0 | vm-O2 |
|---|--:|--:|--:|--:|--:|
| fib_recursive | 15 | **1.11 ms**<br><sub>0.7%</sub> | **1.09 ms**<br><sub>0.975× · 2.6%</sub> | **1.12 ms**<br><sub>1× · 2.7%</sub> | **1.04 ms**<br><sub>0.938× · 3.8%</sub> |
| fib_recursive | 20 | **12.5 ms**<br><sub>1.6%</sub> | **12 ms**<br><sub>0.964× · 1.3%</sub> | **12.4 ms**<br><sub>0.992× · 0.7%</sub> | **11.6 ms**<br><sub>0.93× · 1.2%</sub> |
| fib_recursive | 25 | **139 ms**<br><sub>4.1%</sub> | **147 ms**<br><sub>1.06× · 0.6%</sub> | **137 ms**<br><sub>0.988× · 3.0%</sub> | **128 ms**<br><sub>0.922× · 0.8%</sub> |
| loop | 1,000,000 | **220 ms**<br><sub>0.4%</sub> | **151 ms**<br><sub>0.686× · 0.1%</sub> | **220 ms**<br><sub>0.999× · 0.3%</sub> | **204 ms**<br><sub>0.927× · 0.3%</sub> |
| int_arith | 5,000 | **247 ms**<br><sub>0.7%</sub> | **198 ms**<br><sub>0.8× · 0.6%</sub> | **247 ms**<br><sub>0.998× · 0.6%</sub> | **235 ms**<br><sub>0.952× · 0.6%</sub> |
| mandelbrot | 100 | **240 ms**<br><sub>0.7%</sub> | **208 ms**<br><sub>0.864× · 0.5%</sub> | **241 ms**<br><sub>1× · 0.7%</sub> | **241 ms**<br><sub>1× · 0.9%</sub> |
| math_builtins | 100,000 | **180 ms**<br><sub>3.6%</sub> | **171 ms**<br><sub>0.952× · 2.4%</sub> | **179 ms**<br><sub>1× · 6.2%</sub> | **177 ms**<br><sub>0.984× · 8.7%</sub> |
| calls | 100,000 | **164 ms**<br><sub>2.8%</sub> | **141 ms**<br><sub>0.859× · 6.7%</sub> | **163 ms**<br><sub>0.995× · 4.2%</sub> | **157 ms**<br><sub>0.956× · 6.2%</sub> |
| tak | 4 | **1.33 ms**<br><sub>7.4%</sub> | **1.3 ms**<br><sub>0.98× · 8.9%</sub> | **1.34 ms**<br><sub>1.01× · 11.9%</sub> | **1.26 ms**<br><sub>0.946× · 8.5%</sub> |
| tak | 5 | **8.86 ms**<br><sub>3.0%</sub> | **7.04 ms**<br><sub>0.794× · 5.4%</sub> | **8.87 ms**<br><sub>1× · 4.7%</sub> | **8.44 ms**<br><sub>0.953× · 4.3%</sub> |
| tak | 6 | **52.1 ms**<br><sub>5.2%</sub> | **43.3 ms**<br><sub>0.83× · 4.6%</sub> | **51.3 ms**<br><sub>0.984× · 5.8%</sub> | **49 ms**<br><sub>0.941× · 2.1%</sub> |
| branching | 300,000 | **321 ms**<br><sub>2.9%</sub> | **230 ms**<br><sub>0.717× · 3.5%</sub> | **321 ms**<br><sub>1× · 0.3%</sub> | **299 ms**<br><sub>0.933× · 2.9%</sub> |
| var_global | 1,000,000 | **220 ms**<br><sub>2.8%</sub> | **151 ms**<br><sub>0.686× · 3.5%</sub> | **220 ms**<br><sub>1× · 3.5%</sub> | **204 ms**<br><sub>0.926× · 1.7%</sub> |
| var_nested | 25 | **171 ms**<br><sub>2.3%</sub> | **113 ms**<br><sub>0.665× · 3.8%</sub> | **171 ms**<br><sub>1× · 3.6%</sub> | **164 ms**<br><sub>0.963× · 2.5%</sub> |
| var_lifecycle | 300,000 | **220 ms**<br><sub>1.9%</sub> | **148 ms**<br><sub>0.675× · 2.1%</sub> | **220 ms**<br><sub>0.999× · 1.4%</sub> | **220 ms**<br><sub>0.998× · 0.9%</sub> |
| sieve | 40,000 | **34.9 ms**<br><sub>11.6%</sub> | **43.1 ms**<br><sub>1.23× · 11.8%</sub> | **34.9 ms**<br><sub>1× · 10.0%</sub> | **31.7 ms**<br><sub>0.908× · 9.4%</sub> |
| sort | 1,000 | **206 ms**<br><sub>4.2%</sub> | **234 ms**<br><sub>1.13× · 4.1%</sub> | **207 ms**<br><sub>1× · 3.9%</sub> | **208 ms**<br><sub>1.01× · 3.9%</sub> |
| byte_buffer | 100,000 | **102 ms**<br><sub>3.2%</sub> | **101 ms**<br><sub>0.993× · 3.1%</sub> | **102 ms**<br><sub>1× · 2.1%</sub> | **98.3 ms**<br><sub>0.967× · 3.7%</sub> |
| maps | 50,000 | **160 ms**<br><sub>2.9%</sub> | **179 ms**<br><sub>1.12× · 5.7%</sub> | **160 ms**<br><sub>0.999× · 1.9%</sub> | **155 ms**<br><sub>0.971× · 2.1%</sub> |
| strings | 70,000 | **111 ms**<br><sub>9.9%</sub> | **89.4 ms**<br><sub>0.808× · 5.9%</sub> | **110 ms**<br><sub>0.998× · 2.7%</sub> | **109 ms**<br><sub>0.987× · 9.8%</sub> |
| records | 150,000 | **155 ms**<br><sub>1.1%</sub> | **107 ms**<br><sub>0.688× · 1.4%</sub> | **155 ms**<br><sub>1× · 8.5%</sub> | **153 ms**<br><sub>0.987× · 1.2%</sub> |
| binary_trees | 4 | **271 ms**<br><sub>1.1%</sub> | **260 ms**<br><sub>0.963× · 0.7%</sub> | **271 ms**<br><sub>1× · 0.8%</sub> | **263 ms**<br><sub>0.97× · 0.5%</sub> |
| binary_trees | 8 | **271 ms**<br><sub>0.4%</sub> | **266 ms**<br><sub>0.982× · 2.3%</sub> | **270 ms**<br><sub>0.998× · 2.4%</sub> | **262 ms**<br><sub>0.969× · 1.6%</sub> |
| binary_trees | 12 | **296 ms**<br><sub>0.6%</sub> | **291 ms**<br><sub>0.981× · 0.7%</sub> | **296 ms**<br><sub>0.998× · 0.4%</sub> | **287 ms**<br><sub>0.968× · 0.4%</sub> |
| script_to_host | 100,000 | **90.7 ms**<br><sub>1.2%</sub> | **83.2 ms**<br><sub>0.917× · 0.9%</sub> | **90.6 ms**<br><sub>0.999× · 0.5%</sub> | **89.4 ms**<br><sub>0.985× · 1.1%</sub> |
| host_to_script | 100,000 | **225 ms**<br><sub>0.7%</sub> | **108 ms**<br><sub>0.479× · 2.0%</sub> | **225 ms**<br><sub>0.999× · 1.1%</sub> | **217 ms**<br><sub>0.967× · 1.1%</sub> |
| buffer | 400 | **148 ms**<br><sub>0.8%</sub> | **117 ms**<br><sub>0.79× · 2.5%</sub> | **148 ms**<br><sub>1× · 0.8%</sub> | **146 ms**<br><sub>0.984× · 0.9%</sub> |
| string_exchange | 250,000 | **159 ms**<br><sub>0.7%</sub> | **281 ms**<br><sub>1.77× · 0.3%</sub> | **160 ms**<br><sub>1.01× · 0.6%</sub> | **155 ms**<br><sub>0.979× · 0.6%</sub> |
| array_exchange | 10 | **30.2 ms**<br><sub>14.9%</sub> | **33.5 ms**<br><sub>1.11× · 1.4%</sub> | **29.5 ms**<br><sub>0.978× · 2.9%</sub> | **29 ms**<br><sub>0.96× · 0.8%</sub> |
| array_exchange | 1,000 | **24.4 ms**<br><sub>0.6%</sub> | **26.2 ms**<br><sub>1.08× · 0.7%</sub> | **24.2 ms**<br><sub>0.992× · 0.5%</sub> | **23.6 ms**<br><sub>0.966× · 1.0%</sub> |
| array_exchange | 40,000 | **25.6 ms**<br><sub>0.2%</sub> | **28.1 ms**<br><sub>1.1× · 1.9%</sub> | **25.5 ms**<br><sub>0.997× · 0.5%</sub> | **24.9 ms**<br><sub>0.972× · 0.5%</sub> |
| engine_lifecycle | 10 | **12.8 ms**<br><sub>0.8%</sub> | **8.36 ms**<br><sub>0.655× · 1.6%</sub> | **13.9 ms**<br><sub>1.09× · 3.4%</sub> | **11.8 ms**<br><sub>0.923× · 4.7%</sub> |
| engine_minimal | 10 | **163 µs**<br><sub>5.3%</sub> | **156 µs**<br><sub>0.961× · 4.0%</sub> | **157 µs**<br><sub>0.965× · 2.4%</sub> | **157 µs**<br><sub>0.965× · 3.7%</sub> |
| parse_compile | 100 | **3.11 ms**<br><sub>4.5%</sub> | **1.78 ms**<br><sub>0.572× · 2.4%</sub> | **3.42 ms**<br><sub>1.1× · 3.3%</sub> | **2.73 ms**<br><sub>0.878× · 3.5%</sub> |
| parse_compile | 1,000 | **28 ms**<br><sub>1.0%</sub> | **17.3 ms**<br><sub>0.617× · 1.2%</sub> | **31.2 ms**<br><sub>1.11× · 0.8%</sub> | **28.2 ms**<br><sub>1.01× · 0.6%</sub> |
| parse_compile | 10,000 | **284 ms**<br><sub>0.4%</sub> | **172 ms**<br><sub>0.607× · 0.7%</sub> | **324 ms**<br><sub>1.14× · 0.6%</sub> | **286 ms**<br><sub>1.01× · 0.6%</sub> |
| register_functions | 10 | **3.5 µs**<br><sub>12.9%</sub> | **3.9 µs**<br><sub>1.11× · 24.4%</sub> | **4.45 µs**<br><sub>1.27× · 5.6%</sub> | **3.5 µs**<br><sub>1× · 5.7%</sub> |
| register_functions | 100 | **42.5 µs**<br><sub>3.1%</sub> | **42.8 µs**<br><sub>1.01× · 3.2%</sub> | **40.6 µs**<br><sub>0.955× · 3.2%</sub> | **42.5 µs**<br><sub>0.999× · 2.9%</sub> |
| register_functions | 1,000 | **297 µs**<br><sub>4.2%</sub> | **305 µs**<br><sub>1.03× · 6.4%</sub> | **296 µs**<br><sub>0.999× · 4.6%</sub> | **299 µs**<br><sub>1.01× · 2.7%</sub> |
| register_variables | 10 | **2.4 µs**<br><sub>12.5%</sub> | **1.6 µs**<br><sub>0.667× · 112.5%</sub> | **3.4 µs**<br><sub>1.42× · 16.2%</sub> | **2.75 µs**<br><sub>1.15× · 3.6%</sub> |
| register_variables | 100 | **31.5 µs**<br><sub>1.9%</sub> | **32.7 µs**<br><sub>1.04× · 4.0%</sub> | **31.1 µs**<br><sub>0.989× · 2.6%</sub> | **33.1 µs**<br><sub>1.05× · 6.2%</sub> |
| register_variables | 1,000 | **219 µs**<br><sub>2.6%</sub> | **235 µs**<br><sub>1.07× · 9.5%</sub> | **222 µs**<br><sub>1.01× · 8.3%</sub> | **221 µs**<br><sub>1.01× · 1.7%</sub> |
| call_by_name | 10 | **68 ms**<br><sub>3.0%</sub> | **32.1 ms**<br><sub>0.473× · 1.7%</sub> | **68.1 ms**<br><sub>1× · 1.2%</sub> | **66.4 ms**<br><sub>0.977× · 0.7%</sub> |
| call_by_name | 1,000 | **69.2 ms**<br><sub>2.1%</sub> | **35.6 ms**<br><sub>0.514× · 1.5%</sub> | **68.9 ms**<br><sub>0.996× · 2.2%</sub> | **67.5 ms**<br><sub>0.975× · 0.9%</sub> |
| call_by_name | 10,000 | **76.5 ms**<br><sub>0.8%</sub> | **49.1 ms**<br><sub>0.642× · 1.3%</sub> | **76.1 ms**<br><sub>0.994× · 1.1%</sub> | **74.7 ms**<br><sub>0.977× · 1.6%</sub> |
| engine_reset | 100 | **30.7 µs**<br><sub>2.9%</sub> | **20.7 µs**<br><sub>0.674× · 1.4%</sub> | **32.5 µs**<br><sub>1.06× · 3.2%</sub> | **30.8 µs**<br><sub>1× · 3.7%</sub> |

## Peak memory

The peak memory of each process, the median over the processes of a run: the peak working set on Windows, the peak resident set on Linux. It covers the whole process: the runtime and libraries of the language, the unmeasured warm-up engine, the engine of the run and its workload. Below it: the difference to the process of the C++ runner for the same benchmark and n, which is the harness alone where C++ is "n/a" (the host API benchmarks). Headline variants.

### Core

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| fib_recursive | 15 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.4 MiB**<br><sub>+2.2 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| fib_recursive | 20 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **10.5 MiB**<br><sub>+6.3 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| fib_recursive | 25 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **51.2 MiB**<br><sub>+47.0 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| loop | 1,000,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| int_arith | 5,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.0 MiB**<br><sub>+5.8 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| mandelbrot | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| math_builtins | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| calls | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| tak | 4 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.3 MiB**<br><sub>+2.2 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| tak | 5 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **8.2 MiB**<br><sub>+4.0 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| tak | 6 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **17.2 MiB**<br><sub>+13.0 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| branching | 300,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |

### Data

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| var_global | 1,000,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| var_nested | 25 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| var_lifecycle | 300,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.6 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| sieve | 40,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **13.5 MiB**<br><sub>+9.4 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **11.0 MiB**<br><sub>+6.9 MiB</sub> | **14.1 MiB**<br><sub>+9.9 MiB</sub> |
| sort | 1,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.3 MiB**<br><sub>+2.1 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.8 MiB</sub> | **5.9 MiB**<br><sub>+1.8 MiB</sub> |
| byte_buffer | 100,000 | 4.3 MiB | **5.2 MiB**<br><sub>+0.9 MiB</sub> | **24.8 MiB**<br><sub>+20.5 MiB</sub> | **5.2 MiB**<br><sub>+0.9 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.6 MiB**<br><sub>+1.4 MiB</sub> |
| maps | 50,000 | 9.9 MiB | **13.1 MiB**<br><sub>+3.2 MiB</sub> | **30.3 MiB**<br><sub>+20.3 MiB</sub> | **10.2 MiB**<br><sub>+0.2 MiB</sub> | **17.9 MiB**<br><sub>+7.9 MiB</sub> | **28.8 MiB**<br><sub>+18.9 MiB</sub> |
| strings | 70,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.7 MiB**<br><sub>+1.5 MiB</sub> |
| records | 150,000 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.9 MiB**<br><sub>+5.7 MiB</sub> | **5.7 MiB**<br><sub>+1.5 MiB</sub> |
| binary_trees | 4 | 4.2 MiB | **18.5 MiB**<br><sub>+14.3 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| binary_trees | 8 | 4.2 MiB | **26.0 MiB**<br><sub>+21.8 MiB</sub> | **6.9 MiB**<br><sub>+2.7 MiB</sub> | **4.7 MiB**<br><sub>+0.5 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.7 MiB**<br><sub>+1.4 MiB</sub> |
| binary_trees | 12 | 4.2 MiB | **26.3 MiB**<br><sub>+22.1 MiB</sub> | **13.2 MiB**<br><sub>+8.9 MiB</sub> | **7.4 MiB**<br><sub>+3.2 MiB</sub> | **11.7 MiB**<br><sub>+7.4 MiB</sub> | **10.9 MiB**<br><sub>+6.7 MiB</sub> |

### Interop (script <-> C++)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| script_to_host | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.3 MiB**<br><sub>+6.1 MiB</sub> | **5.6 MiB**<br><sub>+1.5 MiB</sub> |
| host_to_script | 100,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| cpp_objects | 150,000 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.7 MiB**<br><sub>+6.5 MiB</sub> | n/a |
| buffer | 400 | 4.8 MiB | **6.8 MiB**<br><sub>+2.0 MiB</sub> | **7.2 MiB**<br><sub>+2.5 MiB</sub> | **5.7 MiB**<br><sub>+0.9 MiB</sub> | **12.1 MiB**<br><sub>+7.3 MiB</sub> | **6.5 MiB**<br><sub>+1.7 MiB</sub> |
| string_exchange | 250,000 | 4.2 MiB | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **5.9 MiB**<br><sub>+1.8 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **9.8 MiB**<br><sub>+5.6 MiB</sub> | **5.8 MiB**<br><sub>+1.6 MiB</sub> |
| array_exchange | 10 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.3 MiB**<br><sub>+2.1 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.7 MiB**<br><sub>+1.6 MiB</sub> |
| array_exchange | 1,000 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.6 MiB**<br><sub>+2.4 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **9.8 MiB**<br><sub>+5.7 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| array_exchange | 40,000 | 4.3 MiB | **5.7 MiB**<br><sub>+1.4 MiB</sub> | **21.4 MiB**<br><sub>+17.2 MiB</sub> | **5.4 MiB**<br><sub>+1.2 MiB</sub> | **11.0 MiB**<br><sub>+6.7 MiB</sub> | **23.3 MiB**<br><sub>+19.0 MiB</sub> |

### Host API (C++ level)

| Benchmark | n | C++ | AngelScript no-threads-no-line-cues | ChaiScript no-threads | Lua | Python | TeaScript vm-O1 |
|---|--:|--:|--:|--:|--:|--:|--:|
| engine_lifecycle | 10 | 4.2 MiB | **4.8 MiB**<br><sub>+0.6 MiB</sub> | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **4.4 MiB**<br><sub>+0.3 MiB</sub> | **10.6 MiB**<br><sub>+6.5 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| engine_minimal | 10 | 4.2 MiB | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **4.3 MiB**<br><sub>+0.1 MiB</sub> | n/a | **4.6 MiB**<br><sub>+0.4 MiB</sub> |
| parse_compile | 100 | 4.2 MiB | **5.7 MiB**<br><sub>+1.5 MiB</sub> | **7.0 MiB**<br><sub>+2.9 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **13.9 MiB**<br><sub>+9.7 MiB</sub> | **8.9 MiB**<br><sub>+4.7 MiB</sub> |
| parse_compile | 1,000 | 4.2 MiB | **8.9 MiB**<br><sub>+4.7 MiB</sub> | **14.6 MiB**<br><sub>+10.5 MiB</sub> | **5.7 MiB**<br><sub>+1.6 MiB</sub> | **36.9 MiB**<br><sub>+32.7 MiB</sub> | **31.1 MiB**<br><sub>+26.9 MiB</sub> |
| parse_compile | 10,000 | 4.2 MiB | **41.5 MiB**<br><sub>+37.4 MiB</sub> | **92.1 MiB**<br><sub>+88.0 MiB</sub> | **15.5 MiB**<br><sub>+11.3 MiB</sub> | **270.0 MiB**<br><sub>+265.8 MiB</sub> | **263.5 MiB**<br><sub>+259.3 MiB</sub> |
| register_functions | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.2 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **11.6 MiB**<br><sub>+7.4 MiB</sub> | **5.9 MiB**<br><sub>+1.8 MiB</sub> |
| register_functions | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.2 MiB**<br><sub>+2.1 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **11.6 MiB**<br><sub>+7.4 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_functions | 1,000 | 4.2 MiB | **6.1 MiB**<br><sub>+1.9 MiB</sub> | **7.2 MiB**<br><sub>+3.0 MiB</sub> | **4.8 MiB**<br><sub>+0.6 MiB</sub> | **12.1 MiB**<br><sub>+7.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> |
| register_variables | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.1 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **11.5 MiB**<br><sub>+7.4 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_variables | 100 | 4.2 MiB | **5.1 MiB**<br><sub>+1.0 MiB</sub> | **6.1 MiB**<br><sub>+2.0 MiB</sub> | **4.6 MiB**<br><sub>+0.4 MiB</sub> | **11.6 MiB**<br><sub>+7.4 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |
| register_variables | 1,000 | 4.2 MiB | **5.4 MiB**<br><sub>+1.2 MiB</sub> | **6.2 MiB**<br><sub>+2.1 MiB</sub> | **4.9 MiB**<br><sub>+0.7 MiB</sub> | **11.7 MiB**<br><sub>+7.5 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> |
| call_by_name | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.9 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | **5.7 MiB**<br><sub>+1.5 MiB</sub> |
| call_by_name | 1,000 | 4.2 MiB | **7.3 MiB**<br><sub>+3.1 MiB</sub> | **8.3 MiB**<br><sub>+4.1 MiB</sub> | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **17.6 MiB**<br><sub>+13.4 MiB</sub> | **11.7 MiB**<br><sub>+7.5 MiB</sub> |
| call_by_name | 10,000 | 4.2 MiB | **28.8 MiB**<br><sub>+24.6 MiB</sub> | **29.9 MiB**<br><sub>+25.8 MiB</sub> | **10.9 MiB**<br><sub>+6.8 MiB</sub> | **82.2 MiB**<br><sub>+78.1 MiB</sub> | **55.4 MiB**<br><sub>+51.2 MiB</sub> |
| call_by_handle | 10 | 4.2 MiB | **5.1 MiB**<br><sub>+0.9 MiB</sub> | **6.0 MiB**<br><sub>+1.8 MiB</sub> | **4.5 MiB**<br><sub>+0.3 MiB</sub> | **10.1 MiB**<br><sub>+5.9 MiB</sub> | n/a |
| call_by_handle | 1,000 | 4.2 MiB | **7.3 MiB**<br><sub>+3.1 MiB</sub> | **8.4 MiB**<br><sub>+4.2 MiB</sub> | **5.0 MiB**<br><sub>+0.8 MiB</sub> | **17.7 MiB**<br><sub>+13.6 MiB</sub> | n/a |
| call_by_handle | 10,000 | 4.2 MiB | **28.8 MiB**<br><sub>+24.6 MiB</sub> | **30.9 MiB**<br><sub>+26.7 MiB</sub> | **11.5 MiB**<br><sub>+7.3 MiB</sub> | **82.2 MiB**<br><sub>+78.0 MiB</sub> | n/a |
| engine_reset | 100 | 4.2 MiB | **5.2 MiB**<br><sub>+1.0 MiB</sub> | **6.2 MiB**<br><sub>+2.0 MiB</sub> | **4.5 MiB**<br><sub>+0.4 MiB</sub> | **10.5 MiB**<br><sub>+6.4 MiB</sub> | **5.9 MiB**<br><sub>+1.7 MiB</sub> |

---
Generated by `runner/report.py` from [results.json](results.json).
