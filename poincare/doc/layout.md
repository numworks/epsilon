# Layout

## Tree Structure

### Racks and Layouts

The Tree structure is bipartite:
- `Racks` have `Layouts` children
- `Layouts` have `Racks` children (or no children like codepoints layouts).

#### Example
This layout:
```
3 + 4
----- * 2
  5
```
is represented by:
```
Rack
├─ FractionLayout
│  ├─ Rack
│  │  ├─ '3'
│  │  ├─ '+'
│  │  └─ '4'
│  └─ Rack
│     └─ '5'
├─ '*'
└─ '2'
```

### Class hierarchy

`Rack` and `Layout` are subclasses of `Tree`, `Grid` is a subclass of `Layout`.
They have no virtuality and are only here to make clear what is expected in signatures.

```
# Class hierarchy
Tree -> Rack
     -> Layout -> Grid
```

## Layout cursor

### Simple cursor

The layout cursor is always at a given index inside a `Rack`, corresponding to the index of the child on the right after the cursor (the cursor is at the left of the child). This index may be equal to `rack->numberOfChildren()` to point to the right of the last child.

#### Example
In this layout (where `|` is the cursor):
```
3 + |4
------ * 2
  5
```
the cursor is here
```
Rack
├─ FractionLayout
│  ├─ Rack
│  │  ├─ '3'
│  │  ├─ '+'
│  │  ├──> cursor in this rack at index 2
│  │  └─ '4'
│  └─ Rack
│     └─ '5'
├─ '*'
└─ '2'
```

### Cursor selection

A cursor selection is between two indexes of a `Rack`.

#### Example
In this layout (where the selection is between the `|`):
```
3 |+ 4|
------ * 2
  5
```
the selection is here
```
Rack
├─ FractionLayout
│  ├─ Rack
│  │  ├─ '3'
│  │  ├──> cursor selection in this rack between index 1 ...
│  │  ├─ '+'
│  │  ├─ '4'
│  │  └──> ... and index 4
│  └─ Rack
│     └─ '5'
├─ '*'
└─ '2'
```

## Constexpr definition

### KTrees literals

Layouts can be defined in constexprs.

Most of them have a constructor named K + the name of the associated builtin + L:
- `KRackL()` builds a rack
- `KAbsL()` builds an absolute value layout
- `KSumL()` builds a sum layout
- etc.

Special case: You can use the operator `"foo"_l` to build a rack containing only codepoints.

_See the [layout/k_tree.h file](../src/layout/k_tree.h) for the full list of all literals._

#### Example
This expression
```constexpr KTree foo = KAbsL("abc"_l)```

builds an Abs layout containing a Rack with 3 codepoints layouts "a", "b" and "c".

```
AbsLayout
└─ Rack
   ├─ 'a'
   ├─ 'b'
   └─ 'c'
```

### Concatenation

You can use `^` to concatenate 2 racks, 2 layouts, or a rack and a layout.

#### Example
This expression
```constexpr KTree foo = KAbsL("abc"_l) ^ "*2"_l```

concatenates the absolute value with `*2`

```
Rack
├─ AbsLayout
│  └─ Rack
│     ├─ 'a'
│     ├─ 'b'
│     └─ 'c'
├─ '*'
└─ '2'
```

## Layouter

There is no memoization of layout properties (size, baseline, absolute position) inside poincare-junior.

Racks and matrices computes their children properties instead of each children computing them. It avoids computing several times the same properties.


### Separators

`ThousandSeparator` and `OperatorSeparator` layouts are inserted in `Racks` by the layouter.

#### Example
The expression
```
# 12345+6789
Add
├─ Integer 12345
└─ Integer 6789
```

is turned into this layout by the layoutter:
```
# 12 345 + 6 789
Rack
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

Separators are ignored by the char* serialization and by the parser.

### Empty layouts

An empty rack displays itself as a yellow square, unless the cursor is pointing to it.

Grids (matrices and piecewise layouts) always possess an additional row and column filled with empty racks called placeholders.
They are displayed as gray squares, if the cursor is in the grid or one of its descendants.

#### Example
```
# Tree of
#  2
# ---
#  Ø
FractionLayout
├─ Rack
│  └─ '2'
└─ Rack (EMPTY)
   └──> if cursor is not here, display rack as a yellow square
```

### VerticalOffset

A `VerticalOffset` is related to a layout positionned before or after him in the same rack. Let's call this layout the anchor of the `VerticalOffset`.

When a rack encounters a `VerticalOffset` while iterating over their children, it places it relatively to its anchor height. `VerticalOffset` does nothing by itself.

The vertical offset has two properties (`isSubScript` and `isPrefix`) that respectively determine how to place it (up or down) and what is its anchor (previous or next layout).

If there is no anchor, an empty yellow square is displayed instead.

#### Example
The expression `2+3^5` is represented by the tree:
```
Rack
├─ '2'
├─ '+'
├─ '3' ──> anchor of the vertical offset layout
└─ VerticalOffsetLayout(isSubscript: false, isPrefix: false)
   └─ Rack
      └─ '5'
```

### Derivative

$$\frac{d^{\textcolor{plum}{n}}}{d{\textcolor{skyblue}{x}}^{\textcolor{plum}{n}}} (f(x)) \bigg|_{{\textcolor{skyblue}{x}}=a}$$

The derivative (and nth derivative) layout have a boolean flag to tell whether the cursor is in the right or the left spot of the variable layout.

The variable child will be placed at the spot where the cursor is and a copy is rendered at the other spot.

If the cursor is not inside the variable layout, the flag has no importance.

Nth derivative has a similar flag for the order.
