# var_nested

**Group:** data. **Measures:** reads of variables of enclosing scopes: four nested loops, whose
innermost body reads the loop variables of all four loops and the accumulator of the function.

## Task

`bench( n )` runs four nested counting loops over `a`, `b`, `c` and `d`, each from 0 to n-1:

```
sum = 0
for a = 0 .. n-1:
    for b = 0 .. n-1:
        for c = 0 .. n-1:
            for d = 0 .. n-1:
                sum = sum + ((a + b + c + d) & 7)
return sum
```

- Default `n`: 25, i.e. n^4 = 390,625 iterations of the innermost body. The checksum is the sum
  (1,367,184 for n = 25).
- All values fit into 32 bit signed integers for n <= 157.
- The innermost body reads `a`, `b` and `c` from the enclosing loops and `sum` from the function
  scope. This shows whether the cost of a variable access grows with the scope depth: TeaScript and
  ChaiScript look names up at runtime, through the scopes from the innermost outwards; AngelScript,
  Lua and Python resolve them when the script is compiled.
- Compared to `loop`, the body has three more additions, and the loops have their own overhead: the
  innermost loop starts n^3 times.

## Rules

- Use the language's natural counting loop for all four loops, like in `loop`. Each loop variable
  belongs to its loop (the loop's scope, where the language has block scopes).
- Write the body exactly as `sum = sum + ((a + b + c + d) & 7)`. Plain assignments, no compound
  assignment, no partial sums hoisted out of the inner loops.
- `sum` is a local variable of `bench`.

## Notes per language

- TeaScript: four `forall` loops over `_seq`. Every loop has its own scope; a name is looked up from
  the innermost scope outwards.
- AngelScript: every variable has its own slot on the stack, found by the compiler.
- Lua: all variables are registers of `bench`, found by the compiler.
- Python: all variables are fast locals of `bench` (Python has no block scopes). Every inner `for`
  creates a new `range` object, which is the natural form. The variable is called `total`, like in
  `loop`.
