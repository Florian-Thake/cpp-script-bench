# cpp_objects

**Group:** interop. **Measures:** a C++ type used from the script: creating an object through the
binding, reading and writing its fields.

## Task

The host binds the C++ struct `Vec3i` (`host.hpp`: the `int` fields `x`, `y` and `z`, all 0 after the
default constructor). `bench( n )` runs the loop of `records` on one `Vec3i`:

```
p = Vec3i()                        (C++ type)
for i = 0 .. n-1:
    p.x = p.x + (i & 7)
    p.y = p.y + (p.x & 3)
    p.z = p.z + (p.y & 1)
return p.x + p.y + p.z
```

- Default `n`: 150,000, like `records`, with the same checksum (787,500), so the results compare
  directly: a record of the script (`records`) and a bound C++ object (`cpp_objects`).
- All values fit into 32 bit signed integers for n <= 300,000,000.
- Per iteration: 5 field reads and 3 field writes. The object is created once per call.

## Rules

- The host binds the type in the `bind` phase with the language's normal way of binding a C++ class
  or struct: a default constructor and the three fields as read-write properties. No methods.
- The script creates the object with the bound constructor and accesses the fields with the dot
  operator, exactly as in `records`.

## Binding per language

| Language    | Headline                                   | Variant                                   |
|-------------|--------------------------------------------|-------------------------------------------|
| TeaScript   | n/a: TeaScript cannot bind C++ types (PLAN.md, decision log 2026-10-25) | |
| ChaiScript  | `user_type<Vec3i>()`, `constructor<Vec3i()>()`, `fun( &Vec3i::x )` per field | |
| AngelScript | a value type (flags `asOBJ_VALUE` and `asOBJ_POD`) with a constructor behaviour and `RegisterObjectProperty` at the field offsets | |
| Lua         | LuaBridge3 `beginClass<Vec3i>`, `addConstructor`, `addProperty( "x", &Vec3i::x, &Vec3i::x )` per field | `c-api`: a full userdata with a metatable; `__index` and `__newindex` find the field with `luaL_checkoption` |
| Python      | pybind11 `py::class_<Vec3i>`, `py::init<>()`, `def_readwrite` per field | `c-api`: a type from `PyType_FromSpec` with a `PyMemberDef` (`T_INT`) per field |

## Notes per language

- C++: the reference runs the loop on a `Vec3i` directly.
- ChaiScript: no `return`, see `fib_recursive/README.md`.
- AngelScript: a value type is AngelScript's form for a small struct without identity: it lives on the
  script stack like the script class of `records`, and the properties are accessed at their offsets,
  without a call.
- Lua: `local Vec3i = host.Vec3i` in the top level, like the functions in `script_to_host`. In the
  `c-api` variant every field access is a call of a C function, which checks the userdata
  (`luaL_checkudata`) and finds the field name (`luaL_checkoption`), the usual form of the C API.
- Python: `from host import Vec3i` in the top level. In the `c-api` variant the fields are member
  descriptors, which CPython reads and writes directly (like `__slots__`); with pybind11 every access
  is a call of a bound C++ getter or setter.

## Measurements (per call of `bench`, n = 150,000)

| Language    | MSVC: `records` | MSVC: headline | MSVC: `c-api` | GCC 11: `records` | GCC 11: headline | GCC 11: `c-api` |
|-------------|-----------------|----------------|---------------|-------------------|------------------|-----------------|
| ChaiScript `no-threads` | 433 ms | 460-470 ms |      | 211 ms            | 198 ms           |                 |
| AngelScript `no-threads-no-line-cues` | 5.0 ms | 5.1 ms | | 2.7 ms     | 2.5 ms           |                 |
| Lua         | 4.9 ms          | 106 ms (LuaBridge3) | 59.5 ms  | 5.2 ms            | 82.9 ms (LuaBridge3) | 46.4 ms     |
| Python      | 13.5 ms         | 257 ms (pybind11) | 31 ms      | 34.8 ms           | 124 ms (pybind11) | 37.6 ms        |

Python: 3.14 with MSVC, 3.10 with GCC 11 (WSL).
