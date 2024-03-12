# Poincare Junior

PoincareJunior is Epsilon's new computer algebra engine.

## Documentation

More in-depth documentation is available [here](doc).

## Rules

Keep PoincareJunior as independent as possible from the rest of Epsilon.

Avoid heavy use of virtuality and v-table.

Only edit expression and layout trees in the EditionPool.

Avoid calls to `child(index)` : use `nextTree()` or `NodeIterator`.

When editing children of a tree, iterate in the right direction to always change the downstream children
