// 1 - Numbers

// 1.1 - Rationals

/* Defined in neg rationals, neg integers, pos integers, pos rationals in order
 * to allow for nested range definitions. */

// 1.1.a - Negative rationals
NODE(RationalNegBig, BASE, 0, {
  uint8_t numberOfNumeratorDigits;
  uint8_t numberOfDenominatorDigits;
  uint8_t digits[];  // most significant digit last
})

NODE(RationalNegShort, BASE, 0, {
  uint8_t absNumerator;
  uint8_t denominator;
})

// 1.2 - Integers

NODE(IntegerNegBig, BASE, 0, {
  uint8_t numberOfDigits;
  uint8_t digits[];  // most significant digit last
})

NODE(IntegerNegShort, BASE, 0, { uint8_t absValue; })

NODE(MinusOne, BASE)
NODE(Zero, BASE)
NODE(One, BASE)
NODE(Two, BASE)

NODE(IntegerPosShort, BASE, 0, { uint8_t value; })

NODE(IntegerPosBig, BASE, 0, {
  uint8_t numberOfDigits;
  uint8_t digits[];  // most significant digit last
})

RANGE(NegativeInteger, IntegerNegBig, Zero)
RANGE(PositiveInteger, Zero, IntegerPosBig)
RANGE(StrictlyNegativeInteger, IntegerNegBig, MinusOne)
RANGE(StrictlyPositiveInteger, One, IntegerPosBig)
RANGE(Integer, IntegerNegBig, IntegerPosBig)

// 1.1.b - Positive rationals

NODE(Half, BASE)  // Not in mathematical order
NODE(RationalPosShort, BASE, 0, {
  uint8_t numerator;
  uint8_t denominator;
})
NODE(RationalPosBig, BASE, 0, {
  uint8_t numberOfNumeratorDigits;
  uint8_t numberOfDenominatorDigits;
  uint8_t digits[];  // most significant digit last
})

RANGE(NegativeRational, RationalNegBig, Zero)
RANGE(PositiveRational, Zero, RationalPosBig)
RANGE(StrictlyNegativeRational, RationalNegBig, MinusOne)
RANGE(StrictlyPositiveRational, One, RationalPosBig)
RANGE(Rational, RationalNegBig, RationalPosBig)

// 1.3 - Floats

NODE(SingleFloat, BASE, 0, { float value; })
NODE(DoubleFloat, BASE, 0, { double value; })

RANGE(Float, SingleFloat, DoubleFloat)
RANGE(RationalOrFloat, RationalNegBig, DoubleFloat)

// 1.4 - Mathematical constants

NODE(EulerE, BASE)
NODE(Pi, BASE)

RANGE(MathematicalConstant, EulerE, Pi)

RANGE(Number, RationalNegBig, Pi)

// 2 - Order dependant expressions

NODE(Mult, BASE, NARY)

NODE(Pow, BASE, 2)

NODE(Add, BASE, NARY)

RANGE(Algebraic, RationalNegBig, Add)

NODE(UserSymbol, BASE, 0, {
  uint8_t size;
  char name[];
})

NODE(UserSequence, SEQUENCE, 1, {
  uint8_t size;
  char name[];
})

NODE(UserFunction, BASE, 1, {
  uint8_t size;
  char name[];
})

RANGE(UserNamed, UserSymbol, UserFunction)

NODE(Random, BASE, 0, { uint8_t seed; })
NODE(RandInt, BASE, 2, { uint8_t seed; })
NODE(RandIntNoRep, BASE, 3, { uint8_t seed; })

RANGE(Randomized, Random, RandIntNoRep)

NODE(Cos, BASE, 1)
NODE(Sin, BASE, 1)
NODE(Tan, BASE, 1)
// Trig(x,y) = {Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2, -Sin(x) if y=3}
NODE(Trig, BASE, 2)

RANGE(DirectTrigonometryFunction, Cos, Trig)

NODE(ACos, BASE, 1)
NODE(ASin, BASE, 1)
NODE(ATan, BASE, 1)
// ATrig(x,y) = {ACos(x) if y=0, ASin(x) if y=1}
NODE(ATrig, BASE, 2)
NODE(ATanRad, BASE, 1)

RANGE(InverseTrigonometryFunction, ACos, ATanRad)
RANGE(TrigonometryFunction, Cos, ATanRad)

NODE(Sec, TRIGONOMETRY_ADVANCED, 1)
NODE(Csc, TRIGONOMETRY_ADVANCED, 1)
NODE(Cot, TRIGONOMETRY_ADVANCED, 1)

RANGE(DirectAdvancedTrigonometryFunction, Sec, Cot)

NODE(ASec, TRIGONOMETRY_ADVANCED, 1)
NODE(ACsc, TRIGONOMETRY_ADVANCED, 1)
NODE(ACot, TRIGONOMETRY_ADVANCED, 1)

RANGE(InverseAdvancedTrigonometryFunction, ASec, ACot)
RANGE(AdvancedTrigonometryFunction, Sec, ACot)

NODE(CosH, TRIGONOMETRY_HYPERBOLIC, 1)
NODE(SinH, TRIGONOMETRY_HYPERBOLIC, 1)
NODE(TanH, TRIGONOMETRY_HYPERBOLIC, 1)

RANGE(DirectHyperbolicTrigonometryFunction, CosH, TanH)

NODE(ArCosH, TRIGONOMETRY_HYPERBOLIC, 1)
NODE(ArSinH, TRIGONOMETRY_HYPERBOLIC, 1)
NODE(ArTanH, TRIGONOMETRY_HYPERBOLIC, 1)

RANGE(InverseHyperbolicTrigonometryFunction, ArCosH, ArTanH)
RANGE(HyperbolicTrigonometryFunction, CosH, ArTanH)
RANGE(AnyTrigonometryFunction, Cos, ArTanH)

// Ln(value)
NODE(Ln, BASE, 1)
// LnUser(value)
NODE(LnUser, BASE, 1)
// Log(value)
NODE(Log, BASE, 1)
// LogBase(value, base)
NODE(LogBase, BASE, 2)

RANGE(Logarithm, Ln, LogBase)

// 3 - Other expressions in Alphabetic order

NODE(Abs, BASE, 1)
NODE(Arg, BASE, 1)
NODE(Binomial, BASE, 2)
NODE(Ceil, BASE, 1)
NODE(ComplexI, BASE)
NODE(Conj, BASE, 1)
// Decimal(Value, -Exponent) with Value a positive int and Exponent an int
NODE(Decimal, BASE, 2)

NODE(Distribution, DISTRIBUTION, NARY, {
  uint8_t distributionId;
  uint8_t methodId;
})

NODE(Div, BASE, 2)
NODE(Exp, BASE, 1)

NODE(EuclideanDivision, EUCLIDEAN_DIVISION, 2)
// Q=, R= node
NODE(EuclideanDivisionResult, EUCLIDEAN_DIVISION, 2)

NODE(Fact, BASE, 1)
NODE(Factor, BASE, 1)
NODE(Floor, BASE, 1)
NODE(Frac, BASE, 1)
NODE(GCD, BASE, NARY)
NODE(Im, BASE, 1)
NODE(Inf, BASE)
NODE(LCM, BASE, NARY)
NODE(MixedFraction, BASE, 2)
NODE(Opposite, BASE, 1)
NODE(PercentSimple, BASE, 1)
NODE(PercentAddition, BASE, 2)
NODE(Permute, BASE, 2)

/* - Polynomial P = a1*x^e1 + ... + an*x^en
 *   n = number of terms
 *   ei are unsigned digits
 *  | P TAG | n+1 | e1 | e2 | ... | en |
 *  This node has n+1 children:
 *  - the first child describes the variable x
 *  - the n following children describe the coefficients.
 *  Polynomials can be recursive (have polynomials children) */
NODE(Polynomial, BASE, NARY)

NODE(PowReal, BASE, 2)
NODE(Quo, BASE, 2)

NODE(Re, BASE, 1)
NODE(Rem, BASE, 2)
NODE(Round, BASE, 2)
NODE(Sign, BASE, 1)
NODE(Sqrt, BASE, 1)
NODE(Root, BASE, 2)
NODE(Sub, BASE, 2)
NODE(TrigDiff, BASE, 2)
NODE(Var, BASE, 0, {
  uint8_t id;
  uint8_t realSign;
  uint8_t imagSign;
})

// 4 - Parametric types

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, SUM_AND_PRODUCT, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, SUM_AND_PRODUCT, 4)

// Diff(Symbol, SymbolValue, Order, Derivand)
NODE(Diff, DIFF, 4)

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, INTEGRAL, 4)

// Integral(Symbol, LowerBound, UpperBound, Integrand,
//          IntegrandNearLowerBound, IntegrandNearUpperBound)
NODE(IntegralWithAlternatives, INTEGRAL, 6)

// Sequence(Symbol, SymbolMax, Function)
NODE(ListSequence, LIST, 3)

RANGE(Parametric, Sum, ListSequence)

// 5 - Matrix and vector builtins

NODE(Dot, MATRIX, 2)
NODE(Norm, MATRIX, 1)
NODE(Trace, MATRIX, 1)
NODE(Cross, MATRIX, 2)
NODE(Det, MATRIX, 1)
NODE(Dim, MATRIX, 1)
NODE(Identity, MATRIX, 1)
NODE(Inverse, MATRIX, 1)
NODE(Ref, MATRIX, 1)
NODE(Rref, MATRIX, 1)
NODE(Transpose, MATRIX, 1)
NODE(PowMatrix, MATRIX, 2)

/* - Matrix M
 * | Number of rows | Number of columns |
 * Children are ordered the row-major way */
NODE(Matrix, BASE, NARY2D)
// Matrix is in BASE because it is needed by the equation solver

RANGE(AMatrixOrContainsMatricesAsChildren, Dot, Matrix)

// 6 - Lists

NODE(List, BASE, NARY)
NODE(ListSort, LIST, 1)

// ListElement(List, ElementIndex)
NODE(ListElement, LIST, 2)
// ListSlice(List, ElementIndexStart, ElementIndexEnd)
NODE(ListSlice, LIST, 3)
NODE(Mean, LIST, 2)
NODE(StdDev, LIST, 2)
NODE(Median, LIST, 2)
NODE(Variance, LIST, 2)
NODE(SampleStdDev, LIST, 2)

RANGE(ListStatWithCoefficients, Mean, SampleStdDev)

NODE(Min, LIST, 1)
NODE(Max, LIST, 1)
NODE(ListSum, LIST, 1)
NODE(ListProduct, LIST, 1)

RANGE(ListToScalar, ListElement, ListProduct)

NODE(Point, POINT, 2)

// 7 - Booleans

NODE(False, BOOLEAN)
NODE(True, BOOLEAN)

RANGE(Boolean, False, True)

NODE(LogicalNot, BOOLEAN, 1)
NODE(LogicalAnd, BOOLEAN, 2)
NODE(LogicalOr, BOOLEAN, 2)
NODE(LogicalXor, BOOLEAN, 2)
NODE(LogicalNor, BOOLEAN, 2)
NODE(LogicalNand, BOOLEAN, 2)

RANGE(LogicalOperator, LogicalNot, LogicalNand)
RANGE(LogicalOperatorOrBoolean, False, LogicalNand)

NODE(Equal, BASE, 2)
NODE(NotEqual, BASE, 2)
NODE(InferiorEqual, BASE, 2)

NODE(Superior, BOOLEAN, 2)
NODE(Inferior, BOOLEAN, 2)
NODE(SuperiorEqual, BOOLEAN, 2)

RANGE(Inequality, InferiorEqual, SuperiorEqual)
RANGE(Comparison, Equal, SuperiorEqual)

// 8 - Units

NODE(Unit, UNIT, 0, {
  uint8_t representativeId;
  uint8_t prefixId;
})
NODE(PhysicalConstant, UNIT, 0, { uint8_t constantId; })
RANGE(UnitOrPhysicalConstant, Unit, PhysicalConstant)

// 9 - Order dependant expressions

NODE(Piecewise, PIECEWISE, NARY)

// Dep(expression, DepList(dep1, …, depN))
NODE(Dep, BASE, 2)
NODE(DepList, BASE, NARY)
// NonNull(x) = {0 if x ∈ ℂ*, undef} (only used in DepList)
NODE(NonNull, BASE, 1)
// Real(x) = {0 if x ∈ ℝ, undef} (only used in DepList)
NODE(Real, BASE, 1)
// RealPos(x) = {0 if x ∈ ℝ+, nonreal} (only used in DepList)
NODE(RealPos, BASE, 1)

NODE(Set, BASE, NARY)
NODE(Parentheses, BASE, 1)

// 10 - Undefined expressions
/* When an expression has multiple undefined children, we bubble up the
 * "biggest" one by default (NonReal < Undef).
 * These could be a single Type with a nodeValue, but it would require a
 * builtin/parser rework since undef/nonreal text would require node value
 * information, or a builtin subclass. */
NODE(NonReal, BASE)  // sqrt(-1) in Real ComplexMode
NODE(UndefFailedSimplification,
     BASE)                      // raised TreeStackOverFlow or IntegerOverFlow
NODE(UndefZeroPowerZero, BASE)  // 0^0
NODE(UndefZeroDivision, BASE)   // 1/0, tan(nπ/2)
NODE(UndefUnhandled, BASE)      // inf - inf, 0 * inf, unimplemented
NODE(UndefUnhandledDimension, BASE)  // [[1,2]] + [[1],[2]]
NODE(UndefBadType, BASE)             // non-integers in gcd,lcm,...
NODE(UndefOutOfDefinition, BASE)     // arg(0)
NODE(UndefNotDefined, BASE)  // Global variable that has not been defined
NODE(UndefForbidden, BASE)   // Forbidden by preferences, exam-modes, ...
NODE(Undef, BASE)            // Default

RANGE(Undefined, NonReal, Undef)

// 11 - Operations on expressions

NODE(Store, BASE, 2)
NODE(UnitConversion, BASE, 2)

RANGE(Expression, RationalNegBig, UnitConversion)

// SequenceExplicit(formula, firstRank)
NODE(SequenceExplicit, BASE, 2)
// SequenceSingleRecurrence(formula, firstRank, initialCondition)
NODE(SequenceSingleRecurrence, BASE, 3)
// SequenceDoubleRecurrence(formula, firstRank, initialCondition,
// initialCondition)
NODE(SequenceDoubleRecurrence, BASE, 4)

RANGE(Sequence, SequenceExplicit, SequenceDoubleRecurrence)

/* Used as intermediary step in beautification to hide context dependant nodes
 * during context-less simplification and approximation. */
NODE(AngleUnitContext, BASE, 1, { uint8_t angleUnit; })

// TODO: should this really be here ?
NODE(PointOfInterest, BASE, 0, {
  double abscissa;
  double ordinate;
  uint32_t data;
  uint8_t interest;
  bool inverted;
  uint8_t subCurveIndex;
})
