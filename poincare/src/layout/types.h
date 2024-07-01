#ifndef ONLY_LAYOUTS
NODE(Rack, NARY16)
#endif

NODE(VerticalOffset, 1, {
  bool isSubscript : 1;
  bool isPrefix : 1;
})

NODE(OperatorSeparator)
NODE(ThousandSeparator)

RANGE(SeparatorLayout, OperatorSeparatorLayout, ThousandSeparatorLayout)

NODE(AsciiCodePoint, 0, { char codePoint; })
NODE(UnicodeCodePoint, 0, { CodePoint codePoint; })

RANGE(CodePointLayout, AsciiCodePointLayout, UnicodeCodePointLayout)

NODE(CombinedCodePoints, 0, {
  CodePoint codePoint;
  CodePoint combinedCodePoint;
})

NODE(Abs, 1)
NODE(Ceil, 1)
NODE(Floor, 1)
NODE(VectorNorm, 1)
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

// Diff(Symbol, SymbolValue, Derivand)
NODE(Diff, 3, { bool cursorIsOnTheLeft; })

// TODO: merge Diff and NthDiff like for expressions?
// Diff(Symbol, SymbolValue, Derivand, Order)
NODE(NthDiff, 4, {
  bool cursorIsOnTheLeft : 1;
  bool cursorIsOnTheBottom : 1;
})

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, 4)

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, 4)

// TODO replace by subscript ?
// Sequence(Symbol, SymbolMax, Function)
NODE(ListSequence, 3)

RANGE(ParametricLayout, DiffLayout, ListSequenceLayout)

NODE(Fraction, 2)
NODE(Point2D, 2)
NODE(Binomial, 2)
NODE(PtBinomial, 2)
NODE(PtPermute, 2)

NODE(Matrix, NARY2D)
NODE(Piecewise, NARY2D)
RANGE(GridLayout, MatrixLayout, PiecewiseLayout)

NODE(Conj, 1)
NODE(Root, 2)
NODE(Sqrt, 1)

RANGE(Layout, VerticalOffsetLayout, SqrtLayout)
RANGE(RackOrLayout, RackLayout, SqrtLayout)
