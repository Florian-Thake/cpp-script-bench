# calls

**Group:** core. **Measures:** the overhead of calls of tiny script functions with 0, 1 and 3
arguments, made from a loop (no recursion).

## Task

`bench( n )` runs the loop of the `loop` benchmark, but its body calls three tiny functions:

```
f0()          = 1
f1( a )       = a & 7
f3( a, b, c ) = (a + b + c) & 7

sum = 0
for i = 0 .. n-1:
    sum = sum + f0() + f1( i ) + f3( i, 1, 2 )
return sum
```

- Default `n`: 100,000, i.e. 300,000 calls. The checksum is the sum (800,000 for n = 100,000;
  every 8 consecutive values of `i` add 64).
- All values fit into 32 bit signed integers for n <= 260,000,000.

## Rules

- `f0`, `f1` and `f3` are normal named (global or module level) functions, called by their names,
  like `fib` in `fib_recursive`. No lambdas, closures or function objects.
- Use the language's natural counting loop, like in `loop`.
- Write the body exactly as `sum = sum + f0() + f1( i ) + f3( i, 1, 2 )`: one expression with all
  three calls, no compound assignment, no inlining by hand.
- `sum` is a local variable of `bench`.

## Notes per language

- C++: the reference uses normal functions, which the compiler inlines, as it would in any C++
  program. The script engines make every call.
- ChaiScript: the functions use the value of their last expression instead of `return`, see
  `fib_recursive/README.md`. Its cost per call grows only within a recursion; the calls here are
  made directly from `bench`, so their cost stays constant.
- Lua: the functions are `local function`, like `fib` in `fib_recursive`, so `bench` reaches them
  through upvalues. `bench` is global, so the host can find it.
- Python: the variable is called `total`, like in `loop` (a local `sum` would hide the built-in
  function `sum()`). The functions are looked up in the module namespace on every call.
