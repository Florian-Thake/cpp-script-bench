# sort

**Group:** data. **Measures:** reads, compares and writes of array elements by index: an
insertion sort written in the script.

## Task

`bench( n )` builds an array of n pseudo-random values by appending, sorts it in place with an
insertion sort, and returns a checksum which depends on the order of the elements:

```
a = empty array (with a capacity of n where the language can reserve one)
x = 1
for k = 0 .. n-1:
    x = (x * 75 + 74) % 65537
    append x to a
for i = 1 .. n-1:
    v = a[i]
    j = i - 1
    while j >= 0 and a[j] > v:
        a[j + 1] = a[j]
        j = j - 1
    a[j + 1] = v
sum = 0
for i = 0 .. n-1:
    sum = sum + a[i] * (i & 7)
return sum
```

- Default `n`: 1,000. The sort shifts 255,665 elements. The checksum is the sum (116,225,934 for
  n = 1,000).
- The generator is a simple linear congruential generator with the modulus 65,537. Its values are
  below 65,537, and `x * 75 + 74` stays below 2^23.
- All values fit into 32 bit signed integers for n <= 4,681 (every value is below 2^16, every factor
  `i & 7` at most 7).
- The run time grows with n^2 (the number of shifts).

## Rules

- The array is the language's natural array type and grows by appending, with a capacity reserved
  where the language can, like in `sieve`.
- The sort is written in the script exactly as above. No native sort (`sorted()`, `table.sort`,
  `sortAsc()`, ...) and no native insert or slice operations, see rule 4 in PLAN.md.
- The inner loop is the language's `while` loop (TeaScript: `repeat` with `stop`). Its condition
  `j >= 0 and a[j] > v` evaluates short-circuit, so `a[j]` is never read with j = -1.
- Plain assignments, no compound assignment. All variables are local variables of `bench`.

## Notes per language

**TeaScript**
- A Tuple with `_tuple_reserve( a, n )`, grown with `_tuple_append`, read and written with the
  subscript operator `a[j]`.
- `_tuple_append` shares the value object of its argument with the new element. Appending the
  variable `x` itself would make all elements the same variable, so the script appends a copy
  (`def v := x`). Other forms of a copy (`x + 0`, or appending the expression and reading `x` back
  from the Tuple) have the same speed: this loop runs only n times. Defining `v` once before the loop
  and assigning `v := x` in it does not work: the assignment writes into the value object which the
  previously appended element shares, so all elements get the last value.
- The inner loop is `repeat` with `if( j < 0 or a[j] <= v ) { stop }`; TeaScript has no `while`.
- `v` is `const`: a copy of `a[i]`, which doesn't change.

**ChaiScript**
- A Vector with `reserve( n )`, grown with `push_back`.

**AngelScript**
- `array<int>` with `reserve( n )`, grown with `insertLast` (see `sieve` for why `reserve` matters).

**Lua**
- A table created with `table.create( n )`; `a[k] = x` with the next index appends.
- Lua indexes from 1: the element with the index i of the spec has the index i + 1, so the inner
  loop stops at `j >= 1`. The checksum loop runs over i = 0 .. n-1 and reads `a[i + 1]`, so that
  `i & 7` is the same factor as in the other languages.

**Python**
- A list, grown with `a.append( x )`.
- The check `j >= 0` is essential in Python: `a[-1]` would be the last element, not an error.
- The checksum variable is called `total`, like in `loop`.
