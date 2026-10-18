# binary_trees

**Group:** data. **Measures:** allocation and freeing of many small records, and the garbage
collection: binary trees are built and checked recursively (the algorithm of the Benchmarks Game).

## Task

`make( d )` builds a full binary tree of depth d from record nodes with the fields `left` and
`right`; `check( t )` counts its nodes. `bench( n )` builds and checks 2^(16 - n) trees of depth n:

```
make( d ):
    if d == 0:
        return Node( no child, no child )
    return Node( make( d - 1 ), make( d - 1 ) )

check( t ):
    if t.left is no child:
        return 1
    return 1 + check( t.left ) + check( t.right )

bench( n ):
    sum = 0
    count = 1 << (16 - n)
    for k = 1 .. count:
        t = make( n )
        sum = sum + check( t )
    return sum
```

- A series over the depth: n = 4 / 8 / 12. Every call builds ~131k nodes in total, 2^17 - 2^(16-n):
  2,048 trees of 63 nodes, 128 trees of 511 nodes, or 8 trees of 8,191 nodes. The checksum is the
  number of nodes (126,976 / 130,816 / 131,056).
- The series shows the effect of the tree size at the same volume: larger trees live longer and are
  freed in larger pieces. It also shows ChaiScript's cost of a recursion (see below).
- n <= 16. All values fit into 32 bit signed integers.

## Rules

- A node is the language's natural record type (like in `records`) with the fields `left` and
  `right`. A leaf is a node whose children are the language's value for "no child" (see below).
- `make` and `check` are named, recursive script functions, like `fib` in `fib_recursive`. No node
  pools or reuse of nodes: every tree is new, and it becomes garbage when `t` gets the next tree.
- The memory is managed by the language as usual. A full garbage collection runs before each
  measured call (PLAN.md, rule 10); the work of the garbage collector during the call counts.
- Use the language's natural counting loop in `bench`. Plain assignments, no compound assignment.

## Notes per language

**TeaScript**
- A node is a named Tuple; `def node.left @= make( d - 1 )` links the subtree with a shared
  assignment (no copy). Function parameters are shared by default, so `check( t )` doesn't copy
  either. TeaScript frees a tree by reference counting when its last reference is gone.
- `false` stands for "no child": every node has the elements `left` and `right`, and "no child"
  needs no object of its own, like `None`, `nil` or `null` in the other languages. `if( t.left )` is
  true for a node (a non-empty Tuple) and false for `false`.
- Alternatives measured with MSVC (`vm-O1`): an empty Tuple for "no child" (the idiom of TeaScript's
  linked list demo) allocates two more Tuples per leaf and was ~13% slower; a leaf as a Tuple
  without elements was ~20% faster, but is not a record with `left` and `right`.

**ChaiScript**
- A `class Node` with two constructors: `Node(l, r)` sets the attributes, `Node()` leaves them
  undefined, which stands for "no child"; `check` tests it with `is_var_undef()`. ChaiScript frees
  the objects by reference counting.
- ChaiScript's cost of a call grows within a recursion (see `fib_recursive/README.md`). One call of
  `make( n )` makes 2^(n+1) - 1 calls, so the larger trees are much slower: ~0.52 / 0.94 / 7.6 s per
  call for n = 4 / 8 / 12 (MSVC, `no-threads`). n = 12 is limited by the time budget.

**AngelScript**
- A script `class Node` with the handles `Node@ left` and `Node@ right`; `null` stands for
  "no child". The objects are reference counted. A class which holds handles of its own type could
  form circular references, so AngelScript also tracks its objects in the garbage collector, which
  works incrementally during the calls (automatic garbage collection stays on).
- With GCC the measured calls were slower than the first call of each run (e.g. ~52-85 ms instead of
  ~42-47 ms), with MSVC they were not. This is work of the memory management during the calls,
  which counts.

**Lua**
- A node is a table `{ left = ..., right = ... }`; `nil` stands for "no child", so a leaf is a table
  without entries. `make` and `check` are `local function`s, like `fib` in `fib_recursive`.
- The incremental garbage collector frees the trees.

**Python**
- A class `Node` with `__init__( self, left, right )`; `None` stands for "no child". Reference
  counting frees every tree when `t` gets the next one.
- `__slots__` was 0-9% faster with Python 3.14 (MSVC; the most for n = 12). The script uses the
  plain class, like in `records`.
- The variable is called `total`, like in `loop`.
