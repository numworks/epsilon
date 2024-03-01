NODE(Rack, NARY16)
NODE(VerticalOffset, 1, 1)

NODE(OperatorSeparator)
NODE(ThousandSeparator)

RANGE(SeparatorLayout, OperatorSeparatorLayout, ThousandSeparatorLayout)

NODE(CodePoint, 0, sizeof(CodePoint))
NODE(CombinedCodePoints, 0, 2 * sizeof(CodePoint))
// TODO Do we need a StringLayout ?

NODE(AbsoluteValue, 1)
NODE(Ceiling, 1)
NODE(Floor, 1)
NODE(VectorNorm, 1)
NODE(Parenthesis, 1, 1)
NODE(CurlyBrace, 1, 1)

RANGE(SquareBracketPair, AbsoluteValueLayout, VectorNormLayout)
RANGE(AutocompletedPair, ParenthesisLayout, CurlyBraceLayout)
RANGE(Pair, AbsoluteValueLayout, CurlyBraceLayout)

// TODO CondensedSum could draw the sigma symbol and have two children
NODE(CondensedSum, 3)
NODE(Derivative, 3, 1)
NODE(NthDerivative, 4, 1)
NODE(Integral, 4)
NODE(Product, 4)
NODE(Sum, 4)
// TODO replace by subscript ?
NODE(ListSequence, 3)

RANGE(ParametricLayout, DerivativeLayout, ListSequenceLayout)

NODE(Fraction, 2)
NODE(Point2D, 2)
NODE(Binomial, 2)
NODE(PtBinomial, 2)
NODE(PtPermute, 2)

NODE(Matrix, NARY2D)
NODE(Piecewise, NARY2D)
RANGE(GridLayout, MatrixLayout, PiecewiseLayout)

NODE(Conjugate, 1)
NODE(NthRoot, 2)
NODE(SquareRoot, 1)

RANGE(Layout, RackLayout, SquareRootLayout)
