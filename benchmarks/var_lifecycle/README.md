# var_lifecycle

**Group:** data. **Measures:** the cost of block-local variables: every loop iteration defines two
new variables in the loop body, which end with the iteration.

## Task

`bench( n )` runs a counting loop. Its body defines two new variables `a` and `b`:

```
sum = 0
for i = 0 .. n-1:
    a = i & 7           (new variable)
    b = a + 1           (new variable)
    b = b * a
    sum = sum + b
return sum
```

- Default `n`: 300,000. The checksum is the sum (6,300,000 for n = 300,000; every 8 consecutive
  values of `i` add 168).
- All values fit into 32 bit signed integers for n <= 100,000,000.
- TeaScript and ChaiScript create a named variable at its definition and remove it when the
  iteration ends. AngelScript and Lua give each variable a fixed slot when the script is compiled,
  so a definition costs about as much as an assignment. Python has no block scopes: `a` and `b` are
  local variables of the function, and their definitions are assignments.

## Rules

- Define `a` and `b` inside the loop body, with the language's form for a new local variable, where
  the language has block scopes. `b` is assigned again after its definition.
- No explicit removal of the variables (TeaScript `undef`, Python `del`): only these two languages
  have it, and removing local variables is unusual in both.
- Use the language's natural counting loop, like in `loop`. Plain assignments, no compound
  assignment.
- `sum` is a local variable of `bench`.

## Notes per language

- TeaScript: `a` is defined with `const` and `b` with `def`. A value which doesn't change is usually
  defined with `const` in TeaScript (like `k` in `branching`). The other languages use their normal
  form of a local variable: `var` (ChaiScript), `int` (AngelScript), `local` (Lua).
- Python: the variable is called `total`, like in `loop`.
