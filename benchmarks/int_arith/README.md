# int_arith

**Group:** core. **Measures:** integer arithmetic with modulo and division, in a loop with a data
dependent number of iterations.

## Task

`bench( n )` counts the steps of the Collatz sequences of all start values `s = 1 .. n`, i.e. how
often `x` must be halved (if even) or replaced by `3 * x + 1` (if odd) until it reaches 1:

```
total = 0
for s = 1 .. n:
    x = s
    while x != 1:
        if x % 2 == 0:
            x = x / 2          (integer division)
        else:
            x = 3 * x + 1
        total = total + 1
return total
```

- Default `n`: 5,000. The checksum is `total` (387,968 for n = 5,000; 849,666 for n = 10,000).
- All values fit into 32 bit signed integers for n < 113,383 (the largest `x` for n = 113,382 is
  1,570,824,736). The sequence of 113,383 exceeds 2^31.

## Rules

- Use the language's natural counting loop for `s`, and a `while` loop (or the language's
  equivalent) for `x`. Languages without `while` (TeaScript) use their endless loop with the negated
  condition as exit: `if( x == 1 ) { stop }`.
- Use the modulo operator and integer division, no bit operations (`x & 1`, `x >> 1`): the `loop`
  benchmark covers those. An engine may still optimize them internally.
- Use plain assignments like `x = x / 2`, no compound assignment.
- All variables are locals of `bench`.

## Notes per language

- Operators: `%` and `/` in AngelScript and ChaiScript (`/` of two integers is the integer
  division), `mod` and `/` in TeaScript (likewise), `%` and `//` in Lua and Python. `/` in Lua and
  Python always gives a float, `//` is their integer (floor) division. `x` is always positive, so
  floor division and truncating division give the same result.
