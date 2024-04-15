#Layout

- The Tree structure is bipartite: Racks contain Layouts that contain Racks
- The layout cursor is always at a given index inside a Rack, a selection is between two indexes of a Rack
- Layout can be defined with constexprs, for instance KAbsL("abc"_l) builds an Abs layout containing a Rack with 3 codepoints layouts (a,b and c)
- leftMargin has been replaced by real ThousandSeparator and OperatorSeparator layouts
- Metrics are gathered in render_metrics.h and position + drawing in render.cpp
- Rack and Layout are subclasses of Tree, Grid is a subclass of Layout
- VerticalOffset is a no-op layout, its parent Rack takes it into account in during positionOfChild
- To avoid state, empty visibility depends on whether the cursor is pointing to the empty Rack
- Matrices always have an additional row/col, it is displayed if the cursor is inside the Layout
