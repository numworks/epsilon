// 1 - Numbers

/* - Zero Z (same for One, Two, Half, MinusOne, TreeBorder)
 * | Z TAG | */
TYPE(Zero)
TYPE(One)
TYPE(Two)
TYPE(Half)
TYPE(MinusOne)

/* - IntegerShort IS
 * | IS TAG | SIGNED DIGIT0 | */
TYPE(IntegerShort)

/* - Integer(Pos/Neg)Big IB: most significant digit last
 * | IB TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | */
TYPE(IntegerPosBig)
TYPE(IntegerNegBig)

/* - RationShort RS
 * | RS TAG | SIGNED DIGIT | UNSIGNED DIGIT | */
TYPE(RationalShort)

/* - Rational(Pos/Neg)Big RB
 * | RB TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED
 * NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | */
TYPE(RationalPosBig)
TYPE(RationalNegBig)

/* - Float F (same for CodePointLayout)
 * | F TAG | VALUE (4 bytes) | */
TYPE(Float)

ALIAS(NumberOfNumbersExpression = Float + 1)

// 2 - Order dependant expressions

/* - Multiplication M (same for Addition, Set, List, RackLayout)
 * | M TAG | NUMBER OF CHILDREN | */
TYPE(Multiplication)

/* - Power P (same for Factorial, Subtraction, Division, FractionLayout,
 * ParenthesisLayout, VerticalOffsetLayout) | P TAG | */
TYPE(Power)

TYPE(Addition)
TYPE(Factorial)
TYPE(Division)

/* - Constant C
 * | C TAG | TYPE | */
TYPE(Constant)

/* - UserSymbol US (same for UserFunction, UserSequence)
 * | US TAG | NUMBER CHARS | CHAR0 | ... | CHARN | */
TYPE(UserSymbol)
TYPE(UserFunction)
TYPE(UserSequence)

/* - Variable V
 * | V TAG | ID | */
TYPE(Variable)

TYPE(Sine)
TYPE(Cosine)
TYPE(Tangent)

TYPE(Infinite)

// 3 - Other expressions in Alphabetic order

TYPE(Abs)
TYPE(ArcCosine)
TYPE(ArcSine)
TYPE(ArcTangent)
TYPE(Complex)
TYPE(ComplexArgument)
TYPE(Conjugate)
TYPE(Decimal)
TYPE(Exponential)
TYPE(ImaginaryPart)
TYPE(Ln)
TYPE(Log)
TYPE(Logarithm)

/* - Polynomial P = a1*x^e1 + ... + an*x^en
 *   n = number of terms
 *   ei are unsigned digits
 *  | P TAG | n+1 | e1 | e2 | ... | en |
 *  This node has n+1 children:
 *  - the first child describes the variable x
 *  - the n following children describe the coefficients.
 *  Polynomials can be recursive (have polynomials children) */
TYPE(Polynomial)

TYPE(PowerReal)
TYPE(RealPart)
TYPE(SquareRoot)
TYPE(Subtraction)
TYPE(Trig)
TYPE(TrigDiff)

// 4 - Parametric types

TYPE(Sum)
TYPE(Product)
TYPE(Derivative)
TYPE(Integral)

// 5 - Matrix and vector builtins

TYPE(Dot)
TYPE(Norm)
TYPE(Trace)
TYPE(Cross)
TYPE(Det)
TYPE(Dim)
TYPE(Identity)
TYPE(Inverse)
TYPE(Ref)
TYPE(Rref)
TYPE(Transpose)
TYPE(PowerMatrix)

/* - Matrix M
 * | M TAG | NUMBER OF ROWS | NUMBER OF COLUMNS |
 * Children are ordered the row-major way */
TYPE(Matrix)

ALIAS(FirstMatrix = Dot)
ALIAS(LastMatrix = Matrix)

// 6 - Order dependant expressions
/* - Unit U
 * | U TAG | REPRESENTATIVE ID | PREFIX ID | */
TYPE(Unit)
TYPE(Dependency)
TYPE(List)
TYPE(Set)
TYPE(Nonreal)
TYPE(Undefined)

ALIAS(NumberOfExpressions = Undefined + 1)

/* TODO:
 * - Are Zero, One etc useful?
 * - Short integers could be coded on n-bytes (with n static) instead of 1-byte.
 * Choosing n = 4 and aligning the node could be useful?
 * - aligning all nodes on 4 bytes might speed up every computation
 */
