# tak

**Group:** core. **Measures:** recursive calls with 3 arguments, where the arguments are
themselves calls.

## Task

`bench( n )` returns the result of the Takeuchi function (in the form of the Gabriel benchmarks)
for `x = 3n`, `y = 2n`, `z = n`:

```
tak( x, y, z ) = tak( tak( x - 1, y, z ), tak( y - 1, z, x ), tak( z - 1, x, y ) )   if y < x
tak( x, y, z ) = z                                                                    otherwise

bench( n ) = tak( 3 * n, 2 * n, n )
```

- Series: n = 4, 5 and 6. The checksum is the result: 5, 10 and 7. The number of calls of `tak`:
  1,733, 10,345 and 63,609.
- All values are small (below 3n).

## Rules

- Use exactly this recursion with `y < x` as the condition, and an if/else (or the language's
  equivalent). The three inner calls are the arguments of the outer call.
- No memoization, no iteration, no shortcut (e.g. the closed form of the result).
- `tak` is a normal named (global or module level) function, called by its name.
- `bench( n )` only calls `tak( 3 * n, 2 * n, n )` and returns the result.

## Notes per language

**AngelScript**
- Statically typed: `int tak( int x, int y, int z )` and `int bench( int n )`.

**Lua**
- `tak` is a `local function`, like `fib` in `fib_recursive`: the recursive calls reach it through
  an upvalue. `bench` is global, so the host can find it.

**Python**
- `tak` is a module-level function, like `fib` in `fib_recursive`.

**ChaiScript**
- No `return`, the value of the last expression is the return value (see
  `fib_recursive/README.md`).
- The cost per call grows with the number of calls made so far within one recursion, like in
  `fib_recursive`. Measured with MSVC: n = 4 3.6 ms, n = 5 93 ms, n = 6 3.6 s per call (6x more
  calls cost 26x and 38x more time). This limits the series: n = 7 (395,757 calls) would take
  minutes per call. The other languages need at most ~50 ms for n = 6.
