# strings

**Group:** data. **Measures:** the common string operations of a script: number -> string with
concatenation, comparison, length, substring, and string -> number.

## Task

`bench( n )` creates one short string per iteration and runs every operation on it once:

```
sum = 0
prev = ""
for i = 0 .. n-1:
    s = "item" + i                 (number -> string, concatenation)
    if s < prev:                   (comparison, lexicographic)
        sum = sum + 1
    sum = sum + length( s )        (length)
    d = substring( s, from 4 )     (substring: the digits)
    sum = sum + (int( d ) & 7)     (string -> number)
    prev = s
return sum
```

- Default `n`: 70,000. The checksum is the sum (863,894 for n = 70,000).
- All values fit into 32 bit signed integers for n <= 100,000,000.
- The strings are ASCII, so the length in bytes and in characters are the same.

## Rules

- Each operation in the language's natural form (see the table). Number -> string as part of the
  concatenation where the language converts implicitly.
- `prev = s` is a plain assignment of the string (a copy or a shared reference, whatever the language
  does for strings).
- Use the language's natural counting loop, like in `loop`. Plain assignments, no compound
  assignment. All variables are local variables of `bench`; where the language has block scopes,
  `s` and `d` are locals of the loop body.

## Notes per language

| Language    | number -> string        | length         | substring from 4         | string -> number |
|-------------|-------------------------|----------------|--------------------------|------------------|
| TeaScript   | `"item" % i`            | `_strlen( s )` | `_substr( s, 4, -1 )`    | `d as i64`       |
| ChaiScript  | `"item" + to_string(i)` | `s.size()`     | `s.substr(4, len - 4)`   | `to_int(d)`      |
| AngelScript | `"item" + i`            | `s.length()`   | `s.substr( 4 )`          | `parseInt( d )`  |
| Lua         | `"item" .. i`           | `#s`           | `sub(s, 5)`              | `tonumber(d)`    |
| Python      | `f"item{i}"`            | `len(s)`       | `s[4:]`                  | `int(d)`         |

All languages compare strings with `<`.

**TeaScript**
- `%` is TeaScript's operator for string concatenation, and converts the number. `_strlen` returns the
  length in bytes. `_substr( s, 4, -1 )` returns the rest of the string from index 4.
- String -> number with the cast operator `as i64`. `_strtonum( d )` ends in the same conversion.
- `s` and `d` are `const`.

**ChaiScript**
- `substr` needs a length, so the script keeps the length of `s` in a local variable `len`, which it
  also adds to the sum. Calling `s.size()` twice instead was ~4% slower (MSVC).
- `"item${i}"` (string interpolation) was ~7% slower than `"item" + to_string(i)` (MSVC).
- The size n = 70,000 is limited by ChaiScript: its headline needs ~0.43 s per call with MSVC.

**AngelScript**
- `"item" + i` is the string add-on's concatenation with an `int64`, which converts with an
  `ostringstream`. `"item" + formatInt( i )` would be ~30% faster for the whole call with MSVC, but
  ~12% slower with GCC, so the script keeps the natural form.
- `int( s.length() )` and `int( parseInt( d ) )` convert explicitly from `uint` and `int64`.

**Lua**
- `local sub, tonumber = string.sub, tonumber` at the top of the script, the idiomatic fast form, like
  in `math_builtins`.
- Lua indexes strings from 1: the substring from the index 4 of the spec is `sub(s, 5)`.

**Python**
- `"item" + str(i)` has the same speed as the f-string with Python 3.14.
- The variable is called `total`, like in `loop`.
