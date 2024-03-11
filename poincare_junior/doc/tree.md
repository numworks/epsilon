# Trees

`Tree` is the main data structure inside poincare-junior. It stores an arbitrary
long, editable tree as a contiguous chunk of memory that can be easily moved,
copied and compared.

## Block, Node and tree types

Trees start with a `Node` directly followed in memory by a given number of other
trees that are its children.

The `Node` always starts with a special block of the `BlockType` enum that
indicates what the tree represents.

Depending on the type of the tree, it may have a fixed number of children –
possibly 0 – or a variable number written in the `Node` just after its type.

Nodes can also contain some more bytes to be interpreted according to their
types. In particular, numbers are represented with leaves (trees with no
children) and their value is contained inside their node.

<details>
<summary>Example</summary>

For instance, $cos(4×x)$ is represented by the tree :

|Cosine|Multiplication|2|IntegerShort|4|UserSymbol|1|'x'|
|-|-|-|-|-|-|-|-|

The first block `Cosine` is always unary so the next block is the start of its
only child.

`Multiplication` is n-ary, the next-block 2 indicates its number of children.

`IntegerShort` has always 0 children but its node has a additional byte to be
interpreted as its value here 4

`UserSymbol` has no children, the next block indicates the number of chars, stored in the following blocks.

</details>


Since trees have a variable size, code manipulates them via `Tree *`
pointers. Moreover, the `const` keyword is used pervasively to differentiate
`const Tree *` from `Tree *` to constrain signatures.

You may find the different tree types available for
[expressions](/poincare_junior/src/expression/types.h) and
[layouts](/poincare_junior/src/layout/types.h).  Each NODE entry declares
BlockType with its number of children and the number of additional bytes its
nodes contains.  They is no class hierarchy corresponding to the types but they
come with some helpers such as `tree->isCosine()` to test them.

Types are sometimes grouped together inside RANGE that provide similarly the
method `tree->isNumber()`.

You may also be interested by the various tree methods that you will find here :
**TODO**

Unlike the previous poincare, Tree can only be iterated forward.  You can't
access the previous child or the parent of a tree, unless you know a root tree
above this parent and your tree and go downward from there (this is what
`parentOfDescendant` does).

Most of the time, algorithms are built such that their behavior doesn't change
depending of where your tree is or what are its siblings.


## The EditionPool

Trees may live anywhere in memory (inside buffers in apps, in the storage or
even in flash) but only trees within the EditionPool can be modified.

The EditionPool is a dedicated range of memory where you can create and play
with your trees temporarily. It's not intended for storage and can be cleared by
exceptions. You must save your trees elsewhere before you return from pcj.

The primary way to create Trees from scratch is to push nodes at the end of the
`SharedEditionPool`. Its push method is templated to accommodate the different
arguments needed by various tree types.

```cpp
// pushing an Addition node with two-children and saving its address in *add*
Tree * add = SharedEditionPool->push<BlockType::Addition>(2);
// pushing a One node (that has no children) representing 1
SharedEditionPool->push(BlockType::One);
// cloning an other Tree at the end of the pool
otherTree->clone()
// add now points to 1 + cloneOfWhatEverOtherTreeWas
```

> [!CAUTION]
> Be careful with `Tree *` they are easily broken when changing something above
> them in the pool :

```cpp
Tree * a = someTree->clone();
Tree * b = anotherTree->clone();
// a and b are now in the EditionPool with b just after a
assert(a->nextTree() == b);

a->removeTree();
// the tree pointed to by a was just deleted from and the rest of the pool after
// it was shifted in the whole; since pointers are not aware of that, a now
// points to the copy of anotherTree and b points at a corrupted place
```

## EditionReferences

EditionReferences are smart pointers used to track Trees as they move inside the
pool.

For this purpose, the EditionPool owns a table of all the alive references and
updates each of them after each modification of a Tree inside the pool. For this
reason, EditionReferences are intended to be temporary and used sparingly where
performance matters. You will often see function passing `EditionReferences&` to
avoid copies. **TODO** what are ER copies ?

All the methods on available on Trees are accessible on EditionReferences as
well. It should be easy to upgrade a `Tree *` into an `EditionReference` at any
point when you want to track your tree safely.

```cpp
EditionReference a = someTree->clone();
EditionReference b = anotherTree->clone();
// a and b are now in the EditionPool with b just after a
assert(a->nextTree() == b);

a->removeTree();
// since we are now using tracking references, a is uninitialized after the
// remove and b points to the anotherTree copy (which is now where a was)
```

You can now read the various Tree motions in `tree.h` and see how they update
references. Mind the difference between `moveBefore` and `moveAt` that are the
same function tree-wise but not reference-wise.

`TODO` Wrappers


## KTrees

Trees may be created at compile time to be included in flash for static content
that won't change.

To do this you need to use the constexpr tree constructors, prefixed by `K`.
Here the up-to-date list for
[expressions](/poincare_junior/src/expression/k_tree.h) and
[layouts](/poincare_junior/src/layout/k_tree.h).

Some litterals are also available to write numbers is a readable way :
 - `23_e` is the integer 23
 - `-4_e/5_e` is the rational -4/5 (a single Tree with no children, unlike
   Opposite(Division(4,5)))
 - `23.0_e` is a decimal
 - `23.0_fe` is a float
 - `23_de` is a double

```cpp
const Tree * immutableExpression = KExp(KMult(2_e, i_e, π_e));
```

<details>
<summary>Note</summary>

While the construction of the KTree is constexpr, the cast to a `const Tree *`
is not. This means you may write :

`constexpr KTree k = 2_e`;

but not

`constexpr const Tree * t = 2_e;`

It practice it does not change anything at runtime since the compiler optimizes
the cast away in both cases but we might fix it at some point since `KTree`
cannot be put in an constexpr array for instance.

</details>



## Pattern matching

Constexpr trees are the basis of a much-used mechanism to create and rewrite
trees : pattern-matching.

The pattern is a constexpr tree containing placeholders named `KA`,`KB`…`KG`.

For instance you can match Cosine(Addition(2, 3)) against `KCos(KA)` and will
obtain a Context where `ctx[KA]` points to the Addition tree inside your
expression.

Or the other way around you can use Create with a structure and a context to
push at the end of the pool a brand new tree :

```cpp
Tree * myTree = PatternMatching::Create(KAdd(1_e, KA), {.KA = otherTree});
```

These two functions are combined into `MatchAndCreate` and `MatchAndReplace` to
avoid dealing with the context at all :

```cpp
// Apply simplification a + a -> 2 * a
bool hasChanged = MatchAndReplace(myTree, KAdd(KA, KA), KMult(2_e, KA));
```
<details>
<summary>Note</summary>

In this example, `x+x` would be matched with KA pointing to the first `x`.

`x+y` would not match.

</details>

Methods `CreateAndSimplify` and `MatchReplaceAndSimplify` perform the same task, but also call systematic simplification on each created tree along the way (but not placeholders, which are assumed to be simplified trees already).


### Placeholders

Placeholders are named from `A` to `H`, and are expected to match with different trees.
They have a type and are expected to have the same type both on match and on create :

```cpp
// Apply simplification a*(b+c)*d -> a*b*d + a*(c)*d
bool hasChanged = MatchReplaceAndSimplify(
    myTree, KMult(KA, KAdd(KB, KC_p), KD_s),
    KAdd(KMult(KA, KB, KD_s), KMult(KA, KAdd(KC_p), KD_s)));
```

There are three types of placeholders :
- `One` : Matching a single tree, named `KA` for example.
- `NoneOrMore` : Matching 0, 1 or more consecutive sibling trees, named `KD_s` for example.
- `OneOrMore` : Matching 1 or more consecutive sibling trees, named `KC_p` for example.

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
