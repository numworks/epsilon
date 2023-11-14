NODE(Rack, NARY)
NODE(Fraction, 2)
NODE(VerticalOffset, 1)

NODE(CodePoint, 0, sizeof(CodePoint))
NODE(CombinedCodePoints, 0, 2 * sizeof(CodePoint))
// NODE(String)

NODE(AbsoluteValue, 1)
NODE(Ceiling, 1)
NODE(Floor, 1)
NODE(Parenthesis, 1)
NODE(CurlyBrace, 1)
NODE(VectorNorm, 1)

RANGE(Pair, AbsoluteValueLayout, VectorNormLayout)

// NODE(CondensedSum, 2)
NODE(Derivative, 3)
NODE(NthDerivative, 4)
NODE(Integral, 4)
NODE(Product, 4)
NODE(Sum, 4)

NODE(Binomial, 2)
NODE(PtBinomial, 2)
NODE(PtPermute, 2)

// TODO replace by subscript ?
NODE(ListSequence, 3)

NODE(Matrix, NARY2D)
NODE(Piecewise, NARY2D)
RANGE(GridLayout, MatrixLayout, PiecewiseLayout)

NODE(Conjugate, 1)
NODE(NthRoot, 2)
NODE(SquareRoot, 1)

RANGE(Layout, RackLayout, SquareRootLayout)
