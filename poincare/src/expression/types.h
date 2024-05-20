// 1 - Numbers

// 1.1 - Rationals

/* Defined in neg rationals, neg integers, pos integers, pos rationals order to
 * allow for nested range definitions. */

// 1.1.a - Negative rationals

/* - Rational(Pos/Neg)Big RB
 * | RB TAG | NUMBER NUMERATOR_DIGITS | NUMBER_DENOMINATOR_DIGITS | UNSIGNED
 * NUMERATOR DIGIT0 | ... | UNSIGNED DENOMINATOR_DIGIT0 | ... | */
NODE(RationalNegBig, 0, 2)

/* - RationalShort
 * | RS TAG | UINT8 ABSOLUTE NUMERATOR | UINT8 DENOMINATOR | */
NODE(RationalNegShort, 0, 2)

// 1.2 - Integers

/* - Integer(Pos/Neg)Big IB: most significant digit last
 * | IB TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | */
NODE(IntegerNegBig, 0, 1)

/* - Integer(Pos/Neg)Short IS
 * | IS TAG | DIGIT0 | */
NODE(IntegerNegShort, 0, 1)

NODE(MinusOne)
NODE(Zero)
NODE(One)
NODE(Two)

NODE(IntegerPosShort, 0, 1)

NODE(IntegerPosBig, 0, 1)

RANGE(IntegerNegative, IntegerNegBig, Zero)
RANGE(IntegerPositive, Zero, IntegerPosBig)
RANGE(IntegerStrictlyNegative, IntegerNegBig, MinusOne)
RANGE(IntegerStrictlyPositive, One, IntegerPosBig)
RANGE(Integer, IntegerNegBig, IntegerPosBig)

// 1.1.b - Positive rationals

NODE(Half)  // Not in mathematical order
NODE(RationalPosShort, 0, 2)
NODE(RationalPosBig, 0, 2)

RANGE(RationalNegative, RationalNegBig, Zero)
RANGE(RationalPositive, Zero, RationalPosBig)
RANGE(RationalStrictlyNegative, RationalNegBig, MinusOne)
RANGE(RationalStrictlyPositive, One, RationalPosBig)
RANGE(Rational, RationalNegBig, RationalPosBig)

// 1.3 - Floats

/* - Float F
 * | F TAG | VALUE (4 bytes) | */
NODE(SingleFloat, 0, sizeof(float))

/* - Double D
 * | D TAG | VALUE (8 bytes) | */
NODE(DoubleFloat, 0, sizeof(double))

RANGE(Float, SingleFloat, DoubleFloat)

// 1.4 - Mathematical constants

NODE(EulerE)
NODE(Pi)

RANGE(MathematicalConstant, EulerE, Pi)

RANGE(Number, RationalNegBig, Pi)

// 2 - Order dependant expressions

/* - Multiplication M (same for Addition, Set, List)
 * | M TAG | NUMBER OF CHILDREN | */
NODE(Mult, NARY)

/* - Power P (same for Factorial, Subtraction, Division) | P TAG | */
NODE(Pow, 2)

NODE(Add, NARY)

RANGE(Algebraic, RationalNegBig, Add)

/* - UserSymbol US, CHARN must be 0.
 * | US TAG | SIGN | NUMBER CHARS | CHAR0 | ... | CHARN | */
NODE(UserSymbol, 0, 2)
/* - UserFunction UF (same for UserSequence) CHARN must be 0.
 * | UF TAG | NUMBER CHARS | CHAR0 | ... | CHARN | */
NODE(UserFunction, 1, 1)
NODE(UserSequence, 1, 1)

RANGE(UserNamed, UserSymbol, UserSequence)

/* - Variable V
 * | V TAG | ID | Sign | */
NODE(Var, 0, 2)
NODE(Inf)

// 3 - Other expressions in Alphabetic order

NODE(Abs, 1)
NODE(ACos, 1)
NODE(ACot, 1)
NODE(ACsc, 1)
NODE(ASec, 1)
NODE(ASin, 1)
NODE(ATan, 1)
NODE(ATanRad, 1)
NODE(ATrig, 2)
NODE(Binomial, 2)
NODE(Ceil, 1)
NODE(ComplexI)
NODE(Arg, 1)
NODE(Conj, 1)
NODE(Csc, 1)
NODE(Cos, 1)
NODE(Cot, 1)

/* - Decimal DC
 * | DC TAG | NUMBER DIGITS AFTER ZERO | */
NODE(Decimal, 1, 1)

/* - Distribution DS
 * | DS TAG | Distribution::Type | DistributionMethod::Type | */
NODE(Distribution, NARY, 2)
NODE(Div, 2)
NODE(Exp, 1)
NODE(Fact, 1)
NODE(Factor, 1)
NODE(Floor, 1)
NODE(Frac, 1)
NODE(GCD, NARY)
NODE(ArCosH, 1)
NODE(ArSinH, 1)
NODE(ArTanH, 1)
NODE(CosH, 1)
NODE(SinH, 1)
NODE(TanH, 1)
NODE(Im, 1)
NODE(LCM, NARY)
// Ln(value)
NODE(Ln, 1)
// LnReal(value)
NODE(LnReal, 1)
// Log(value)
NODE(Log, 1)
// Logarithm(value, base)
NODE(Logarithm, 2)
NODE(MixedFraction, 2)
NODE(Opposite, 1)
NODE(PercentSimple, 1)
NODE(PercentAddition, 2)
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

NODE(PowReal, 2)
NODE(Quo, 2)

/* - RandomNode RN
 * | RN TAG | LOCAL RANDOM SEED | */
NODE(Random, 0, 1)
NODE(RandInt, 2, 1)
NODE(RandIntNoRep, 3, 1)

RANGE(RandomNode, Random, RandIntNoRep)

NODE(Re, 1)
NODE(Rem, 2)
NODE(Round, 2)
NODE(Sec, 1)
NODE(Sign, 1)
NODE(Sin, 1)
NODE(Sqrt, 1)
NODE(Root, 2)
NODE(Sub, 2)
NODE(Tan, 1)
NODE(TanRad, 1)
NODE(Trig, 2)
NODE(TrigDiff, 2)

// 4 - Parametric types

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, 4)

// Diff(Symbol, SymbolValue, Derivand)
NODE(Diff, 3)

// NthDiff(Symbol, SymbolValue, Order, Derivand)
NODE(NthDiff, 4)

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, 4)

// Sequence(Symbol, SymbolMax, Function)
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
NODE(PowMatrix, 2)

/* - Matrix M
 * | M TAG | NUMBER OF ROWS | NUMBER OF COLUMNS |
 * Children are ordered the row-major way */
NODE(Matrix, NARY2D)

RANGE(AMatrixOrContainsMatricesAsChildren, Dot, Matrix)

// 6 - Lists

NODE(List, NARY)
NODE(ListSort, 1)

// ListElement(List, ElementIndex)
NODE(ListElement, 2)
// ListSlice(List, ElementIndexStart, ElementIndexEnd)
NODE(ListSlice, 3)
NODE(Mean, 2)
NODE(StdDev, 2)
NODE(Median, 2)
NODE(Variance, 2)
NODE(SampleStdDev, 2)

RANGE(ListStatWithCoefficients, Mean, SampleStdDev)

NODE(Min, 1)
NODE(Max, 1)
NODE(ListSum, 1)
NODE(ListProduct, 1)

RANGE(ListToScalar, ListElement, ListProduct)

NODE(Point, 2)

// 7 - Booleans

NODE(False)
NODE(True)

RANGE(Boolean, False, True)

NODE(LogicalNot, 1)
NODE(LogicalAnd, 2)
NODE(LogicalOr, 2)
NODE(LogicalXor, 2)
NODE(LogicalNor, 2)
NODE(LogicalNand, 2)

RANGE(LogicalOperator, LogicalNot, LogicalNand)
RANGE(LogicalOperatorOrBoolean, False, LogicalNand)

NODE(Equal, 2)
NODE(NotEqual, 2)
NODE(Superior, 2)
NODE(Inferior, 2)
NODE(SuperiorEqual, 2)
NODE(InferiorEqual, 2)

RANGE(Inequality, Superior, InferiorEqual)
RANGE(Comparison, Equal, InferiorEqual)

// 8 - Units

/* - Unit U
 * | U TAG | REPRESENTATIVE ID | PREFIX ID | */
NODE(Unit, 0, 2)
/* - Physical constant PC
 * | PC TAG | CONSTANT INDEX | */
NODE(PhysicalConstant, 0, 1)

// 9 - Order dependant expressions

NODE(Dependency, 2)
NODE(Piecewise, NARY)
// Used in dependencies only, shall never be systematic simplified.
NODE(Set, NARY)
NODE(Parenthesis, 1)
NODE(Empty)  // TODO_PCJ temporary

// 10 - Undefined expressions
/* When an expression has multiple undefined children, we bubble up the
 * "biggest" one by default (NonReal < Undef).
 * These could be a single Type with a nodeValue, but it would require a
 * builtin/parser rework since undef/nonreal text would require node value
 * information, or a builtin subclass. */
NODE(NonReal)                  // sqrt(-1) in Real ComplexMode
NODE(UndefZeroPowerZero)       // 0^0
NODE(UndefZeroDivision)        // 1/0, tan(nπ/2)
NODE(UndefUnhandled)           // inf - inf, 0 * inf, unimplemented
NODE(UndefUnhandledDimension)  // [[1,2]] + [[1],[2]]
NODE(UndefBadType)             // non-integers in gcd,lcm,...
NODE(UndefOutOfDefinition)     // arg(0)
NODE(UndefNotDefined)          // Global variable that has not been defined
NODE(Undef)                    // Default

RANGE(Undefined, NonReal, Undef)

// 11 - Operations on expressions

NODE(Store, 2)
NODE(UnitConversion, 2)

RANGE(Expression, RationalNegBig, UnitConversion)

/* - PointOfInterest PI
 * | PI TAG | ABSCISSA | ORDINATE | DATA | INTEREST | INVERTED | SUBCURVEINDEX |
 */
NODE(PointOfInterest, 0,
     sizeof(double) + sizeof(double) + sizeof(uint32_t) + sizeof(uint8_t) +
         sizeof(bool) + sizeof(uint8_t))

/* TODO:
 * - Short integers could be coded on n-bytes (with n static) instead of 1-byte.
 * Choosing n = 4 and aligning the node could be useful?
 * - aligning all nodes on 4 bytes might speed up every computation
 */
