# records

**Group:** data. **Measures:** reads and writes of the fields of a record (a structured value with
named fields), without allocations in the loop.

## Task

`bench( n )` creates one record with the fields `x`, `y` and `z`, all 0, and updates its fields in a
loop, each from the field before:

```
p = record { x = 0, y = 0, z = 0 }
for i = 0 .. n-1:
    p.x = p.x + (i & 7)
    p.y = p.y + (p.x & 3)
    p.z = p.z + (p.y & 1)
return p.x + p.y + p.z
```

- Default `n`: 150,000. The checksum is x + y + z (787,500 for n = 150,000).
- All values fit into 32 bit signed integers for n <= 300,000,000.
- Per iteration: 5 field reads and 3 field writes. The record is created once per call, so its
  creation plays no role; allocation is measured by `binary_trees`.

## Rules

- The record is the language's natural type for a structured value with named fields (see below),
  accessed with the dot operator.
- No methods: TeaScript has no classes and no automatic `this`, so the fields are read and written
  in `bench` itself.
- Use the language's natural counting loop, like in `loop`. Plain assignments, no compound
  assignment. `p` is a local variable of `bench`.

## Notes per language

**TeaScript**
- A named Tuple: `_tuple_create()`, then `def p.x := 0` etc. adds the named elements. TeaScript has
  no literal for a named Tuple.

**ChaiScript**
- A `class Point` with the attributes `x`, `y` and `z` (`var`) and a constructor which sets them to
  0. It creates a `Dynamic_Object`, whose attributes are found by name.

**AngelScript**
- A script `class Point` with three `int` members, initialized in their declarations.

**Lua**
- A table with the fields `x`, `y` and `z`, created with a table constructor.

**Python**
- A class `Point` with `__init__`, which sets the three attributes. `__slots__` was 0-5% faster
  with Python 3.14 (MSVC), close to the noise; the script uses the plain class, the usual form.
