# var_global

**Group:** data. **Measures:** reads and writes of a script-level (global) variable from inside a
function.

## Task

`bench( n )` runs the loop of the `loop` benchmark, but the accumulator `sum` is a variable of the
script's top level instead of a local variable of `bench`:

```
sum = 0                         (script level)

bench( n ):
    sum = 0
    for i = 0 .. n-1:
        sum = sum + (i & 7)
    return sum
```

- Default `n`: 1,000,000, the same as in `loop`, so the two results can be compared directly: the
  difference is the cost of the global variable. The checksum is the sum (3,500,000 for
  n = 1,000,000).
- All values fit into 32 bit signed integers for n <= 300,000,000.

## Rules

- `sum` is defined at the top level of the script, in the language's form for a variable which
  functions can read and write. `bench` uses it directly in the loop, not through a local copy.
- `bench` sets `sum` to 0 first, so every call returns the same result.
- The loop variable `i` is local. Use the language's natural counting loop, like in `loop`.
- Plain assignments, no compound assignment.

## Notes per language

**TeaScript**
- `def sum := 0` at the top level. A function finds a name which is not defined in its own scopes
  in the top-level scope.

**ChaiScript**
- `global sum = 0;`. A `var` at the top level is not visible inside functions; `global` defines a
  variable which is.

**AngelScript**
- A global `int sum`. `Build()` initializes the global variables, so this happens in the load phase.

**Lua**
- `sum` without `local`: a global, i.e. a field of the global environment table `_ENV`, which is
  looked up by its name on every access.
- A `local sum` at the top level of the script would be an upvalue of `bench` (a local variable of
  the enclosing chunk), not a global. It is the usual form of module-level state in Lua, but it is
  not what this benchmark measures.

**Python**
- A module variable, declared with `global total` in `bench`, which is required for assigning it.
- The variable is called `total`, like in `loop` (`sum` would hide the built-in function `sum()`).
