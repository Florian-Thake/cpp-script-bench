# sieve

**Group:** data. **Measures:** an array of flags: appending elements, then reading and writing
elements by index.

## Task

`bench( n )` builds an array of n + 1 flags by appending, crosses out the multiples of every prime
with the sieve of Eratosthenes, and counts the primes up to n:

```
flags = empty array (with a capacity of n + 1 where the language can reserve one)
for k = 0 .. n:
    append true to flags
i = 2
while i * i <= n:
    if flags[i]:
        for j = i * i .. n, step i:
            flags[j] = false
    i = i + 1
count = 0
for k = 2 .. n:
    if flags[k]:
        count = count + 1
return count
```

- Default `n`: 40,000. The checksum is the number of primes up to n (4,203 for n = 40,000).
- All values fit into 32 bit signed integers for n <= 2,000,000,000 (the last `i * i` and `j`
  exceed n by at most ~2 times the square root of n).
- The work: n + 1 appends, ~1.8 n writes for crossing out (73,758 for n = 40,000), and n - 1 reads
  for counting. The outer loop only runs up to the square root of n, so its form plays no role.

## Rules

- The array is the language's natural array type (see below), and it grows by appending in a loop.
  No native fill or preallocated size (e.g. `[True] * (n + 1)`, `array<bool>( n + 1, true )`,
  `resize`), no slice assignment and no native counting (e.g. `sum()`, `count()`), see rule 4 in
  PLAN.md.
- A capacity is reserved before the appends where the language can reserve one without changing the
  size: AngelScript `reserve`, ChaiScript `reserve`, Lua `table.create`. Reserving capacity is the
  natural form when the final size is known; the appends are still made one by one in the script.
  TeaScript (Tuple) and Python (list) have no such function.
- Use the language's natural counting loops; the crossing-out loop uses the step `i`. The outer loop
  is a `while` loop with the condition `i * i <= n` (or the language's equivalent), no square root.
- Plain assignments, no compound assignment. All variables are local variables of `bench`.

## Notes per language

**TeaScript**
- A Tuple, created with `_tuple_create()`, grown with `_tuple_append`, and read and written with the
  subscript operator `flags[i]`. TeaScript has no way to reserve the capacity of a Tuple in the script;
  the script will use it if TeaScript gets one.
- The outer loop is `repeat` with `if( i * i > n ) { stop }`; TeaScript has no `while`.

**ChaiScript**
- A Vector (`[]`), grown with `push_back`. For a Vector, `push_back` is a function written in
  ChaiScript which clones the value and then calls the native `push_back_ref`. This makes it the
  dominant cost: ~9 us per call with MSVC. `reserve` changes nothing measurable, the Vector grows
  geometrically anyway.
- The size n = 40,000 is limited by ChaiScript: its headline needs ~0.47 s per call with MSVC.

**AngelScript**
- `array<bool>`, grown with `insertLast`. `reserve( n + 1 )` is essential here: without it, the array
  add-on grows the buffer by exactly one element per `insertLast` (a new allocation and a copy of the
  whole array each time), so building the array is quadratic in n. Measured with MSVC (`no-threads`,
  appends only): 15 / 64 / 268 / 1,470 ms for n = 37,500 / 75,000 / 150,000 / 300,000; the whole
  sieve with `reserve` needs ~22 ms for n = 300,000.
- `for( int i = 2; i * i <= n; ++i )` is the natural form of the outer loop in a language with a
  C-style `for`; ChaiScript uses the same.

**Lua**
- A table used as a sequence, created with `table.create( n + 1 )` (Lua 5.5), which reserves its
  array part. `flags[k] = true` with the next index is Lua's append.
- Lua indexes from 1: the flag of the number m has the index m + 1. The loops run over the shifted
  index ranges, so there is no extra addition per access.

**Python**
- A list, grown with `flags.append( True )`. Caching the bound method in a local
  (`append = flags.append`) made no measurable difference with Python 3.14, which specializes
  `list.append` calls, so the script uses the plain form.
