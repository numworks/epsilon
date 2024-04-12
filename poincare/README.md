# Poincare

Poincare is Epsilon's computer algebra engine.

## Documentation

More in-depth documentation is available [here](doc).

## Guidelines

Here are guidelines to follow when developing in Poincare.
These practices ensure an optimized, uniform and self-contained code base.

> [!NOTE]
> As Poincare is being updated, old files are still being processed and may not yet follow those guidelines.
>
> If there is a good reason not to follow one of them, please add a comment explaining why.
>

| Avoid | Prefer |
|-------|------|
| `#include <apps/*>` in `poincare/*` | Implementation in Poincare |
| `#include <poincare/src/*>` anywhere but `poincare/src/*` or `poincare/**/*.cpp` | Forward class declaration or source implementation |
| Tree edition outside of the TreeStack | Copy it on the TreeStack, edit it and overwrite the original |
| Using virtuality and v-tables | Switch and C-style code |
| Creating Trees in the middle of the TreeStack | Pushing them a the end of the TreeStack |
| `Tree::alter(bool * changed)` | `bool AlterTree(Tree* tree)` |
| <pre>for (int i = 0; i < e->numberOfChildren(); i++) {<br>&emsp; f(e->child(i));<br>} | <pre>for(Tree * child : e->children()) {<br>&emsp; f(child);<br>} |
| `Tree * parent = e->parent(root);` | Handle it at parent's level |
| Excessive use of `child(index)` with `index>0`| Use `NodeIterator` or `nextTree()` while loops |
| Non-recursive bottom-up iteration | Iterate in the right direction to always change the downstream children |
| Handling ill-formatted expression during simplification | Implement check in `DeepCheckDimensions` or `DeepCheckListLength` to always assume valid expressions |
| Uncertain manipulation of multiple `Tree *` | Using `TreeRef`
