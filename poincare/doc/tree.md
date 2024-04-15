# Tree data structure

[`Tree`](../src/memory/tree.h) is the central data structure in Poincare.

It stores an arbitrary
long, editable tree as a contiguous chunk of memory that can be easily moved,
copied and compared. Tools are provided to edit them safely, build them at
compile time or runtime and rewrite them using pattern-matching.

It is designed for space-efficiency and may be manipulated at a low-level
when fine control is preferred over safe abstractions.

## Block, Node, Type and Tree

Every `Tree` starts with a `Node` directly followed in memory by a given number of other
trees that are its children.

The `Node` always starts with a special `Block` of the `Type` enum that indicates
what the tree represents.

Depending on the type of the tree, it may have a fixed number of children –
possibly 0 – or a variable number written in the `Node` just after its type.

Nodes can also contain some more bytes to be interpreted according to their
types. In particular, numbers are represented with leaves (trees with no
children) and their value is contained inside their node.

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

Each NODE entry declares a Type with its number of children and the number of additional bytes its node
contains.

There is no class hierarchy corresponding to the types and they are intended to
be used in a C-style manner :

```cpp
switch (tree->type()) {
  case Type::Cos:
  case Type::Sin:
    ...
}
```

For convenience, `tree->isCos()` is defined as an equivalent to `tree->type() == Type::Cos`.

Related types are sometimes grouped inside ranges that provide a similar
method `tree->isNumber()`.

Since trees have a variable size, code manipulates them via `Tree *`
pointers. Moreover, the `const` keyword is used pervasively to differentiate
`const Tree *` from `Tree *` to constrain signatures.

Once you have a tree pointer, you may navigate with :
```cpp
Tree * sibling = tree->nextTree();
const Tree * child = constTree->child(0);
for (Tree * child : tree->children()) { ... }
for (const Tree * subTree : tree->selfAndDescendants()) { ... }
```

Unlike the previous Poincare, Tree can only be iterated forward.  You can't
access the previous child or the parent of a tree, unless you know a root tree
above this parent and walk downward from there (this is what
`parentOfDescendant` does).

Most of the time, algorithms are built such that their behavior doesn't change
depending of where your tree is or what are its siblings.


## The TreeStack

Trees may live anywhere (inside buffers in apps, in the storage, in flash) but only trees within the TreeStack can be modified.

The TreeStack is a dedicated range of memory where you can create and play
with your trees temporarily. It is not intended for storage and can be cleared by
exceptions. You must save your trees elsewhere before you return to the app’s code.

The primary way to create Trees from scratch is to push nodes at the end of the
`SharedTreeStack`. Its push method is templated to accommodate the different
arguments needed by various tree types.

```cpp
// pushing an Addition node with two-children and saving its address in *add*
Tree * add = SharedTreeStack->push<Type::Addition>(2);
// pushing a One node (that has no children) representing 1
SharedTreeStack->push(Type::One);
// cloning an other Tree at the end of the TreeStack
otherTree->clone()
// add now points to 1 + cloneOfOtherTree
```

> [!CAUTION]
> Be careful with `Tree *` they are easily broken when changing something above
> them in the TreeStack :

```cpp
Tree * a = someTree->clone();
Tree * b = anotherTree->clone();
// a and b are now in the TreeStack with b just after a
assert(a->nextTree() == b);

a->removeTree();
// the tree pointed to by a was just deleted from and the rest of the TreeStack after
// it was shifted in the whole; since pointers are not aware of that, a now
// points to the copy of anotherTree and b points at a corrupted place
```

## TreeRefs

TreeRefs are smart pointers used to track Trees as they move inside the
TreeStack.

For this purpose, the TreeStack owns a table of all the alive references and
updates each of them after each modification of a Tree inside the TreeStack. For this
reason, TreeRefs are intended to be temporary and used sparingly where
performance matters. You will often see function passing `TreeRefs&` to
avoid `TreeRef` object copy that would uselessly multiply the number of references pointing to a same tree.

All the methods on available on Trees are accessible on TreeRefs as
well. It should be easy to upgrade a `Tree *` into an `TreeRef` at any
point when you want to track your tree safely.

```cpp
TreeRef a = someTree->clone();
TreeRef b = anotherTree->clone();
// a and b are now in the TreeStack with b just after a
assert(a->nextTree() == b);

a->removeTree();
// since we are now using tracking references, a is uninitialized after the
// remove and b points to the anotherTree copy (which is now where a was)
```

You can now read the various Tree motions in `tree.h` and see how they update
references. Mind the difference between `moveBefore` and `moveAt` that are the
same function tree-wise but not reference-wise.

<details>
<summary>Implementation details </summary>

The TreeStack has a reference table, which is an array of node offsets. This
array has a maximal size (`TreeStack::k_maxNumberOfReferences`).

In addition, offset can be a special identifier
(`TreeStack::ReferenceTable::InvalidatedOffset`), indicating the node doesn't
exist anymore in the TreeStack.

Each TreeRef has an identifier. It represents the index at which the
TreeRef's node offset can be found in the TreeStack's reference
table.

Similarly, the identifier can be special :
- `TreeStack::ReferenceTable::NoNodeIdentifier` indicates the TreeRef
doesn't point to any tree.
- `TreeStack::ReferenceTable::DeletedOffset` indicates the TreeRef
has been deleted.

To retrieve the node pointed by an TreeRef, we just return the node in
the TreeStack at the corresponding offset.

Each time something is moved or changed in the TreeStack, all node offsets are
updated (`TreeStack::ReferenceTable::updateNodes`).

Once an TreeRef is destroyed, the corresponding node offset is set back
to `TreeStack::ReferenceTable::DeletedOffset`.

</details>

### Wrappers

Some methods manipulating `Tree *` may overwrite it with something else.

This isn't an issue with `Tree *` since the tree still lives at the same place.

However, TreeRef will be invalidated.

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
methods on TreeRef while preserving them.

For the example above, just add :
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

TreeRefs allow a safer and more readable Tree manipulation.

The only requirement is to ensure that they are only used in methods:
- Where efficiency isn't critical
- Expecting `TreeRef&` or `const Tree *`
- Expecting `Tree *`, but having a `EDITION_REF_WRAP` wrapper
// been overwritten.
- Expecting `Tree *`, but the tracked tree being overridden is either impossible or handled

Also remember that there is a limit to the number of TreeRef used at the same time (`TreeStack::k_maxNumberOfReferences`).

## KTrees

Trees may be created at compile time to be included in flash for static content
that won't change.

To do this you need to use the constexpr tree constructors, prefixed by `K`.
Here the up-to-date list for
[expressions](/poincare/src/expression/k_tree.h) and
[layouts](/poincare/src/layout/k_tree.h).

Some literals are also available to write numbers is a readable way :
 - `23_e` is the integer 23
 - `-4_e/5_e` is the rational -4/5 (a single Tree with no children, unlike
   Opposite(Division(4,5)))
 - `23.0_e` is a decimal
 - `23.0_fe` is a float
 - `23_de` is a double

```cpp
const Tree * immutableExpression = KExp(KMult(2_e, i_e, π_e));
```

You may construct large KTrees or factorize their construction using a constexpr
(mind that the twoPi will appear twice in the flash in this case) :

```cpp
constexpr KTree twoPi = KMult(2_e, π_e);

...
  KAdd(KCos(twoPi), KSin(twoPi))->clone();
```

You can use the name of a Node that expect children without the invocation to refer to its node.
However if the node is n-ary you need to provide the number of arguments with node :

```cpp
KCos->cloneNode(); // equivalent to SharedTreeStack->push(Type::Cos)
if (expr->nodeIsIdenticalTo(KMult.node<2>)) {}
```



<details>
<summary>Implementation details</summary>

KTrees are implemented with templates and each different tree has a different
type. The type includes as template parameters the complete list of blocks needed to represent the Tree.
`KCos(2_e)` is under-the-hood an alias to `KTree<Type::Cos, Type::Two>()`, ie an object of a special type used to represent only trees with a 2 in a cos.

These objects are empty and when they need to be casted to a `const Tree *` they instead 
return the pointer to a static member in their type that contains an array of blocks with the same content as the type.
This means all KCos(2_e) will point to the same object and that the pointer can out-live the object.

The behavior is similar to `const char * name() { return "poincare"; }` where the pointer is not a pointer to a local "poincare" const array but a static one that somehow escaped the function.

The only role of the template machinery in k_tree.h is to set-up these type aliases where `KAdd(a, b)` expands to `KTree<Type::Add, 2, blocks of the type of a..., blocks of the type of b...>()`.

As a consequence, you cannot build an array of different KTrees or declare a function that takes a
KTree. However you can define a template that only accept KTrees
using the concept that gathers them all like this :

```cpp
template <KTreeConcept KT> f(KT ktree) {
  ...
}
```

While the construction of the KTree is constexpr, the cast to a `const Tree *`
is not (yet). This means you may write :

```cpp
constexpr KTree k = 2_e;
```

but not

```cpp
constexpr const Tree * t = 2_e;
```

It practice it does not change anything at runtime since the compiler optimizes
the cast away in both cases.

Alternatively you can use the constexpr cast to Block * and then obtain a Tree *
from it at runtime with Tree::FromBlock if you want to use a constexpr array for
instance.

</details>



## Pattern matching

Constexpr trees are the basis of a much-used mechanism to create and rewrite
trees : pattern-matching.

The pattern is a constexpr tree containing placeholders named `KA`,`KB`…`KG`.

For instance you can match Cosine(Addition(2, 3)) against `KCos(KA)` and will
obtain a Context where `ctx[KA]` points to the Addition tree inside your
expression.

Or the other way around you can use Create with a structure and a context to
push at the end of the TreeStack a brand new tree :

```cpp
Tree * myTree = PatternMatching::Create(KAdd(1_e, KA), {.KA = otherTree});
```

These two functions are combined into `MatchCreate` and `MatchReplace` to
avoid dealing with the context at all :

```cpp
// Apply simplification a + a -> 2 * a
bool hasChanged = MatchReplace(myTree, KAdd(KA, KA), KMult(2_e, KA));
```
<details>
<summary>Note</summary>

In this example, `x+x` would be matched with KA pointing to the first `x`.

`x+y` would not match.

</details>

Methods `CreateSimplify` and `MatchReplaceSimplify` perform the same task,
but also call systematic simplification on each created tree along the way (but
not placeholders, which are assumed to be simplified trees already).


### Placeholders

Placeholders are named from `A` to `H`, and are expected to match with different
trees.

They have a type and are expected to have the same type both on match and on
create :

```cpp
// Apply simplification a*(b+c)*d -> a*b*d + a*(c)*d
bool hasChanged = MatchReplaceSimplify(
    myTree, KMult(KA, KAdd(KB, KC_p), KD_s),
    KAdd(KMult(KA, KB, KD_s), KMult(KA, KAdd(KC_p), KD_s)));
```

There are three types of placeholders :
- `One` : Matching a single tree, named `KA` for example.
- `ZeroOrMore` : Matching 0, 1 or more consecutive sibling trees, named `KD_s`
for example.
- `OneOrMore` : Matching 1 or more consecutive sibling trees, named `KC_p` for
example.

<details>
<summary>Note</summary>

In the `a*(b+c)*d -> a*b*d + a*(c)*d` example above, we would match :

|expression|KA|KB|KC_p|KD_s|
|-|-|-|-|-|
|`a*(b+c)*d`|`{a}`|`{b}`|`{c}`|`{d}`|
|`x*(y+x+1)`|`{x}`|`{y}`|`{x, 1}`|`{}`|
|`a*(b+c)*d*π*2`|`{a}`|`{b}`|`{c}`|`{d, π, 2}`|
|`(b+c)*d*π*2`|`{a}`|`{b}`|`{c}`|`{d, π, 2}`|

`(b+c)*d*π*2` or `a*b*c` would not match.

</details>
