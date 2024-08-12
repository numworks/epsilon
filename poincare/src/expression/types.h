// 1 - Numbers

// 1.1 - Rationals

/* Defined in neg rationals, neg integers, pos integers, pos rationals order to
 * allow for nested range definitions. */

// 1.1.a - Negative rationals

NODE(RationalNegBig, 0, {
  uint8_t numberOfNumeratorDigits;
  uint8_t numberOfDenominatorDigits;
  uint8_t digits[];  // most significant digit last
})

NODE(RationalNegShort, 0, {
  uint8_t absNumerator;
  uint8_t denominator;
})

// 1.2 - Integers

NODE(IntegerNegBig, 0, {
  uint8_t numberOfDigits;
  uint8_t digits[];  // most significant digit last
})

NODE(IntegerNegShort, 0, { uint8_t absValue; })

NODE(MinusOne)
NODE(Zero)
NODE(One)
NODE(Two)

NODE(IntegerPosShort, 0, { uint8_t value; })

NODE(IntegerPosBig, 0, {
  uint8_t numberOfDigits;
  uint8_t digits[];  // most significant digit last
})

RANGE(NegativeInteger, IntegerNegBig, Zero)
RANGE(PositiveInteger, Zero, IntegerPosBig)
RANGE(StrictlyNegativeInteger, IntegerNegBig, MinusOne)
RANGE(StrictlyPositiveInteger, One, IntegerPosBig)
RANGE(Integer, IntegerNegBig, IntegerPosBig)

// 1.1.b - Positive rationals

NODE(Half)  // Not in mathematical order
NODE(RationalPosShort, 0, {
  uint8_t numerator;
  uint8_t denominator;
})
NODE(RationalPosBig, 0, {
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

NODE(SingleFloat, 0, { float value; })
NODE(DoubleFloat, 0, { double value; })

RANGE(Float, SingleFloat, DoubleFloat)
RANGE(RationalOrFloat, RationalNegBig, DoubleFloat)

// 1.4 - Mathematical constants

NODE(EulerE)
NODE(Pi)

RANGE(MathematicalConstant, EulerE, Pi)

RANGE(Number, RationalNegBig, Pi)

// 2 - Order dependant expressions

NODE(Mult, NARY)

NODE(Pow, 2)

NODE(Add, NARY)

RANGE(Algebraic, RationalNegBig, Add)

NODE(UserSymbol, 0, {
  uint8_t size;
  char name[];
})
NODE(UserFunction, 1, {
  uint8_t size;
  char name[];
})
NODE(UserSequence, 1, {
  uint8_t size;
  char name[];
})

RANGE(UserNamed, UserSymbol, UserSequence)

NODE(Random, 0, { uint8_t seed; })
NODE(RandInt, 2, { uint8_t seed; })
NODE(RandIntNoRep, 3, { uint8_t seed; })

RANGE(Randomized, Random, RandIntNoRep)

NODE(Cos, 1)
NODE(Tan, 1)
NODE(Sin, 1)
// Trig(x,y) = {Cos(x) if y=0, Sin(x) if y=1, -Cos(x) if y=2, -Sin(x) if y=3}
NODE(Trig, 2)

RANGE(DirectTrigonometryFunction, Cos, Trig)

NODE(ACos, 1)
NODE(ATan, 1)
NODE(ASin, 1)
NODE(ATrig, 2)
NODE(ATanRad, 1)

RANGE(IndirectTrigonometryFunction, ACos, ATanRad)
RANGE(TrigonometryFunction, Cos, ATanRad)

NODE(Sec, 1)
NODE(Csc, 1)
NODE(Cot, 1)

RANGE(DirectAdvancedTrigonometryFunction, Sec, Cot)

NODE(ASec, 1)
NODE(ACsc, 1)
NODE(ACot, 1)

RANGE(IndirectAdvancedTrigonometryFunction, ASec, ACot)
RANGE(AdvancedTrigonometryFunction, Sec, ACot)

NODE(CosH, 1)
NODE(SinH, 1)
NODE(TanH, 1)

RANGE(DirectHyperbolicTrigonometryFunction, CosH, TanH)

NODE(ArCosH, 1)
NODE(ArSinH, 1)
NODE(ArTanH, 1)

RANGE(IndirectHyperbolicTrigonometryFunction, ArCosH, ArTanH)
RANGE(HyperbolicTrigonometryFunction, CosH, ArTanH)
RANGE(AnyTrigonometryFunction, Cos, ArTanH)

// Ln(value)
NODE(Ln, 1)
// LnReal(value)
NODE(LnReal, 1)
// Log(value)
NODE(Log, 1)
// LogBase(value, base)
NODE(LogBase, 2)

RANGE(Logarithm, Ln, LogBase)

// 3 - Other expressions in Alphabetic order

NODE(Abs, 1)
NODE(Binomial, 2)
NODE(Ceil, 1)
NODE(ComplexI)
NODE(Arg, 1)
NODE(Conj, 1)
// Decimal(Value, -Exponent)
NODE(Decimal, 2)
NODE(Distribution, NARY, {
  uint8_t distributionId;
  uint8_t methodId;
})
NODE(Div, 2)
NODE(Exp, 1)
NODE(Fact, 1)
NODE(Factor, 1)
NODE(Floor, 1)
NODE(Frac, 1)
NODE(GCD, NARY)
NODE(Im, 1)
NODE(Inf)
NODE(LCM, NARY)
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

NODE(Re, 1)
NODE(Rem, 2)
NODE(Round, 2)
NODE(Sign, 1)
NODE(Sqrt, 1)
NODE(Root, 2)
NODE(Sub, 2)
NODE(TrigDiff, 2)
NODE(Var, 0, {
  uint8_t id;
  uint8_t sign;
})

// 4 - Parametric types

// Sum(Symbol, LowerBound, UpperBound, Function)
NODE(Sum, 4)

// Product(Symbol, LowerBound, UpperBound, Function)
NODE(Product, 4)

// Diff(Symbol, SymbolValue, Order, Derivand)
NODE(Diff, 4)

// Integral(Symbol, LowerBound, UpperBound, Integrand)
NODE(Integral, 4)

// Integral(Symbol, LowerBound, UpperBound, Integrand,
//          IntegrandNearLowerBound, IntegrandNearUpperBound)
NODE(IntegralWithAlternatives, 6)

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
 * | Number of rows | Number of columns |
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

NODE(Unit, 0, {
  uint8_t representativeId;
  uint8_t prefixId;
})
NODE(PhysicalConstant, 0, { uint8_t constantId; })

// 9 - Order dependant expressions

NODE(Piecewise, NARY)

// Dep(expression, DepList(dep1, …, depN))
NODE(Dep, 2)
NODE(DepList, NARY)

NODE(Set, NARY)
NODE(Parentheses, 1)
NODE(Empty)

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
NODE(UndefForbidden)           // Forbidden by preferences, exam-modes, ...
NODE(Undef)                    // Default

RANGE(Undefined, NonReal, Undef)

// 11 - Operations on expressions

NODE(Store, 2)
NODE(UnitConversion, 2)

RANGE(Expression, RationalNegBig, UnitConversion)

// SequenceExplicit(formula, firstRank)
NODE(SequenceExplicit, 2)
// SequenceSingleRecurrence(formula, firstRank, initialCondition)
NODE(SequenceSingleRecurrence, 3)
// SequenceDoubleRecurrence(formula, firstRank, initialCondition,
// initialCondition)
NODE(SequenceDoubleRecurrence, 4)

RANGE(Sequence, SequenceExplicit, SequenceDoubleRecurrence)

// TODO: should this really be here ?
NODE(PointOfInterest, 0, {
  double abscissa;
  double ordinate;
  uint32_t data;
  uint8_t interest;
  bool inverted;
  uint8_t subCurveIndex;
})
