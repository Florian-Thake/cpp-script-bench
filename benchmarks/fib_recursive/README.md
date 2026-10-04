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

## Notes per language

**ChaiScript**
- `return` is implemented with a C++ exception (`detail::Return_Value`), unless it is the last
  statement of a function (then the optimizer removes it). Exceptions are expensive, especially
  with MSVC. The script therefore uses the value of the last expression, like the TeaScript script.
- The cost per call grows with the number of calls made so far within one call from the host:
  every call inserts its parameters at the *front* of a vector which is only cleared when the
  outermost call returns (`save_function_params()` / `pop_function_call()` in
  `dispatchkit/dispatchkit.hpp`). This makes the run time quadratic in the number of calls.
  Measured with MSVC: fib(15) 2.4 ms, fib(20) 152 ms, fib(25) 18.0 s (11x more calls cost 118x
  more time). ChaiScript 6.1.0 behaves the same.
