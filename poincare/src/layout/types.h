NODE(Rack, NARY16)
NODE(VerticalOffset, 1, 1)

NODE(OperatorSeparator)
NODE(ThousandSeparator)

RANGE(SeparatorLayout, OperatorSeparatorLayout, ThousandSeparatorLayout)

NODE(AsciiCodePoint, 0, 1)
NODE(UnicodeCodePoint, 0, sizeof(CodePoint))

RANGE(CodePointLayout, AsciiCodePointLayout, UnicodeCodePointLayout)

NODE(CombinedCodePoints, 0, 2 * sizeof(CodePoint))
// TODO Do we need a StringLayout ?

NODE(Abs, 1)
NODE(Ceil, 1)
NODE(Floor, 1)
NODE(VectorNorm, 1)
NODE(Parenthesis, 1, 1)
NODE(CurlyBrace, 1, 1)

RANGE(SquareBracketPair, AbsLayout, VectorNormLayout)
RANGE(AutocompletedPair, ParenthesisLayout, CurlyBraceLayout)
RANGE(Pair, Abs, CurlyBraceLayout)

// TODO CondensedSum could draw the sigma symbol and have two children
NODE(CondensedSum, 3)
// Diff(Derivand, Symbol, SymbolValue)
NODE(Diff, 3, 1)
// Diff(Derivand, Symbol, SymbolValue, Order)
NODE(NthDiff, 4, 1)
// Integral(Integrand, Symbol, LowerBound, UpperBound)
NODE(Integral, 4)
// Product(Function, Symbol, LowerBound, UpperBound)
NODE(Product, 4)
// Sum(Function, Symbol, LowerBound, UpperBound, )
NODE(Sum, 4)
// TODO replace by subscript ?
// Sequence(Function, Symbol, SymbolMax)
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

RANGE(Layout, RackLayout, SqrtLayout)
