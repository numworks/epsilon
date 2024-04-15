# Layout

## Tree Structure

### Racks and Layouts

The Tree structure is bipartite:
- **Racks** have **Layouts** children
- **Layouts** have **Racks** children (or no children like codepoints layouts).

#### Example
This layout:
```
3 + 4
----- * 2
  5
```
is represented by:
```
rack
├─ fraction_layout
│  ├─ rack
│  │  ├─ '3'
│  │  ├─ '+'
│  │  └─ '4'
│  └─ rack
│     └─ '5'
├─ '*'
└─ '2'
```

### Class hierarchy

**Rack** and **Layout** are subclasses of **Tree**, **Grid** is a subclass of **Layout**.
They have no virtuality and are only here to make clear what is expected in signatures.

```
# Class hierarchy
Tree -> Rack
     -> Layout -> Grid
```

## Layout cursor

### Simple cursor

The **layout cursor** is always at a given index inside a Rack.

#### Example
In this layout (where `|` is the cursor):
```
3 + |4
------ * 2
  5
```
the cursor is here
```
rack
├─ fraction_layout
│  ├─ rack
│  │  ├─ '3'
│  │  ├─ '+'
│  │  ├──> cursor in this rack at index 2
│  │  └─ '4'
│  └─ rack
│     └─ '5'
├─ '*'
└─ '2'
```

### Cursor selection

A **cursor selection** is between two indexes of a Rack.

#### Example
In this layout (where the selection is between the `|`):
```
3 |+ 4|
------ * 2
  5
```
the selection is here
```
rack
├─ fraction_layout
│  ├─ rack
│  │  ├─ '3'
│  │  ├──> cursor selection in this rack between index 1 ...
│  │  ├─ '+'
│  │  ├─ '4'
│  │  └──> ... and index 4
│  └─ rack
│     └─ '5'
├─ '*'
└─ '2'
```

## Constexpr definition

### KTrees literals

Layouts can be defined in constexprs.

Most of it is defined using literals:
- `KRackL()` builds a rack
- `KAbsL()` builds an absolute value layout
- `KSumL()` builds a sum layout
- etc.

Special case: You can use the operator `"foo"_l` to build a rack layout only containing codepoints.

_See the [layout/k_tree.h file](../src/layout/k_tree.h) to have the full list of all literals._

#### Example
This expression
```constexpr foo = KAbsL("abc"_l)```

builds an Abs layout containing a Rack with 3 codepoints layouts "a", "b" and "c".

```
abs_layout
└─ rack
   ├─ 'a'
   ├─ 'b'
   └─ 'c'
```

### Concatenation

You can use `^` to concatenate 2 racks, 2 layouts, or a rack and a layout.

#### Example
This expression
```constexpr foo = KAbsL("abc"_l) ^ "*2"_l```

concatenates the absolute value with `*2`

```
rack
├─ abs_layout
│  └─ rack
│     ├─ 'a'
│     ├─ 'b'
│     └─ 'c'
├─ '*'
└─ '2'
```

## Layoutter

There is no memoization of the computed layout properties inside poincare-junior.

Algorithms on racks and matrices iterates their children in the correct order to avoid computing several times the same properties.

### Separators

`ThousandSeparator` and `OperatorSeparator` layouts are inserted in Racks by the layoutter.

#### Example
This layout
```
# 12345+6789
rack
├─ '1'
├─ '2'
├─ '3'
├─ '4'
├─ '5'
├─ '+'
├─ '6'
├─ '7'
├─ '8'
└─ '9'
```
is turned into this one by the layoutter:
```
# 12 345 + 6 789
rack
├─ '1'
├─ '2'
├─ ThousandSeparator
├─ '3'
├─ '4'
├─ '5'
├─ OperatorSeparator
├─ '+'
├─ OperatorSeparator
├─ '6'
├─ ThousandSeparator
├─ '7'
├─ '8'
└─ '9'
```

### Empty layouts

An empty Rack displays itself as a yellow square, unless the cursor is pointing to it.

Grids (matrices and piecewise layouts) always possess an additional row and column filled with empty racks called placeholders.
They are displayed as gray squares, if the cursor is in the grid or one of its descendants.

#### Example
```
# Tree of
#  2
# ---
#  Ø
fraction_layout
├─ rack
│  └─ '2'
└─ rack (EMPTY)
   └──> if cursor is not here, display rack as a yellow square
```

### VerticalOffset

When a Rack encounter a `VerticalOffset` when iterating over their children, it places it relatively to the anchor child height. `VerticalOffset` does nothing by itself.

The vertical offset has two properties (`isSubScript` and `isPrefix`) that respectively determine how to place it (up or down) and what is its anchor (previous or next layout).

If there is no anchor, an empty yellow square is displayed instead.

#### Example
The expression `2+3^5` is represented by the tree:
```
rack
├─ '2'
├─ '+'
├─ '3' ──> anchor of the vertical offset layout
└─ vertical_offset_layout(isSubscript: false, isPrefix: false)
   └─ rack
      └─ '5'
```

### Derivative

The derivative (and nth derivative) layout have a boolean flag to tell whether the cursor is in the right or the left copy of the variable layout.

Its variable child will be placed (`positionOfChild`) to the side where the cursor is and a copy is rendered on the other side.

If the cursor is not inside the variable layout, the flag has no importance.
Nth derivative has a similar flag for the order.
