# Style guide

Here are guidelines to follow when developing in Poincare.
These practices ensure an optimized, uniform and self-contained code base.

> [!NOTE]
> As Poincare is being updated, old files are still being processed and may not yet follow those guidelines.
>
> If there is a good reason not to follow one of them, please add a comment explaining why.
>

## Keep internal API private

Poincare has an internal and an external API. Only the external API should be accessible from outside.

Poincare should never use code from `apps/*`.

In general, avoid exposing `poincare/src/*` in apps, directly or indirectly (via poincare's external headers for example).

> [!CAUTION]
> Avoid this:

```cpp
/* In poincare/include/expression.h */
#include <apps/shared/sequence.h>
#include <poincare/src/expression/context.h>
// ...
void myMethod(Internal::Context ctx) const {
  // ...
}
```

> [!TIP]
> Prefer this:

```cpp
/* In apps/shared/sequence.h */
#include <poincare/expression.h>
```
```cpp
/* In poincare/include/expression.h */
class Internal::Context;
// ...
// Implementation in poincare/src/expression.cpp
void myMethod(Internal::Context ctx) const;
```

## Only edit Trees on the TreeStack

The `TreeStack` is the only place where `Tree` edition is allowed.

When a `Tree` is stored out of the `TreeStack` and needs to be changed, it is copied onto `TreeStack`, edited there, and then the original `Tree` is overwritten with the result.

## Use switch and C-style code with Tree structure

To avoid heavy v-tables and better organize the code, we steer away from heavy use of virtuality on our `Tree` structure.

> [!CAUTION]
> Avoid this:

```cpp
class AdditionTree : Public Tree {
 public:
  Type type() const override;
  bool simplify() override;
  // ...
};
```

> [!TIP]
> Prefer this:

```cpp
// Static method of Simplification module. Tree is a final class.
bool Simplify(Tree* t) {
  switch (t->type()) {
    case Type::Add:
      return SimplifyAdd(t);
    // ...
  }
}
```

## Tree creation on TreeStack

To preserve valid tree pointers whenever something is put on the `TreeStack`, trees must be pushed at the end.

> [!CAUTION]
> Avoid this:

```cpp
// Create a Undef Tree right after u
Tree* v = u->nextTree()->cloneTreeBeforeNode(KUndef);
// Edit v
```

> [!TIP]
> Prefer this:

```cpp
// Create a Undef Tree at the end of the TreeStack
Tree* v = KUndef->clone();
// Edit v
```

## Signature for methods altering trees

We try to use a consistent signature for methods altering trees inplace.

> [!CAUTION]
> Avoid this:

```cpp
// Alter tree and set changed to true if something changed.
void Tree::alter(bool* changed, Context ctx);
```

> [!TIP]
> Prefer this:

```cpp
// Alter tree, return true if something changed.
static bool Alter(Tree* tree, Context ctx);
```

## Iteration over child

`Tree::child` method can be quite costly as it needs to browse through every previous children. Calling it multiple time over successive children is really costly.

> [!CAUTION]
> Avoid this:

```cpp
for (int i = 0; i < tree->numberOfChildren(); i++) {
  f(tree->child(i));
}
```

> [!TIP]
> Prefer this:

```cpp
for (Tree* child : tree->children()) {
  f(child);
}
```

If you need the index of the child use:
```cpp
for (IndexedChild<const Tree*> child : tree->indexedChildren()) {
  if (child.index == 0) {
    f(child); // child get casted into a const Tree *
  }
}
```

For more advanced operations, you can use :
```cpp
Tree* child = tree->child(0);
while (n < tree->numberOfChildren()) {
  f(child);
  child = child->nextTree();
}
```

## Tree's parent access

Accessing a Tree's parent requires a root node and is costly.

> [!CAUTION]
> Avoid this:

```cpp
/* At child's level */
if (e->parent(root)->isVerticalOffset()) {
  // ...
}
```

> [!TIP]
> Prefer this:

```cpp
/* At parent's level */
if (isVerticalOffset()) {
  // ...
}
```

## Access to first child

> [!CAUTION]
> Avoid this:

```cpp
Tree* firstChild = tree->nextNode();
```

> [!TIP]
> Prefer this:

```cpp
Tree* firstChild = tree->child(0);
```

Both examples are equivalent to access a tree's first child, but the later asserts the Tree has a child and makes the intent clear.

## Save children pointers when are used several times

Remind that `nextTree` and `child` walk the tree descendants, iterating over all of them.
It may be costly when the Tree is large.

> [!CAUTION]
> Avoid this:

```cpp
bool TestSomethingOnPow(const Tree * pow) {
  if (pow->child(1)->isInteger() && !pow->child(1)->isZero()) {
    ...
  }
}
```

> [!TIP]
> Prefer this:

```cpp
bool TestSomethingOnPow(const Tree * pow) {
  const Tree * base = pow->child(0);
  const Tree * exponent = pow->child(1); // or base->nextTree()
  if (exponent->isInteger() && !exponent->isZero()) {
    ...
  }
}
```

## Distinguish Trees from Nodes

Childless trees are equivalent to their node. We avoid mixing the definition for clarity, and in case children are added later.

> [!CAUTION]
> Avoid this:

```cpp
tree->cloneNodeOverTree(KUndef);
```

> [!TIP]
> Prefer this:

```cpp
tree->cloneTreeOverTree(KUndef);
```

> [!CAUTION]
> This is broken:

```cpp
KPow->clone(); // random bytes are copied as children
```

> [!TIP]
> This does what you want:

```cpp
KPow->cloneNode();
```
## Add TREE_REF_WRAP to public methods overriding their input Tree
> [!CAUTION]
> This is dangerous :

```cpp
bool HighlyAdvancedReduction(Tree *t) {
  t->cloneTreeOverTree(0_e);
  return true;
}

void user() {
  TreeRef r = ...;
  HighlyAdvancedReduction(r);
  r->removeTree(); // r is broken !
}
```

> [!TIP]
> But fine if you add :

```cpp
bool HighlyAdvancedReduction(Tree *t) { ... }
TREE_REF_WRAP(HighlyAdvancedReduction);
```


## Create Ranges to test membership to related node types
> [!CAUTION]
> Avoid this:

```cpp
bool IsTrigoHyperbolic(const Tree *t) {
  return t->isCosH() || t->isSinH() || t->isTanH();
}
```

> [!TIP]
> Prefer this:

```cpp
// in types.h
NODE(CosH, 1)
NODE(SinH, 1)
NODE(TanH, 1)
RANGE(TrigoHyperbolic, CosH, TanH)

// automatically defined in your code
t->isTrigoHyperbolic()
```

## Use layoutType in a switch over layout’s types
> [!CAUTION]
> Avoid this:

```cpp
switch(layout->type()) {
  case Type::CodePointLayout:
    ...
  case Type::VerticalOffsetLayout:
    ...
  default: // do nothing if expression
}
```

> [!TIP]
> Prefer this:

```cpp
switch(layout->layoutType()) {
  case LayoutType::CodePoint:
    ...
  case LayoutType::VerticalOffset:
    ...
  // no default if all layouts are handled
}
```

## Use Rack* and Layout* 
To make the distinction on what the Tree* points to.
> [!CAUTION]
> Avoid this:

```cpp
void moveCursorAt(Tree * rack, int index) {
  Tree * layout = rack->child(index);
  ...
```

> [!TIP]
> Prefer this:

```cpp
void moveCursorAt(Rack * rack, int index) {
  Layout * layout = rack->child(index);
  ...
```

## Others

| Avoid | Prefer |
|-------|------|
| Non-recursive bottom-up iteration | Iterate in the right direction to always change the downstream children |
| Handling ill-formatted expression during simplification | Implement check in `DeepCheckDimensions` or `DeepCheckListLength` to always assume valid expressions |
| Uncertain manipulation of multiple `Tree* ` | Using `TreeRef` |
