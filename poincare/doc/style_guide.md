# Style guide

Here are guidelines to follow when developing in Poincare.
These practices ensure an optimized, uniform and self-contained code base.

> [!NOTE]
> As Poincare is being updated, old files are still being processed and may not yet follow those guidelines.
>
> If there is a good reason not to follow one of them, please add a comment explaining why.
>

## Keep internal API private

Poincare has an internal and an external API. Only the external API should be available and accessed from outside.

Poincare should never use code from `apps/*`.

Code from `poincare/src/*` should only be used in `poincare/src/*` or `poincare/**/*.cpp`.

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

TreeStack is the only place Tree edition is allowed.

When a Tree is stored out of the TreeStack and needs to be changed, it is copied onto TreeStack, edited there, and the original tree is overwritten with the result.

## Use switch and C-style code with Tree structure

To avoid heavy v-tables and better organize the code, we steer away from heavy use of virtuality on our Tree structure.

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

To preserve valid `Tree*` pointers whenever something is put on the TreeStack, it must be pushed at the end.

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
// Create a Undef Tree at the end of the tree Stack
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

To access a tree's first child, they are equivalent, but the later asserts the Tree has a child.

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

## Distinguish Trees from Nodes

Childless Trees are equivalent to their Node. We avoid mixing the definition for clarity, and in case children are added later.

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

## Others

| Avoid | Prefer |
|-------|------|
| Non-recursive bottom-up iteration | Iterate in the right direction to always change the downstream children |
| Handling ill-formatted expression during simplification | Implement check in `DeepCheckDimensions` or `DeepCheckListLength` to always assume valid expressions |
| Uncertain manipulation of multiple `Tree* ` | Using `TreeRef` |
| Public methods susceptible of overwriting their input Tree | Use EDITION_REF_WRAP |
| `isUserSymbol() \|\| isUserSequence() \|\| isUserFunction()` | Organize types.h and create a `RANGE`: `isUserNamed()` |
| With layout trees, `switch (type())` | `switch (layoutType())` |
| With layout trees, `Tree*` | `Rack*` or `Layout*` |
