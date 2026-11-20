# maps

**Group:** data. **Measures:** the language's map (dictionary) with string keys: insert, lookup,
remove and membership test.

## Task

`bench( n )` builds the keys "k0" .. "k{n-1}" once into an array, then runs the four operations
over all keys (removing only every second one):

```
keys = empty array (with a capacity of n where the language can reserve one)
for i = 0 .. n-1:
    append "k" + i to keys            (number -> string in the language's natural form)
m = empty map (with a capacity of n where the language can reserve one)
for i = 0 .. n-1:
    m[keys[i]] = i                    (insert)
sum = 0
for i = 0 .. n-1:
    sum = sum + m[keys[i]]            (lookup)
for i = 0 .. n-1, step 2:
    remove keys[i] from m             (remove)
for i = 0 .. n-1:
    if keys[i] is in m:               (membership)
        sum = sum + 1
return sum
```

- Default `n`: 50,000. The checksum is the sum of all looked-up values plus the number of remaining
  keys: n * (n - 1) / 2 + n / 2 for an even n (1,250,000,000 for n = 50,000).
- All values fit into 32 bit signed integers for n < 65,536.
- The keys are built before the map operations, so the conversions number -> string are made only
  once per key. They are part of the measured time (n conversions and appends, much less than the
  3.5 n map operations).

## Rules

- The map is the language's natural map type with string keys (see below). Maps use string keys
  only: ChaiScript's Map and AngelScript's dictionary have no integer keys, and Lua stores dense
  integer keys in the array part of a table, so an integer-key test would measure arrays there.
- Each operation uses the language's natural form for it: the subscript operator for insert and
  lookup where the language has one, and its function or operator for remove and membership. No
  bulk operations (e.g. `dict.update()`, a comprehension, building the map from a list of pairs).
- The keys array grows by appending, with a capacity reserved where possible, like in `sieve`. The
  map reserves its capacity too where the language can (only Lua can).
- All loops are the language's natural counting loop and read the keys by index, like in
  `byte_buffer` (see Python).
- Plain assignments, no compound assignment. All variables are local variables of `bench`.

## Notes per language

| Language    | Map type (implementation)                  | insert, lookup     | remove              | membership              | number -> string |
|-------------|--------------------------------------------|--------------------|---------------------|-------------------------|------------------|
| TeaScript   | Map (`std::map`, a tree)                   | `m[k] := i`, `m[k]` | `_map_remove( m, k )` | `_map_contains( m, k )` | `"k" % i` |
| ChaiScript  | Map (`std::map<std::string, Boxed_Value>`) | `m[k] = i`, `m[k]` | `m.erase( k )`      | `m.count( k ) > 0`      | `"k" + to_string( i )` |
| AngelScript | dictionary (`std::unordered_map`)          | `m[k] = i`, `int( m[k] )` | `m.delete( k )` | `m.exists( k )`         | `"k" + i` |
| Lua         | table (hash part)                          | `m[k] = i`, `m[k]` | `m[k] = nil`        | `m[k] ~= nil`           | `"k" .. i` |
| Python      | dict (hash table)                          | `m[k] = i`, `m[k]` | `del m[k]`          | `k in m`                | `f"k{i}"` |

**TeaScript**
- The keys array is a Tuple with `_tuple_reserve( keys, n )`. `m[k] := i` stores a copy of `i` (`:=` is
  a copy assignment).

**ChaiScript**
- The keys array is a Vector with `reserve( n )`. ChaiScript's Map has no `contains`; `count` is
  its membership test.

**AngelScript**
- The keys array is `array<string>` with `reserve( n )`. The dictionary stores every value as a
  `dictionaryValue`; `int( m[k] )` converts it back. `m.set( k, i )` and `m.get( k, v )` with an
  `int64` are the same speed (measured with MSVC).

**Lua**
- The keys array is a table created with `table.create( n )`, the map a table created with
  `table.create( 0, n )`, which reserves the hash part for n keys (Lua 5.5). Assigning `nil` removes
  a key.
- Lua indexes from 1: the key with the index i of the spec has the index i + 1, so the keys and the
  values are built from `i - 1`, and every second key is removed with `for i = 1, n, 2`.

**Python**
- The keys array is a list, built with `append`.
- The idiomatic Python loops over the keys would be `for i, key in enumerate( keys )` and
  `for key in keys`, ~5% faster for the whole call with Python 3.14 (MSVC). They are not used, so
  that every language reads the keys by index, like in `byte_buffer`.
- The checksum variable is called `total`, like in `loop`.
