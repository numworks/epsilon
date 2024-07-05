> [!IMPORTANT]
> This file is about the internal representation and usage of Trees.
>
> Refer to the [External API](./external.md) if you want to use them in applications.

## Table of contents

- [What is a Tree ?](#what-is-a-tree-)
- [How to know what Type a Tree has ?](#how-to-know-what-type-a-tree-has-)
- [How to walk through a Tree ?](#how-to-walk-through-a-tree-)
- [How to display a Tree ?](#how-to-display-a-tree-)
- [How to create a Tree at compile time ?](#how-to-create-a-tree-at-compile-time-)
- [How to create a Tree at runtime ?](#how-to-create-a-tree-at-runtime-)
- [How to retrieve sub-trees using pattern matching ?](#how-to-retrieve-sub-trees-using-pattern-matching-)
- [How to transform a Tree using pattern matching ?](#how-to-transform-a-tree-using-pattern-matching-)
- [How to transform an n-ary Tree using pattern matching ?](#how-to-transform-an-n-ary-tree-using-pattern-matching-)
- [How to reorganize Trees in the TreeStack ?](#how-to-reorganize-trees-in-the-treestack-)
- [How to track Trees in the TreeStack ?](#how-to-track-trees-in-the-treestack-)


## What is a Tree ?

[`Tree`](../src/memory/tree.h) is the central data structure in Poincare.

It stores an arbitrary
long, editable tree as a contiguous chunk of memory that can be easily moved,
copied and compared. Tools are provided to edit them safely, build them at
compile time or runtime and rewrite them using pattern-matching.

It is designed for space-efficiency and may be manipulated at a low-level
when fine control is preferred over safe abstractions.

### Block, Node, Type and Tree

Every `Tree` starts with a `Node` directly followed in memory by a given number of other
trees that are its children.

There are two types of `Block`:
- `TypeBlock`: The `Node` always starts with a special block containing the type (from
the `Type` enum) that indicates what the `Tree` represents.
- `ValueBlock`: Every other block. If a `Tree` can have a variable number of children,
it is written in a block just after the type block. Nodes can also contain some more
blocks to be interpreted according to their types. In particular, numbers are represented
with leaves (trees with no children) and their value is contained inside their node.

> [!NOTE]
> For instance, $cos(4×x)$ is represented by this tree of 9 blocks:
>
> |Cos|Mult|2|IntegerShort|4|UserSymbol|2|'x'|0|
> |-|-|-|-|-|-|-|-|-|
>
> `Cos` is a unary tree so the next block is the start of its only child.
>
> `Mult` is n-ary, the next block `2` indicates its number of children.
>
> `IntegerShort` has always 0 children but its node has an additional byte to be
> interpreted as its value, `4`
>
> `UserSymbol` has no children, the next block indicates the number of chars,
> stored in the following blocks.
>

You will find all the available types in [expression/types.h](/poincare/src/expression/types.h) and
 [layout/types.h](/poincare/src/layout/types.h).

There is no class hierarchy corresponding to the types and they are intended to
be used in a C-style manner:

```cpp
switch (tree->type()) {
  case Type::Cos:
  case Type::Sin:
    ...
}
```

Since trees have a variable size, code manipulates them via `Tree *`
pointers. Moreover, the `const` keyword is used pervasively to differentiate
`const Tree *` from `Tree *` to constrain signatures.


Unlike the previous Poincare, `Tree` can only be iterated forward.  You can't
access the previous child or the parent of a tree, unless you know a root tree
above this parent and walk downward from there (this is what
`parentOfDescendant` does).

Most of the time, algorithms are built such that their behavior doesn't change
depending of where your tree is or what are its siblings.


## How to know what Type a Tree has ?

There are three situations to distinguish:

- If you want to test if a node has a given type, say `Cos`, use the automatically defined `tree->isCos()` method. This is equivalent to `tree->type() == Type::Cos`. You can also use `TypeBlock::IsCos(type)`.

- If you want to test if a node belongs to a group of related types (called a range), say `Integer`, use the automatically defined `tree->isInteger()` method. You can also use `TypeBlock::IsInteger(type)`.
  <details>
  <summary>Ranges declarations</summary>
   
  A range is defined on consecutive nodes with `RANGE(rangeName, FirstNode, LastNode)` and may be nested.

  ```cpp
  NODE(Zero)
  ...
  NODE(EulerE)
  NODE(Pi)

  RANGE(MathematicalConstant, EulerE, Pi)
  RANGE(Number, Zero, Pi)
  ```

  In the example above, the range `MathematicalConstant` contains all the nodes from `EulerE` to `Pi`,
  and the range `Number` contains all the nodes from `Zero` to `Pi`. The first range generates a method
  `tree->isMathematicalConstant()` and the second range generates a method `tree->isNumber()`.

  The range `MathematicalConstant` is included in the range `Number`.
  </details>

- If you want to know the dimension of your expression (scalar, matrix, boolean, point or unit) use `Dimension::Get(tree)`. You can have a list of any dimension listed above, and you can check the list length with `Dimension::ListLength(tree)`.


## How to walk through a Tree ?
Once you have a tree pointer, you may iterate over its children or descendants with:
```cpp
Tree * firstChild = tree->nextNode();
Tree * secondChild = firstChild->nextTree();
Tree * childN = tree->child(n);
```
```cpp
const Tree * child = constTree->child(0);
```
```cpp
for (Tree * child : tree->children()) {
  f(child);
}
```
```cpp
for (const Tree * subTree : tree->selfAndDescendants()) {
  f(subtree);
}
```

See more in [Iteration over child](./style_guide.md#iteration-over-child).


## How to display a Tree ?
The structure of a `Tree` can be inspected in DEBUG with several log functions, depending on the level of detail your are interested with:
- `tree->logBlocks()` shows the block representation with each block displayed as `[interpretation]`
  ```
  [Add][2]
    [Two]
    [IntegerPosShort][3]
  ```

- `tree->log()` shows the tree structure in a XML fashion with addresses to help tell who is who
  ```xml
  <Add size="2" address="0x1006bb3a5" numberOfChildren="2">
    <Two size="1" address="0x1006bb3a7" value="2"/>
    <IntegerPosShort size="2" address="0x1006bb3a8" value="3"/>
  </Add>
  ```

- `tree->logDiffWith(otherTree)` can compare trees with a diff-like output on top of the log output

- `tree->logSerialized()` is more concise and used to focus on the mathematical logic rather than the structure
  ```
  2+3
  ```

## How to create a Tree at compile time ?

When you have a formula that does not need to change, you can define it at
compile time (to be included in the flash) using a `KTree`.

KTrees are a collective name for a collection of constexpr constructors defined in the headers
[expression/k_tree.h](/poincare/src/expression/k_tree.h) and
[layout/k_tree.h](/poincare/src/layout/k_tree.h).

They have the name of the node prefixed by K and can be nested to create the tree you would expect:

```cpp
constexpr const Tree * k_immutableExpression = KExp(KMult(2_e, i_e, π_e)); // exp(2*i*π)
```

### Literals

Some specials literals with the suffix `_e` exist to write numbers in a readable way:
 - `23_e` is the integer 23
 - `-4_e/5_e` is the rational -4/5 (a single tree with no children, unlike
   Opposite(Division(4,5)))
 - `23.0_e` is a decimal
 - `23.0_fe` is a float
 - `23_de` is a double

### Layouts

KTrees to create layouts work in a similar fashion with:
 - an L suffix to the constructors, e.g. `KRackL`, `KAbsL`
 - `"a"_cl` is a shortcut for KCodePointL<'a'>()
 - `"abc"_l` is a shortcut for KRackL("a"_cl, "b"_cl, "c"a"_cl)
 - `^` can concatenate layouts and racks into racks

```cpp
const Tree * quadraticLayout = "a·x"_l ^ KSuperscriptL("2"_l) ^ "+b·x+c"_l;
```

### Reusing KTrees

You may construct large KTrees or factorize their construction using a constexpr.

```cpp
constexpr KTree twoPi = KMult(2_e, π_e);

...
  KAdd(KCos(twoPi), KSin(twoPi))->cloneTree();
```

### Using KTrees just for a node

You can use a `KTree` constructor without the parentheses and children if you only need the node.

```cpp
KCos->cloneNode(); // equivalent to SharedTreeStack->pushCos()
```

If the node is n-ary, you need to provide the number of arguments with `.node<nbChildren>`:

```cpp
if (expr->nodeIsIdenticalTo(KMult.node<2>)) {}
```

### Implementation details
<details>
<summary>Details</summary>

`KTrees` are implemented with templates and each different tree has a different C++
type. The type includes as template parameters the complete list of blocks needed to represent the `Tree`.
`KCos(2_e)` is under-the-hood an alias to `KTree<Type::Cos, Type::Two>()`, ie an object of a special type used to represent only trees with a 2 in a cos.

These objects are empty and when they need to be casted to a `const Tree *` they instead 
return the pointer to a static member in their type that contains an array of blocks with the same content as the type.
This means all `KCos(2_e)` will point to the same object and that the pointer can out-live the object.

The behavior is similar to `const char * name() { return "poincare"; }` where the pointer is not a pointer to a local "poincare" const array but a static one that somehow escaped the function.

The only role of the template machinery in k_tree.h is to set-up these type aliases where `KAdd(a, b)` expands to `KTree<Type::Add, 2, blocks of the type of a..., blocks of the type of b...>()`.

As a consequence, you cannot build an array of different KTrees or declare a function that takes a
`KTree`. However you can define a template that only accept KTrees
using the concept that gathers them all like this:

```cpp
template <KTreeConcept KT> f(KT ktree) {
  ...
}
```
</details>

## How to create a Tree at runtime ?

When you need a `Tree` depending on user content or computed values, you need to build it on the `TreeStack`.

> [!NOTE]
> The `TreeStack` is a dedicated range of memory where you can create
> and play with your trees temporarily. It is not intended for storage and can be
> cleared by exceptions. You must save your trees elsewhere before you return to
> the app’s code.

### Pushing nodes

The most basic way to create trees from scratch is to push nodes successively at the end of the `TreeStack`.
The global object `SharedTreeStack` has push methods for each kind of node:

```cpp
// pushing an Add (addition) node with two-children
Tree * expr1 = SharedTreeStack->pushAdd(2);
```
```cpp
// pushing Pi
SharedTreeStack->pushPi();
```
```cpp
// pushing a double
SharedTreeStack->pushDoubleFloat(3.0);
```

For nodes with additional value blocks, you can find the push methods and their arguments in [tree_stack.h](../src/memory/tree_stack.h).

You are responsible to create a valid tree structure using this method.

If you do the three previous operations in that order, `expr1` will point to the tree `π+3.0`.

If you do only the first two, `expr1` will silently point to a broken tree with garbage that will crash when used.

### Cloning trees

Since all methods returning a new `Tree*` need to push it at the end of the `TreeStack`,
you can use both node pushes and tree creation methods to build a more complex structure.

```cpp
Tree * expr2 = SharedTreeStack->pushCos();
expr1->cloneTree()
```
```
(lldb) expr2->logSerialize()
cos(π+3.0)
```

### Cloning nodes

Similarly you can clone a node with `cloneNode` but mind the difference between:
```cpp
Tree * clone1 = twoPi->cloneTree();  // nodes on TreeStack: ...[Mult][Two][Pi]
Tree * clone2 = clone1->cloneNode(); // nodes on TreeStack: ...[Mult][Two][Pi][Mult]
```
`clone2` is not a valid tree: `clone2->nextTree()`, `clone2->cloneTree()` and `clone2->log()` will crash.
Read [style guide](./style_guide.md#distinguish-trees-from-nodes).

### Using pattern matching

A safe way to build a tree is to use `PatternMaching::Create`.

```cpp
Tree * expr3 = PatternMatching::Create(KAdd(1_e, KA), {.KA = expr2});
```
```
(lldb) expr3->logSerialize()
1+cos(π+3.0)
```

The first argument is a pattern, a [constexpr tree](#how-to-create-a-tree-at-compile-time-) that may contain placeholders named `KA`,`KB`… up to `KH`.

The second argument is a `PatternMatching::Context` that associates any placeholder used in the pattern to a `const Tree*` to be cloned each time the pattern is encountered.

The resulting tree is valid and will be pushed at the end of the `TreeStack`.

If you need a simplified tree, use `CreateSimplify` with a pattern that is a simplified tree and context values that are simplified too.

> [!NOTE]
> Pattern matching is only interesting when you have a context. Is it not optimal to do for example:

```cpp
Tree * expr4 = PatternMatching::Create(KAdd(1_e, i_e));
```

You can do instead:

```cpp
Tree * expr4 = KAdd(1_e, i_e)->cloneTree();
```

> [!WARNING]
> Placeholders are cloned, so beware that they still live after the pattern matching: you are responsible to delete them if necessary, using `tree->removeTree()`.


## How to retrieve sub-trees using pattern matching ?

`PatternMatching::Match` works the other way around and will fill a `PatternMatching::Context` if
the tree fits the pattern.

For instance you can match Cos(Add(2, 3)) against `KCos(KA)` and will
obtain a Context where `ctx[KA]` points to the addition tree inside your
expression.

```cpp
PatternMatching::Context ctx;
const Tree * expr5 = Cos(Add(2, 3));
if (PatternMatching::Match(expr5, KCos(KA), &ctx)) {
  ctx->getTree(KA); // Points to Add inside expr5
}
```


## How to transform a Tree using pattern matching ?

The functions `Match` and `Create` are combined in `MatchCreate` and `MatchReplace` to alter the structure of trees without dealing with a context at all:

```cpp
// Apply simplification a + a -> 2 * a
bool hasChanged = MatchReplace(tree, KAdd(KA, KA), KMult(2_e, KA));
```
<details>
<summary>Note</summary>

- In this example, `x+x` would be matched with KA pointing to the first `x`.
- `x+y` would not match.

</details>

Methods `CreateSimplify` and `MatchReplaceSimplify` perform the same task,
but also call systematic simplification on each created tree along the way (**but
not placeholders**, which are assumed to be simplified trees already).


## How to transform an n-ary Tree using pattern matching ?

If you want to match children in an n-ary without knowing its number of children in advance,
you can use `_s` and `_p` to match several children with a single placeholder:
 - `_s` stands for `*` in regex, `KA_s` will match 0, 1 or more trees
 - `_p` stands for `+` in regex, `KA_p` will match 1 or more trees

When you used `_s` or `_p` to match several children with a placeholder, you are expected to reuse the
same suffix to insert these trees inside an n-ary in the create pattern.

```cpp
// Apply simplification a*(b+c...)*d... -> a*b*d + a*(c...)*d...
bool hasChanged = MatchReplaceSimplify(
    tree, KMult(KA, KAdd(KB, KC_p), KD_s),
    KAdd(KMult(KA, KB, KD_s), KMult(KA, KAdd(KC_p), KD_s)));
```

More examples of expressions that match `KMult(KA, KAdd(KB, KC_p), KD_s)`:

|expression|KA|KB|KC_p|KD_s|
|-|-|-|-|-|
|`a*(b+c)*d`|`{a}`|`{b}`|`{c}`|`{d}`|
|`x*(y+x+1)`|`{x}`|`{y}`|`{x, 1}`|`{}`|
|`a*(b+c)*d*π*2`|`{a}`|`{b}`|`{c}`|`{d, π, 2}`|

`(b+c)*d*π*2` or `a*b*c` would not match.


## How to reorganize Trees in the TreeStack ?
The [Tree class](../src/memory/tree.h) offers a lot of methods to move, clone, delete and swap trees in the `TreeStack`.

When doing so you should always keep in mind that the `TreeStack` is a Stack and that removing or extending
a tree will move the trees placed after it.

```cpp
Tree * a = someTree->cloneTree();
Tree * b = anotherTree->cloneTree();
// a and b are now in the TreeStack with b just after a
assert(a->nextTree() == b);

a->removeTree();
// the tree pointed to by a was just deleted from and the rest of the TreeStack after
// it was shifted in the whole; since pointers are not aware of that, a now
// points to the copy of anotherTree and b points at a corrupted place
```


## How to track Trees in the TreeStack ?

A **`TreeRef`** is a smart pointer used to track a `Tree` as it moves inside the
`TreeStack`. It is not needed in the `Pool` since nodes cannot be edited there.

For this purpose, the `TreeStack` owns a table of all the alive `TreeRef` and
updates each of them after each modification of a `Tree` inside the `TreeStack`. For this
reason, `TreeRefs` are intended to be temporary and used sparingly where
performance matters. You will often see function passing `TreeRefs &` to
avoid `TreeRef` object copy that would uselessly multiply the number of references pointing to a same `Tree`.

All the methods on available on `Tree` are accessible on `TreeRef` as
well. It should be easy to upgrade a `Tree *` into a `TreeRef` at any
point when you want to track your `Tree` safely.

```cpp
TreeRef a = someTree->cloneTree();
TreeRef b = anotherTree->cloneTree();
// a and b are now in the TreeStack with b just after a
assert(a->nextTree() == b);

a->removeTree();
// since we are now using tracking references, a is uninitialized after the
// remove and b points to the anotherTree copy (which is now where a was)
```

You can now read the various `Tree` motions in [tree.h](/poincare/src/memory/tree.h) and see how they update
references. Mind the difference between `moveBefore` and `moveAt` that are the
same function tree-wise but not reference-wise.

The `TreeStack` has a `log` method as well, that shows each tree it contains and the references pointing to trees.

`(lldb) p Poincare::Internal::TreeStack::SharedTreeStack->log()`

```xml
<TreeStack>
  <Reference id="0, ">
    <Add numberOfChildren="2">
      <Two value="2"/>
      <IntegerPosShort value="3"/>
    </Add>
  </Reference>
</TreeStack>
```

<details>
<summary>Implementation details </summary>

The `TreeStack` has a reference table, which is an array of node offsets. This
array has a maximal size (`TreeStack::k_maxNumberOfReferences`).

In addition, offset can have a special value:
- `TreeStack::ReferenceTable::InvalidatedOffset` indicates that the node doesn't
exist anymore in the `TreeStack`.
- `TreeStack::ReferenceTable::DeletedOffset` indicates that the `TreeRef`
has been deleted.

Each `TreeRef` has an identifier. It represents the index at which the
`TreeRef`'s node offset can be found in the `TreeStack`'s reference
table.

Similarly, the identifier can be the special value
`TreeStack::ReferenceTable::NoNodeIdentifier` to indicates that the `TreeRef` doesn't
point to any tree.

To retrieve the node pointed by an `TreeRef`, we just return the node in
the `TreeStack` at the corresponding offset.

Each time something is moved or changed in the `TreeStack`, all node offsets are
updated (`TreeStack::ReferenceTable::updateNodes`).

Once an `TreeRef` is destroyed, the corresponding node offset is set back
to `TreeStack::ReferenceTable::DeletedOffset`.

</details>

### Wrappers

Some methods manipulating `Tree *` may overwrite it with something else.

This isn't an issue with `Tree *` since the tree still lives at the same place. Indeed, the method can only edit this specific tree and not other trees in the `TreeStack`.

However, `TreeRef` will be invalidated.

```cpp
static void ReplaceTreeWithZero(Tree * tree) {
  tree->cloneTreeOverTree(0_e);
}

Tree * a = someTree->clone();
TreeRef b = a
ReplaceTreeWithZero(b); // Exact Equivalent of ReplaceTreeWithZero(a);

assert(a->isZero()); // Ok
assert(b->isZero()); // Raise because b no longer exists, the tracked tree has
// been overwritten.
```

To minimize the risk of mistakes, we created a wrapper allowing the use of such
methods on `TreeRef` while preserving them.

For the example above, just add:
```cpp
static void ReplaceTreeWithZero(Tree * tree) {
  tree->cloneTreeOverTree(0_e);
}
/* Create a static void ReplaceTreeWithZero(TreeRef& tree) calling the
 * original ReplaceTreeWithZero, and restoring the TreeRef to the
 * original Tree */
EDITION_REF_WRAP(ReplaceTreeWithZero);
// ...
assert(b->isZero()); // Ok
```

### When to use TreeRef

TreeRefs allow a safer and more readable tree manipulation.

The only requirement is to ensure that they are only used in methods:
- Where efficiency isn't critical
- Expecting `TreeRef&` or `const Tree *`
- Expecting `Tree *`, but having a `EDITION_REF_WRAP` wrapper
- Expecting `Tree *`, but the tracked tree being overridden is either impossible or handled

Also remember that there is a limit to the number of `TreeRef` used at the same time (`TreeStack::k_maxNumberOfReferences`).
