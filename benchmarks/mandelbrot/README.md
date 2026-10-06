# mandelbrot

**Group:** core. **Measures:** floating point arithmetic in nested loops, plus a loop with a
two-part condition.

## Task

`bench( n )` computes the Mandelbrot set on an `n` x `n` grid over the area
x = -2.0 .. 0.5 and y = -1.25 .. 1.25, with at most 50 iterations per point.
It returns the total number of iterations of all points.

```
max_iter = 50
count = 0
for y = 0 .. n-1:
    ci = -1.25 + 2.5 * y / n
    for x = 0 .. n-1:
        cr = -2.0 + 2.5 * x / n
        zr = 0.0
        zi = 0.0
        k  = 0
        while k < max_iter and zr * zr + zi * zi <= 4.0:
            t  = zr * zr - zi * zi + cr
            zi = 2.0 * zr * zi + ci
            zr = t
            k  = k + 1
        count = count + k
return count
```

- Default `n`: 100. The checksum is `count`.
- All values fit into 32 bit signed integers for n <= 6,000.

## Rules

- All operations happen in this order, in IEEE 754 double precision. `2.5 * y / n` means
  `(2.5 * y) / n`. The checksum is an integer, so it only matches if every language rounds the same.
- Use the language's natural counting loops for `y` and `x`, and a `while` loop (or the language's
  equivalent) for `k`. Check `k < max_iter` first. Languages without `while` (TeaScript) use their
  endless loop with the negated condition as exit: `if( k >= max_iter or zr * zr + zi * zi > 4.0 ) { stop }`.
- Use plain assignments like `k = k + 1`, no compound assignment.
- All variables are locals of `bench`.

## Notes per language

- `max_iter` is a constant where the language has constants: `const` in TeaScript,
  `const int` in AngelScript, `local max_iter <const>` in Lua (a compile-time constant, so the
  comparison `k < max_iter` uses an immediate operand). ChaiScript has no constant locals.
- AngelScript: statically typed, `int` for the counters and `double` for the coordinates.
