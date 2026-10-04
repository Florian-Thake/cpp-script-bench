# fib_recursive

**Group:** core. **Measures:** the overhead of function calls and recursion.

## Task

`bench( n )` returns the n-th Fibonacci number, computed with the naive double recursion:

```
fib( x ) = x                        if x < 2
fib( x ) = fib( x - 1 ) + fib( x - 2 )  otherwise
```

- Default `n`: 25. The checksum is `fib( n )` (75025 for n = 25).
- All values fit into 32 bit signed integers for n <= 46.

## Rules

- Use exactly this recursion with `x < 2` as the condition, and an if/else (or the language's equivalent).
- No memoization, no iteration, no closed formula.
- `fib` is a normal named (global or module level) function, called by its name.
- `bench( n )` only calls `fib( n )` and returns the result.
