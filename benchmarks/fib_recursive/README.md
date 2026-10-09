# fib_recursive

**Group:** core. **Measures:** the overhead of function calls and recursion.

## Task

`bench( n )` returns the n-th Fibonacci number, computed with the naive double recursion:

```
fib( x ) = x                        if x < 2
fib( x ) = fib( x - 1 ) + fib( x - 2 )  otherwise
```

- Series: n = 15, 20 and 25. The checksum is `fib( n )`: 610, 6765 and 75025.
- All values fit into 32 bit signed integers for n <= 46.

## Rules

- Use exactly this recursion with `x < 2` as the condition, and an if/else (or the language's equivalent).
- No memoization, no iteration, no closed formula.
- `fib` is a normal named (global or module level) function, called by its name.
- `bench( n )` only calls `fib( n )` and returns the result.

## Notes per language

**AngelScript**
- Statically typed: `int fib( int x )` and `int bench( int n )`. `int` is 32 bit.

**Lua**
- `fib` is a `local function` at the top of the script, the idiomatic form for a module-level
  function in Lua. The recursive calls then reach `fib` through an upvalue instead of a lookup in
  the global table. `bench` is global, so the host can find it.

**Python**
- `fib` is a module-level function. The recursive calls look it up in the module namespace, which
  Python 3.11 and newer speed up with a specialized instruction.

**ChaiScript**
- `return` is implemented with a C++ exception (`detail::Return_Value`), unless it is the last
  statement of a function (then the optimizer removes it). Exceptions are expensive, especially
  with MSVC. The script therefore uses the value of the last expression, like the TeaScript script.
- The cost per call grows with the number of calls made so far within one outermost script call
  (here `fib( n )`, called from `bench`): every call inserts its parameters at the *front* of a
  vector which is only cleared when the script call depth returns to 0 (`save_function_params()` /
  `pop_function_call()` in `dispatchkit/dispatchkit.hpp`). This makes the run time of a recursion
  quadratic in its number of calls. Calls made directly from `bench` are not affected.
  Measured with MSVC: fib(15) 2.4 ms, fib(20) 152 ms, fib(25) 18.0 s (11x more calls cost 118x
  more time). ChaiScript 6.1.0 behaves the same. The series n = 15 / 20 / 25 shows this growth.
