#ifndef ONLY_LAYOUTS
NODE(RackSimple, BASE, NARY16)
/* RackMemoized is a RackSimple with a struct to store its computed baseline and
 * size. It is used only temporarily inside Render methods. */
NODE(RackMemoized, BASE, NARY16, {
  OMG::unaligned_uint16_t width;
  OMG::unaligned_uint16_t height;
  OMG::unaligned_uint16_t baseline;
})
RANGE(RackLayout, RackSimpleLayout, RackMemoizedLayout)
#endif

NODE(VerticalOffset, BASE, 1, {
  bool isSubscript : 1;
  bool isPrefix : 1;
})

// Small spaces around a "+" for instance
NODE(OperatorSeparator, BASE)
// Small space between a value and its unit
NODE(UnitSeparator, BASE)
// Small space between groups of three digits
NODE(ThousandsSeparator, BASE)

RANGE(SeparatorLayout, OperatorSeparatorLayout, ThousandsSeparatorLayout)

NODE(AsciiCodePoint, BASE, 0, { char codePoint; })
NODE(UnicodeCodePoint, BASE, 0, { CodePoint codePoint; })

RANGE(CodePointLayout, AsciiCodePointLayout, UnicodeCodePointLayout)

NODE(CombinedCodePoints, BASE, 0, {
  CodePoint codePoint;
  CodePoint combiningCodePoint;
})

NODE(Abs, BASE, 1)
NODE(Ceil, BASE, 1)
NODE(Floor, BASE, 1)

NODE(VectorNorm, MATRIX, 1)

NODE(Parentheses, BASE, 1, {
  bool leftIsTemporary : 1;
  bool rightIsTemporary : 1;
})
NODE(CurlyBraces, BASE, 1, {
  bool leftIsTemporary : 1;
  bool rightIsTemporary : 1;
})

RANGE(SquareBrackets, AbsLayout, VectorNormLayout)
RANGE(AutocompletedPair, ParenthesesLayout, CurlyBracesLayout)
RANGE(Pair, AbsLayout, CurlyBracesLayout)

// TODO CondensedSum could draw the sigma symbol and have two children
// CondensedSum(sumSymbol, start, end)
NODE(CondensedSum, SUM_AND_PRODUCT, 3)

// Diff(Symbol, SymbolValue, Order, Derivand)
/* FIXME isNthDerivative wastes an extra metablock so it can be initialized by
 * the ktree */
NODE(Diff, DIFF, 4, {
  bool cursorIsOnTheLeft : 1;
  bool cursorIsOnTheBottom : 1;
  bool isNthDerivative;
})

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, INTEGRAL, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, SUM_AND_PRODUCT, 4)

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, SUM_AND_PRODUCT, 4)

// TODO replace by subscript ?
// Sequence(Symbol, SymbolMax, Function)
NODE(ListSequence, LIST, 3)

RANGE(ParametricLayout, DiffLayout, ListSequenceLayout)

NODE(Fraction, BASE, 2)
NODE(Point2D, BASE, 2)
NODE(Binomial, BASE, 2)
NODE(PtBinomial, PT_COMBINATORICS_LAYOUTS, 2)
NODE(PtPermute, PT_COMBINATORICS_LAYOUTS, 2)

NODE(Matrix, MATRIX, NARY2D)
NODE(Piecewise, PIECEWISE, NARY2D)
RANGE(GridLayout, MatrixLayout, PiecewiseLayout)

NODE(Conj, BASE, 1)
NODE(Root, BASE, 2)
NODE(Sqrt, BASE, 1)

RANGE(Layout, VerticalOffsetLayout, SqrtLayout)
RANGE(RackOrLayout, RackSimpleLayout, SqrtLayout)
