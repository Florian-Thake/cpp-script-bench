# branching

**Group:** core. **Measures:** conditional branches: an if/else chain and a condition with
short-circuit logic, in a loop.

## Task

`bench( n )` runs a counting loop. Each iteration takes `k = i & 7` through an if/else chain with
four branches, then through a condition with `and` and `or`:

```
sum = 0
for i = 0 .. n-1:
    k = i & 7
    if k == 0:
        sum = sum + 1
    else if k == 1:
        sum = sum + 2
    else if k < 4:
        sum = sum + 3
    else:
        sum = sum + 4
    if (k > 2 and k < 6) or k == 7:
        sum = sum + 5
return sum
```

- Default `n`: 300,000. The checksum is the sum (1,687,500 for n = 300,000; every 8 consecutive
  values of `i` add 45).
- All values fit into 32 bit signed integers for n <= 380,000,000.

## Rules

- Use the language's natural if/else chain (`else if`, `elif`, `elseif`), no switch or match:
  TeaScript and Lua have none, and a jump table would measure something else.
- Write the second condition exactly as `(k > 2 and k < 6) or k == 7`, with the language's logical
  operators (`and`/`or` or `&&`/`||`), which evaluate short-circuit in every language. The
  parentheses are explicit, so operator precedence plays no role.
- Use the language's natural counting loop, like in `loop`. Plain assignments like
  `sum = sum + 1`, no compound assignment.
- `sum` and `k` are local variables of `bench`; `k` is a local of the loop body where the language
  has block scopes.

## Notes per language

- `k` is `const` in TeaScript, where a value that doesn't change is usually defined with `const`
  (like `ci` and `cr` in `mandelbrot`). The other scripts use a normal local variable.
- Python: written with `and`, not as the chained comparison `2 < k < 6`, so every language evaluates
  the same comparisons. The chained form is slower in CPython: ~9% for this loop with Python 3.14,
  because it needs extra stack operations to keep `k`. The variable is called `total`, like in
  `loop`.
