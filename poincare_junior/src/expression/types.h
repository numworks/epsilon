// 1 - Numbers

// 1.1 - Integers

/* - Zero Z (same for One, Two, Half, MinusOne)
 * | Z TAG | */
TYPE(Zero, 0)
TYPE(One, 0)
TYPE(Two, 0)
TYPE(MinusOne, 0)

/* - IntegerShort IS
 * | IS TAG | SIGNED DIGIT0 | */
TYPE(IntegerShort, 0)

/* - Integer(Pos/Neg)Big IB: most significant digit last
 * | IB TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | */
TYPE(IntegerPosBig, 0)
TYPE(IntegerNegBig, 0)

RANGE(Integer, Zero, IntegerNegBig)

// 1.2 - Rationals

TYPE(Half, 0)

/* - RationShort RS
 * | RS TAG | SIGNED DIGIT | UNSIGNED DIGIT | */
TYPE(RationalShort, 0)

/* - Rational(Pos/Neg)Big RB
 * | RB TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED
 * NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | */
TYPE(RationalPosBig, 0)
TYPE(RationalNegBig, 0)

RANGE(Rational, Zero, RationalNegBig)

// 1.3 - Other numbers

/* - Float F
 * | F TAG | VALUE (4 bytes) | */
TYPE(SingleFloat, 0)

/* - Double D
 * | D TAG | VALUE (8 bytes) | */
TYPE(DoubleFloat, 0)

RANGE(Float, SingleFloat, DoubleFloat)

/* - Constant C
 * | C TAG | TYPE | */
TYPE(Constant, 0)

RANGE(Number, Zero, Constant)

// 2 - Order dependant expressions

/* - Multiplication M (same for Addition, Set, List, RackLayout)
 * | M TAG | NUMBER OF CHILDREN | */
TYPE(Multiplication, NARY)

/* - Power P (same for Factorial, Subtraction, Division, FractionLayout,
 * ParenthesisLayout, VerticalOffsetLayout) | P TAG | */
TYPE(Power, 2)

TYPE(Addition, NARY)

RANGE(Algebraic, Zero, Addition)

/* - UserSymbol US (same for UserFunction, UserSequence)
 * | US TAG | NUMBER CHARS | CHAR0 | ... | CHARN | */
TYPE(UserSymbol, 0)
TYPE(UserFunction, 0)
TYPE(UserSequence, 0)

RANGE(UserNamed, UserSymbol, UserSequence)

/* - Variable V
 * | V TAG | ID | */
TYPE(Variable, 0)

TYPE(Sine, 1)
TYPE(Cosine, 1)
TYPE(Tangent, 1)

TYPE(Infinity, 0)

// 3 - Other expressions in Alphabetic order

TYPE(Abs, 1)
TYPE(ArcCosine, 1)
TYPE(ArcSine, 1)
TYPE(ArcTangent, 1)
TYPE(Ceiling, 1)
TYPE(Complex, 2)
TYPE(ComplexArgument, 1)
TYPE(Conjugate, 1)

/* - Decimal DC
 * | DC TAG | NUMBER DIGITS AFTER ZERO | */
TYPE(Decimal, 1)
TYPE(Division, 2)
TYPE(Exponential, 1)
TYPE(Factorial, 1)
TYPE(Factor, 1)
TYPE(Floor, 1)
TYPE(FracPart, 1)
TYPE(GCD, NARY)
TYPE(ImaginaryPart, 1)
TYPE(LCM, NARY)
TYPE(Ln, 1)
TYPE(Log, 1)
TYPE(Logarithm, 2)
TYPE(Opposite, 1)

/* - Polynomial P = a1*x^e1 + ... + an*x^en
 *   n = number of terms
 *   ei are unsigned digits
 *  | P TAG | n+1 | e1 | e2 | ... | en |
 *  This node has n+1 children:
 *  - the first child describes the variable x
 *  - the n following children describe the coefficients.
 *  Polynomials can be recursive (have polynomials children) */
TYPE(Polynomial, NARY)

TYPE(PowerReal, 2)
TYPE(Quotient, 2)
TYPE(RealPart, 1)
TYPE(Remainder, 2)
TYPE(Round, 2)
TYPE(Sign, 1)
TYPE(SquareRoot, 1)
TYPE(Subtraction, 2)
TYPE(Trig, 2)
TYPE(TrigDiff, 2)

// 4 - Parametric types

TYPE(Sum, 4)
TYPE(Product, 4)
TYPE(Derivative, 3)
TYPE(Integral, 4)

RANGE(Parametric, Sum, Integral)

// 5 - Matrix and vector builtins

TYPE(Dot, 2)
TYPE(Norm, 1)
TYPE(Trace, 1)
TYPE(Cross, 2)
TYPE(Det, 1)
TYPE(Dim, 1)
TYPE(Identity, 1)
TYPE(Inverse, 1)
TYPE(Ref, 1)
TYPE(Rref, 1)
TYPE(Transpose, 1)
TYPE(PowerMatrix, 2)

/* - Matrix M
 * | M TAG | NUMBER OF ROWS | NUMBER OF COLUMNS |
 * Children are ordered the row-major way */
TYPE(Matrix, NARY2D)

RANGE(AMatrixOrContainsMatricesAsChildren, Dot, Matrix)

// 6 - Order dependant expressions
/* - Unit U
 * | U TAG | REPRESENTATIVE ID | PREFIX ID | */
TYPE(Unit, 0)
TYPE(Dependency, 2)
TYPE(List, NARY)
TYPE(Set, NARY)
TYPE(Nonreal, 0)
TYPE(Undefined, 0)

RANGE(Expression, Zero, Undefined)

/* TODO:
 * - Short integers could be coded on n-bytes (with n static) instead of 1-byte.
 * Choosing n = 4 and aligning the node could be useful?
 * - aligning all nodes on 4 bytes might speed up every computation
 */
