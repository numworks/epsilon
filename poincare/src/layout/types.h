#ifndef ONLY_LAYOUTS
NODE(RackSimple, NARY16)
/* RackMemoized is a RackSimple with a struct to store its computed baseline and
 * size. It is used only temporarily inside Render methods. */
NODE(RackMemoized, NARY16, {
  uint16_t width;
  uint16_t height;
  uint16_t baseline;
})
RANGE(RackLayout, RackSimpleLayout, RackMemoizedLayout)
#endif

NODE(VerticalOffset, 1, {
  bool isSubscript : 1;
  bool isPrefix : 1;
})

// Small spaces around a "+" for instance
NODE(OperatorSeparator)
// Small space between a value and its unit
NODE(UnitSeparator)
// Small space between groups of three digits
NODE(ThousandsSeparator)

RANGE(SeparatorLayout, OperatorSeparatorLayout, ThousandsSeparatorLayout)

NODE(AsciiCodePoint, 0, { char codePoint; })
NODE(UnicodeCodePoint, 0, { CodePoint codePoint; })

RANGE(CodePointLayout, AsciiCodePointLayout, UnicodeCodePointLayout)

NODE(CombinedCodePoints, 0, {
  CodePoint codePoint;
  CodePoint combiningCodePoint;
})

NODE(Abs, 1)
NODE(Ceil, 1)
NODE(Floor, 1)

#if POINCARE_MATRIX
NODE(VectorNorm, 1)
#else
UNDEF_NODE(VectorNorm, 1)
#endif

NODE(Parentheses, 1, {
  bool leftIsTemporary : 1;
  bool rightIsTemporary : 1;
})
NODE(CurlyBraces, 1, {
  bool leftIsTemporary : 1;
  bool rightIsTemporary : 1;
})

RANGE(SquareBrackets, AbsLayout, VectorNormLayout)
RANGE(AutocompletedPair, ParenthesesLayout, CurlyBracesLayout)
RANGE(Pair, AbsLayout, CurlyBracesLayout)

// TODO CondensedSum could draw the sigma symbol and have two children
// CondensedSum(sumSymbol, start, end)
NODE(CondensedSum, 3)

// Diff(Symbol, SymbolValue, Order, Derivand)
/* FIXME isNthDerivative wastes an extra metablock so it can be initialized by
 * the ktree */
NODE(Diff, 4, {
  bool cursorIsOnTheLeft : 1;
  bool cursorIsOnTheBottom : 1;
  bool isNthDerivative;
})

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, 4)

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, 4)

#if POINCARE_LIST
// TODO replace by subscript ?
// Sequence(Symbol, SymbolMax, Function)
NODE(ListSequence, 3)
#else
UNDEF_NODE(ListSequence, 3)
#endif

RANGE(ParametricLayout, DiffLayout, ListSequenceLayout)

NODE(Fraction, 2)
NODE(Point2D, 2)
NODE(Binomial, 2)
#if POINCARE_PT_COMBINATORICS_LAYOUTS
NODE(PtBinomial, 2)
NODE(PtPermute, 2)
#else
UNDEF_NODE(PtBinomial, 2)
UNDEF_NODE(PtPermute, 2)
#endif

#if POINCARE_MATRIX
NODE(Matrix, NARY2D)
#else
UNDEF_NODE(Matrix, NARY2D)
#endif
#if POINCARE_PIECEWISE
NODE(Piecewise, NARY2D)
#else
UNDEF_NODE(Piecewise, NARY2D)
#endif
#if POINCARE_MATRIX || POINCARE_PIECEWISE
RANGE(GridLayout, MatrixLayout, PiecewiseLayout)
#else
UNDEF_RANGE(GridLayout, MatrixLayout, PiecewiseLayout)
#endif

NODE(Conj, 1)
NODE(Root, 2)
NODE(Sqrt, 1)

RANGE(Layout, VerticalOffsetLayout, SqrtLayout)
RANGE(RackOrLayout, RackSimpleLayout, SqrtLayout)
