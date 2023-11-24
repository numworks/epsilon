// 1 - Numbers

// 1.1 - Integers

/* - Zero Z (same for One, Two, Half, MinusOne)
 * | Z TAG | */
NODE(Zero)
NODE(One)
NODE(Two)
NODE(MinusOne)

/* - IntegerShort IS
 * | IS TAG | SIGNED DIGIT0 | */
NODE(IntegerShort, 0, 1)

/* - Integer(Pos/Neg)Big IB: most significant digit last
 * | IB TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | */
NODE(IntegerPosBig, 0, 1)
NODE(IntegerNegBig, 0, 1)

RANGE(Integer, Zero, IntegerNegBig)

// 1.2 - Rationals

NODE(Half)

/* - RationShort RS
 * | RS TAG | SIGNED DIGIT | UNSIGNED DIGIT | */
NODE(RationalShort, 0, 2)

/* - Rational(Pos/Neg)Big RB
 * | RB TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED
 * NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | */
NODE(RationalPosBig, 0, 2)
NODE(RationalNegBig, 0, 2)

RANGE(Rational, Zero, RationalNegBig)

// 1.3 - Other numbers

/* - Float F
 * | F TAG | VALUE (4 bytes) | */
NODE(SingleFloat, 0, sizeof(float))

/* - Double D
 * | D TAG | VALUE (8 bytes) | */
NODE(DoubleFloat, 0, sizeof(double))

RANGE(Float, SingleFloat, DoubleFloat)

/* - Constant C
 * | C TAG | NODE | */
NODE(Constant, 0, 1)

RANGE(Number, Zero, Constant)

// 2 - Order dependant expressions

/* - Multiplication M (same for Addition, Set, List, RackLayout)
 * | M TAG | NUMBER OF CHILDREN | */
NODE(Multiplication, NARY)

/* - Power P (same for Factorial, Subtraction, Division, FractionLayout,
 * ParenthesisLayout, VerticalOffsetLayout) | P TAG | */
NODE(Power, 2)

NODE(Addition, NARY)

RANGE(Algebraic, Zero, Addition)

/* - UserSymbol US (same for UserFunction, UserSequence)
 * | US TAG | NUMBER CHARS | CHAR0 | ... | CHARN | */
NODE(UserSymbol, 0, 1)
NODE(UserFunction, 0, 1)
NODE(UserSequence, 0, 1)

RANGE(UserNamed, UserSymbol, UserSequence)

/* - Variable V
 * | V TAG | ID | */
NODE(Variable, 0, 1)

NODE(Sine, 1)
NODE(Cosine, 1)
NODE(Tangent, 1)

NODE(Infinity)

// 3 - Other expressions in Alphabetic order

NODE(Abs, 1)
NODE(ArcCosine, 1)
NODE(ArcSine, 1)
NODE(ArcTangent, 1)
NODE(ATrig, 2)
NODE(Binomial, 2)
NODE(Ceiling, 1)
NODE(Complex, 2)
NODE(ComplexArgument, 1)
NODE(Conjugate, 1)

/* - Decimal DC
 * | DC TAG | NUMBER DIGITS AFTER ZERO | */
NODE(Decimal, 1, 1)
NODE(Division, 2)
NODE(Exponential, 1)
NODE(Factorial, 1)
NODE(Factor, 1)
NODE(Floor, 1)
NODE(FracPart, 1)
NODE(GCD, NARY)
NODE(ImaginaryPart, 1)
NODE(LCM, NARY)
NODE(Ln, 1)
NODE(Log, 1)
NODE(Logarithm, 2)
NODE(Opposite, 1)
NODE(Permute, 2)

/* - Polynomial P = a1*x^e1 + ... + an*x^en
 *   n = number of terms
 *   ei are unsigned digits
 *  | P TAG | n+1 | e1 | e2 | ... | en |
 *  This node has n+1 children:
 *  - the first child describes the variable x
 *  - the n following children describe the coefficients.
 *  Polynomials can be recursive (have polynomials children) */
NODE(Polynomial, NARY)

NODE(PowerReal, 2)
NODE(Quotient, 2)

NODE(Random, 0)
NODE(RandInt, 2)
NODE(RandIntNoRep, 3)

RANGE(RandomNode, Random, RandIntNoRep)

NODE(RealPart, 1)
NODE(Remainder, 2)
NODE(Round, 2)
NODE(Sign, 1)
NODE(SquareRoot, 1)
NODE(Subtraction, 2)
NODE(Trig, 2)
NODE(TrigDiff, 2)

// 4 - Parametric types

NODE(Sum, 4)
NODE(Product, 4)
NODE(Derivative, 3)
NODE(Integral, 4)
NODE(ListSequence, 3)

RANGE(Parametric, Sum, ListSequence)

// 5 - Matrix and vector builtins

NODE(Dot, 2)
NODE(Norm, 1)
NODE(Trace, 1)
NODE(Cross, 2)
NODE(Det, 1)
NODE(Dim, 1)
NODE(Identity, 1)
NODE(Inverse, 1)
NODE(Ref, 1)
NODE(Rref, 1)
NODE(Transpose, 1)
NODE(PowerMatrix, 2)

/* - Matrix M
 * | M TAG | NUMBER OF ROWS | NUMBER OF COLUMNS |
 * Children are ordered the row-major way */
NODE(Matrix, NARY2D)

RANGE(AMatrixOrContainsMatricesAsChildren, Dot, Matrix)

// 6 - Lists

NODE(List, NARY)
NODE(ListSort, 1)

NODE(ListAccess, 2)
NODE(Mean, 2)
NODE(StdDev, 2)
NODE(Median, 2)
NODE(Variance, 2)
NODE(SampleStdDev, 2)
NODE(Minimum, 1)
NODE(Maximum, 1)
NODE(ListSum, 1)
NODE(ListProduct, 1)

RANGE(ListToScalar, ListAccess, ListProduct)

// 7 - Order dependant expressions
/* - Unit U
 * | U TAG | REPRESENTATIVE ID | PREFIX ID | */
NODE(Unit, 0, 2)
NODE(Dependency, 2)
NODE(Set, NARY)
NODE(Nonreal)
NODE(Undefined)

RANGE(Expression, Zero, Undefined)

/* TODO:
 * - Short integers could be coded on n-bytes (with n static) instead of 1-byte.
 * Choosing n = 4 and aligning the node could be useful?
 * - aligning all nodes on 4 bytes might speed up every computation
 */
