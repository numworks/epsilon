# Layout

The Tree structure is bipartite: Racks contain Layouts that contain Racks.

The layout cursor is always at a given index inside a Rack, a selection is between two indexes of a Rack.

Layouts can be defined with constexprs, for instance `KAbsL("abc"_l)` builds an Abs layout containing a Rack with 3 codepoints layouts "a", "b" and "c".

`ThousandSeparator` and `OperatorSeparator` layouts are inserted in Racks by the layoutter.

Rack and Layout are subclasses of Tree, Grid is a subclass of Layout.
They have no virtuality and are only here to make clear what is expected in signatures.

An empty Rack displays itself as a yellow square, unless the cursor is pointing to it.

Grids (matrices and piecewise layouts) always possess an additional row and column filled with empty racks called placeholders.
They are displayed as gray squares, if the cursor is in the grid or one of its descendants.

When a Rack encounter a VerticalOffset when iterating over their children, it places it relatively to the previous child height. VerticalOffset does nothing by itself.
If there is no previous child it displays an empty yellow square.

The derivative (and nth derivative) layout have a boolean flag to tell whether the cursor is in the right or the left copy of the variable layout.
Its variable child will be placed (positionOfChild) to the side where the cursor is and a copy is rendered on the other side.
If the cursor is not inside the variable layout, the flag has no importance. 
Nth derivative has a similar flag for the order.

There is no memoization of the computed layout properties inside poincare-junior.
Algorithms on racks and matrices iterates their children in the correct order to avoid computing several times the same properties.
