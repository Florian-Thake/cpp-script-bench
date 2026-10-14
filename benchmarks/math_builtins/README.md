# math_builtins

**Group:** core. **Measures:** calls of the language's standard math functions `sqrt` and
`floor`, and the conversion between integers and floating point numbers.

## Task

`bench( n )` sums up the integer square roots of `i = 1 .. n`, computed with `floor( sqrt( i ) )`:

```
total = 0
for i = 1 .. n:
    total = total + floor( sqrt( i ) )
return total (as an integer)
```

- Default `n`: 100,000. The checksum is `total` (21,032,170 for n = 100,000).
- All values fit into 32 bit signed integers for n < 2,181,864.
- The checksum is exact, no tolerance is needed: `sqrt` is correctly rounded (IEEE 754), and for
  integers far below 2^52 the rounded square root of a non-square never reaches the next integer,
  so `floor` gives the exact integer square root. Every partial sum is an integer below 2^53, so a
  double holds it exactly.

## Rules

- Use the language's standard `sqrt` and `floor` (its standard library, or the official math
  module or add-on where the standard library has none). No integer square root function
  (e.g. `math.isqrt`), no caching, no other shortcut.
- `total` has the type the functions return: a double, or an integer where `floor` returns one
  (Lua, Python). If it is a double, it is converted to an integer once, at the end.
- Where the language does not convert the integer `i` for `sqrt` by itself, the script converts it
  in its fastest usual form (see ChaiScript).
- Use the language's natural counting loop for `i`. Plain assignments, no compound assignment.
- `total` is a local variable of `bench`.

## Notes per language

**TeaScript**
- `sqrt` and `floor` are the convenience functions of the Core Library, the ones a TeaScript user
  calls. They are TeaScript functions on top of the native `_sqrt` and `_trunc`: `sqrt( val )` is
  `_sqrt( val + 0.0 )`, `floor` uses `_trunc` plus comparisons. Both return a `f64`.
  `total as i64` converts the result at the end.

**ChaiScript**
- Its standard library has no math functions. `sqrt` and `floor` come from the math module of
  ChaiScript_Extras (`math.hpp`), ChaiScript's official module for `<cmath>`. The runner loads it
  for every benchmark, like AngelScript's math add-on, so it adds to ChaiScript's startup
  (~0.6 ms with MSVC).
- The functions are registered for `float`, `double` and `long double`. ChaiScript does not
  convert an `int` argument for them: `sqrt(i)` fails with "Error with function dispatch". The
  script writes `sqrt(i + 0.0)`, the fastest of the usual forms. Measured with MSVC (`no-threads`,
  per call): `sqrt(to_double(i))` 161 ms, `sqrt(double(i))` 155 ms, `sqrt(i + 0.0)` 122 ms.
  TeaScript's `sqrt` does the same `+ 0.0` internally.
- `to_int(total)` converts the result at the end.

**AngelScript**
- The math add-on (scriptmath) provides `sqrt` and `floor`. By default it registers only float
  versions of its functions (`float sqrt(float)`), which would compute in single precision (and
  a float sum can't even hold the checksum exactly). The runner builds it with `AS_USE_FLOAT=0`,
  the switch the add-on's documentation names for the double versions.
- `sqrt( i )` converts the `int` implicitly to `double`. `int( total )` converts the result at the
  end.

**Lua**
- `local sqrt, floor = math.sqrt, math.floor` at the top of the script, the idiomatic fast form:
  `bench` reaches the functions as upvalues, without a lookup in the table `math` on every call.
- `math.floor` returns an integer when the result fits (Lua 5.3 and newer), so `total` is an
  integer and needs no conversion.

**Python**
- `from math import sqrt, floor`, the idiomatic fast form: no attribute lookup in the module `math`
  on every call. `math` is a built-in module in the tested builds (Windows, Ubuntu), so the import
  during load reads nothing from disk.
- `math.floor` returns an `int`, so `total` is an `int` and needs no conversion.
