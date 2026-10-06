# loop

**Group:** core. **Measures:** the overhead of a counting loop with a minimal body.

## Task

`bench( n )` loops over `i = 0 .. n-1` and sums up `i & 7` (bitwise and):

```
sum = 0
for i = 0 .. n-1:
    sum = sum + (i & 7)
return sum
```

- Default `n`: 1,000,000. The checksum is the sum (3,500,000 for n = 1,000,000).
- All values fit into 32 bit signed integers for n <= 300,000,000.

## Rules

- Use the language's natural counting loop. Examples: `for( var i = 0; i < n; ++i )` in ChaiScript,
  `for( int i = 0; i < n; ++i )` in AngelScript, `forall( i in _seq( 0, n - 1, 1 ) )` in TeaScript,
  `for i = 0, n - 1 do` in Lua, `for i in range( n )` in Python.
- Write the body as `sum = sum + (i & 7)` (or the language's operator names); no compound assignment
  like `+=`, so all languages do the same operations.
- `sum` is a local variable of `bench`.

## Notes per language

- Python: the variable is called `total`, because a local `sum` would hide the built-in function
  `sum()`, which idiomatic Python avoids. The name has no effect on the speed.
