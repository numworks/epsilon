#include <poincare/power.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/constant.h>
#include <poincare/cosine.h>
#include <poincare/division.h>
#include <poincare/infinity.h>
#include <poincare/nth_root.h>
#include <poincare/opposite.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/parenthesis.h>
#include <poincare/sine.h>
#include <poincare/square_root.h>
#include <poincare/symbol.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/serialization_helper.h>
#include <cmath>
#include <math.h>
#include <ion.h>
#include <assert.h>

namespace Poincare {

// Properties
ExpressionNode::Sign PowerNode::sign(Context * context) const {
  if (Expression::ShouldStopProcessing()) {
    return Sign::Unknown;
  }
  if (childAtIndex(0)->sign(context) == Sign::Positive && childAtIndex(1)->sign(context) != Sign::Unknown) {
    return Sign::Positive;
  }
  if (childAtIndex(0)->sign(context) == Sign::Negative && childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (r->denominator().isOne()) {
      assert(!Integer::Division(r->signedNumerator(), Integer(2)).remainder.isOverflow());
      if (Integer::Division(r->signedNumerator(), Integer(2)).remainder.isZero()) {
        return Sign::Positive;
      } else {
        return Sign::Negative;
      }
    }
  }
  return Sign::Unknown;
}

Expression PowerNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  return Power(this).setSign(s, context, complexFormat, angleUnit, target);
}

int PowerNode::polynomialDegree(Context & context, const char * symbolName) const {
  int deg = ExpressionNode::polynomialDegree(context, symbolName);
  if (deg == 0) {
    return deg;
  }
  int op0Deg = childAtIndex(0)->polynomialDegree(context, symbolName);
  if (op0Deg < 0) {
    return -1;
  }
  if (childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (!r->denominator().isOne() || Number(r).sign() == Sign::Negative) {
      return -1;
    }
    Integer numeratorInt = r->signedNumerator();
    if (Integer::NaturalOrder(numeratorInt, Integer(Integer::k_maxExtractableInteger)) > 0) {
      return -1;
    }
    op0Deg *= numeratorInt.extractedInt();
    return op0Deg;
  }
  return -1;
}

int PowerNode::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  return Power(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

bool PowerNode::isReal(Context & context) const {
  ExpressionNode * base = childAtIndex(0);
  ExpressionNode * index = childAtIndex(1);
  // Both base and index are real and:
  // - either base > 0
  // - or index is an integer
  if (base->isReal(context) &&
      index->isReal(context) &&
      (base->sign(&context) == Sign::Positive ||
       (index->type() == ExpressionNode::Type::Rational && static_cast<RationalNode *>(index)->denominator().isOne()))) {
    return true;
  }
  return false;
}

// Private

template<typename T>
Complex<T> PowerNode::compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) {
  std::complex<T> result;
  if (c.imag() == 0.0 && d.imag() == 0.0 && c.real() != 0.0 && (c.real() > 0.0 || std::round(d.real()) == d.real())) {
    /* pow: (R+, R) -> R+ (2^1.3 ~ 2.46)
     * pow: (R-, N) -> R+ ((-2)^3 = -8)
     * In these cases we rather use std::pow(double, double) because:
     * - pow on complexes is not as precise as pow on double: for instance,
     *   pow(complex<double>(2.0,0.0), complex<double>(3.0,0.0) = complex(7.9999999999999982,0.0)
     *   and pow(2.0,3.0) = 8.0
     * - Using complex pow, std::pow(2.0, 1000) = (INFINITY, NAN).
     *   Openbsd pow of a positive real and another real has a undefined
     *   imaginary when the real result is infinity.
     * However, we exclude c == 0 because std:pow(0.0, 0.0) = 1.0 and we would
     * rather have 0^0 = undef. */
    result = std::complex<T>(std::pow(c.real(), d.real()));
  } else {
    result = std::pow(c, d);
  }
  /* Openbsd trigonometric functions are numerical implementation and thus are
   * approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to
   * avoid weird results as e(i*pi) = -1+6E-17*i, we compute the argument of
   * the result of c^d and if arg ~ 0 [Pi], we discard the residual imaginary
   * part and if arg ~ Pi/2 [Pi], we discard the residual real part. */
  return Complex<T>::Builder(ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument(result));
}

// Layout

Layout PowerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  ExpressionNode * indiceOperand = childAtIndex(1);
  // Delete eventual parentheses of the indice in the pretty print
  if (indiceOperand->type() == ExpressionNode::Type::Parenthesis) {
    indiceOperand = indiceOperand->childAtIndex(0);
  }
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(VerticalOffsetLayout::Builder(
        indiceOperand->createLayout(floatDisplayMode, numberOfSignificantDigits),
        VerticalOffsetLayoutNode::Type::Superscript),
      result.numberOfChildren(),
      result.numberOfChildren(),
      nullptr);
  return result;
}

// Serialize

bool PowerNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->denominator().isOne()) {
    return true;
  }
  Type types[] = {Type::Power, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 6);
}

int PowerNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "^");
}

// Simplify

Expression PowerNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Power(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Expression PowerNode::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  return Power(this).shallowBeautify(context, complexFormat, angleUnit, target);
}

int PowerNode::simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  int baseComparison = SimplificationOrder(childAtIndex(0), e, ascending, canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one = Rational::Builder(1);
  return SimplificationOrder(childAtIndex(1), one.node(), ascending, canBeInterrupted);
}

int PowerNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted) const {
  assert(e->numberOfChildren() > 0);
  int baseComparison = SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending, canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  assert(e->numberOfChildren() > 1);
  return SimplificationOrder(childAtIndex(1), e->childAtIndex(1), ascending, canBeInterrupted);
}

Expression PowerNode::denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Power(this).denominator(context, complexFormat, angleUnit);
}

// Evaluation
template<typename T> MatrixComplex<T> PowerNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
  return MatrixComplex<T>::Undefined();
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d, Preferences::ComplexFormat complexFormat) {
  if (m.numberOfRows() != m.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  T power = Complex<T>::Builder(d).toScalar();
  if (std::isnan(power) || std::isinf(power) || power != (int)power || std::fabs(power) > k_maxApproximatePowerMatrix) {
    return MatrixComplex<T>::Undefined();
  }
  if (power < 0) {
    MatrixComplex<T> inverse = m.inverse();
    if (inverse.isUninitialized()) {
      return MatrixComplex<T>::Undefined();
    }
    Complex<T> minusC = Complex<T>::Builder(-d);
    MatrixComplex<T> result = PowerNode::computeOnMatrixAndComplex(inverse, minusC.stdComplex(), complexFormat);
    return result;
  }
  MatrixComplex<T> result = MatrixComplex<T>::CreateIdentity(m.numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (Expression::ShouldStopProcessing()) {
      return MatrixComplex<T>::Undefined();
    }
    result = MultiplicationNode::computeOnMatrices<T>(result, m, complexFormat);
  }
  return result;
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
  return MatrixComplex<T>::Undefined();
}

// Power

Expression Power::setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  if (childAtIndex(0).sign(context) == ExpressionNode::Sign::Negative) {
    Expression result = Power::Builder(childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, complexFormat, angleUnit, target), childAtIndex(1));
    replaceWithInPlace(result);
    return result.shallowReduce(*context, complexFormat, angleUnit, target);
  }
  return *this;
}

int Power::getPolynomialCoefficients(Context & context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg <= 0) {
    return Expression::defaultGetPolynomialCoefficients(context, symbolName, coefficients);
  }
  /* Here we only consider the case x^4 as privateGetPolynomialCoefficients is
   * supposed to be called after reducing the expression. */
  if (childAtIndex(0).type() == ExpressionNode::Type::Symbol
      && strcmp(childAtIndex(0).convert<Symbol>().name(), symbolName) == 0
      && childAtIndex(1).type() == ExpressionNode::Type::Rational)
  {
    Rational r = childAtIndex(1).convert<Rational>();
    if (!r.integerDenominator().isOne() || r.sign() == ExpressionNode::Sign::Negative) {
      return -1;
    }
    Integer num = r.unsignedIntegerNumerator();
    if (Integer::NaturalOrder(num, Integer(Integer::k_maxExtractableInteger)) > 0) {
      return -1;
    }
    int n = num.extractedInt();
    if (n <= k_maxPolynomialDegree) {
      for (int i = 0; i < n; i++) {
        coefficients[i] = Rational::Builder(0);
      }
      coefficients[n] = Rational::Builder(1);
      return n;
    }
  }
  return -1;
}

Expression Power::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {

  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

#if MATRIX_EXACT_REDUCING
#if 0
  /* Step 0: get rid of matrix */
  if (childAtIndex(1)->type() == ExpressionNode::Type::Matrix) {
    return replaceWith(new Undefined::Builder(), true);
  }
  if (childAtIndex(0)->type() == ExpressionNode::Type::Matrix) {
    Matrix * mat = static_cast<Matrix *>(childAtIndex(0));
    if (childAtIndex(1)->type() != ExpressionNode::Type::Rational || !static_cast<const Rational *>(childAtIndex(1))->denominator().isOne()) {
      return replaceWith(new Undefined::Builder(), true);
    }
    Integer exponent = static_cast<const Rational *>(childAtIndex(1))->numerator();
    if (mat->numberOfRows() != mat->numberOfColumns()) {
      return replaceWith(new Undefined::Builder(), true);
    }
    if (exponent.isNegative()) {
      childAtIndex(1)->setSign(Sign::Positive, context, complexFormat, angleUnit);
      Expression * newMatrix = shallowReduce(context, complexFormat, angleUnit, target);
      Expression * parent = newMatrix->parent();
      MatrixInverse * inv = new MatrixInverse(newMatrix, false);
      parent->replaceOperand(newMatrix, inv, false);
      return inv;
    }
    if (Integer::NaturalOrder(exponent, Integer(k_maxExactPowerMatrix)) > 0) {
      return this;
    }
    int exp = exponent.extractedInt(); // Ok, because 0 < exponent < k_maxExactPowerMatrix
    Matrix * id = Matrix::CreateIdentity(mat->numberOfRows());
    if (exp == 0) {
      return replaceWith(id, true);
    }
    Multiplication * result = new Multiplication::Builder(id, mat->clone());
    // TODO: implement a quick exponentiation
    for (int k = 1; k < exp; k++) {
      result->addOperand(mat->clone());
    }
    replaceWith(result, true);
    return result->shallowReduce(context, complexFormat, angleUnit, target);
  }
#endif
#endif

  Expression power = *this;
  Expression base = childAtIndex(0);
  Expression index = childAtIndex(1);
  /* Step 0: if both children are true unresolved complexes, the result is not simplified. TODO? */
  if (!base.isReal(context) && base.type() != ExpressionNode::Type::ComplexCartesian && !index.isReal(context) && index.type() != ExpressionNode::Type::ComplexCartesian) {
    return *this;
  }

  /* Step 1: We handle simple cases as x^0, x^1, 0^x and 1^x first for 2 reasons:
   * - we can assert after this step that there is no division by 0:
   *   for instance, 0^(-2)->undefined
   * - we save computational time by early escaping for these cases. */
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    const Rational b = childAtIndex(1).convert<Rational>();
    // x^0
    if (b.isZero()) {
      // 0^0 = undef or (±inf)^0 = undef
      if ((childAtIndex(0).type() == ExpressionNode::Type::Rational && childAtIndex(0).convert<Rational>().isZero()) || childAtIndex(0).type() == ExpressionNode::Type::Infinity) {
        Expression result = Undefined::Builder();
        replaceWithInPlace(result);
        return result;
      }
      // x^0
      if (target == ExpressionNode::ReductionTarget::User || childAtIndex(0).isNumber()) {
        /* Warning: if the ReductionTarget is User, in all other cases but 0^0,
         * we replace x^0 by one. This is almost always true except when x = 0.
         * However, not substituting x^0 by one would prevent from simplifying
         * many expressions like x/x->1. */
        Expression result = Rational::Builder(1);
        replaceWithInPlace(result);
        return result;
      }
    }
    // x^1
    if (b.isOne()) {
      Expression result = childAtIndex(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational a = childAtIndex(0).convert<Rational>();
    // 0^x
    if (a.isZero()) {
      // 0^x with x > 0 = 0
      if (childAtIndex(1).sign(&context) == ExpressionNode::Sign::Positive) {
        Expression result = Rational::Builder(0);
        replaceWithInPlace(result);
        return result;
      }
      // 0^x with x < 0 = undef
      if (childAtIndex(1).sign(&context) == ExpressionNode::Sign::Negative) {
        Expression result = Undefined::Builder();
        replaceWithInPlace(result);
        return result;
      }
    }
    // 1^x = 1 if x != ±inf
    if (a.isOne() && !childAtIndex(1).recursivelyMatchesInfinity(context)) {
      Expression result = Rational::Builder(1);
      replaceWithInPlace(result);
      return result;
    }
  }

  /* We do not apply some rules to a^b if the parent node is a logarithm of same
   * base a. In this case there is a simplication of form ln(e^(3^(1/2))->3^(1/2).
   */
  bool letPowerAtRoot = parentIsALogarithmOfSameBase();

  /* Step 2: we now bubble up ComplexCartesian, we handle different cases:
   * At least, one child is a ComplexCartesian and the other is either a
   * ComplexCartesian or real. */

  ComplexCartesian complexBase;
  ComplexCartesian complexIndex;
  ComplexCartesian result;
  /* First, (x+iy)^q with q special values
   * For q = -1, 1/2, -1/2, n with n integer < 10, we avoid introducing arctangent
   * by using the formula (r*e^(i*th))^(a+ib) = r^a*e(-th*b)*e^(b*ln(r)+th*a).
   * Instead, we rather use the cartesian form of the base and the index. */
  if (!letPowerAtRoot && base.type() == ExpressionNode::Type::ComplexCartesian) {
    complexBase = static_cast<ComplexCartesian &>(base);
    Integer ten(10);
    if (index.type() == ExpressionNode::Type::Rational) {
      Rational r = static_cast<Rational &>(index);
      if (r.isMinusOne()) {
        // (x+iy)^(-1)
        result = complexBase.inverse(context, complexFormat, angleUnit, target);
      } else if (r.isHalf()) {
        // (x+iy)^(1/2)
        result = complexBase.squareRoot(context, complexFormat, angleUnit, target);
      } else if (r.isMinusHalf()) {
        // (x+iy)^(-1/2)
        result = complexBase.squareRoot(context, complexFormat, angleUnit, target).inverse(context, complexFormat, angleUnit, target);
      } else if (r.integerDenominator().isOne() && r.unsignedIntegerNumerator().isLowerThan(ten)) {
        if (r.sign() == ExpressionNode::Sign::Positive) {
          // (x+iy)^n, n integer positive n < 10
          result = complexBase.powerInteger(r.unsignedIntegerNumerator().extractedInt(), context, complexFormat, angleUnit, target);
        } else {
          // (x+iy)^(-n), n integer positive n < 10
          assert(r.sign() == ExpressionNode::Sign::Negative);
          result = complexBase.powerInteger(r.unsignedIntegerNumerator().extractedInt(), context, complexFormat, angleUnit, target).inverse(context, complexFormat, angleUnit, target);
        }
      }
      if (!result.isUninitialized()) {
        replaceWithInPlace(result);
        return result.shallowReduce();
      }
    }
  }
  // All other cases where one child at least is a ComplexCartesian
  if ((!letPowerAtRoot && base.isReal(context) && index.type() == ExpressionNode::Type::ComplexCartesian) ||
      (!letPowerAtRoot && base.type() == ExpressionNode::Type::ComplexCartesian && index.isReal(context)) ||
      (!letPowerAtRoot && base.type() == ExpressionNode::Type::ComplexCartesian && index.type() == ExpressionNode::Type::ComplexCartesian)) {
    complexBase = base.type() == ExpressionNode::Type::ComplexCartesian ? static_cast<ComplexCartesian &>(base) : ComplexCartesian::Builder(base, Rational::Builder(0));
    complexIndex = index.type() == ExpressionNode::Type::ComplexCartesian ? static_cast<ComplexCartesian &>(index) : ComplexCartesian::Builder(index, Rational::Builder(0));
    result = complexBase.power(complexIndex, context, complexFormat, angleUnit, target);
    replaceWithInPlace(result);
    return result.shallowReduce();
  }

  /* Step 3: We look for square root and sum of square roots (two terms maximum
   * so far) at the denominator and move them to the numerator. */
  if (target == ExpressionNode::ReductionTarget::User) {
    Expression r = removeSquareRootsFromDenominator(context, complexFormat, angleUnit);
    if (!r.isUninitialized()) {
      return r;
    }
  }

  /* Step 4: we simplify i^(p/q) = e^(i*Pi*p/2q) */
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    const Rational b = childAtIndex(1).convert<Rational>();
    // i^(p/q)
    if (childAtIndex(0).type() == ExpressionNode::Type::Constant && childAtIndex(0).convert<Constant>().isIComplex()) {
      Number r = Number::Multiplication(b, Rational::Builder(1, 2));
      Expression result = CreateComplexExponent(r, context, complexFormat, angleUnit, target);
      replaceWithInPlace(result);
      return result.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }

  // Step 5: (±inf)^x = 0 or ±inf
  if (childAtIndex(0).type() == ExpressionNode::Type::Infinity) {
    Expression result;
    if (childAtIndex(1).sign(&context) == ExpressionNode::Sign::Negative) {
      // --> 0 if x < 0
      result = Rational::Builder(0);
    } else if (childAtIndex(1).sign(&context) == ExpressionNode::Sign::Positive) {
      // --> (±inf) if x > 0
      result = Infinity::Builder(false);
      if (childAtIndex(0).sign(&context) == ExpressionNode::Sign::Negative) {
        // (-inf)^x --> (-1)^x*inf
        Power p = Power::Builder(Rational::Builder(-1), childAtIndex(1));
        result = Multiplication::Builder(p, result);
        p.shallowReduce(context, complexFormat, angleUnit, target);
      }
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }

  // Step 6: p^q with p, q rationals --> a*b^c*exp(i*pi*d) with a, b, c, d rationals
  if (!letPowerAtRoot && childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational a = childAtIndex(0).convert<Rational>();
    // p^q with p, q rationals
    // TODO: maybe reduce Number^Rational?
    if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
      Rational exp = childAtIndex(1).convert<Rational>();
      if (RationalExponentShouldNotBeReduced(a, exp)) {
        return *this;
      }
      return simplifyRationalRationalPower(context, complexFormat, angleUnit, target);
    }
  }
  // Step 7: (a)^(1/2) --> i*(-a)^(1/2)
  // WARNING: this rule true only if:
  // - a real: (-1*i)^(1/2) != i*i^(1/2)
  // - a is negative: (-(-2))^(1/2) != -2^(1/2)
  // We apply this rule only when a is a negative numeral
  if (!letPowerAtRoot
      && childAtIndex(0).isNumber()
      && childAtIndex(1).type() == ExpressionNode::Type::Rational
      && childAtIndex(1).convert<Rational>().isHalf())
  {
    Expression m0 = childAtIndex(0).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
    if (!m0.isUninitialized()) {
      replaceChildAtIndexInPlace(0, m0);
      // m0 doest not need to be shallowReduce as makePositiveAnyNegativeNumeralFactor returns a reduced expression
      Multiplication m1 = Multiplication::Builder();
      replaceWithInPlace(m1);
      // Multiply m1 by i complex
      Constant i = Constant::Builder(UCodePointMathematicalBoldSmallI);
      m1.addChildAtIndexInPlace(i, 0, 0);
      i.shallowReduce(context, complexFormat, angleUnit, target);
      m1.addChildAtIndexInPlace(*this, 1, 1);
      shallowReduce(context, complexFormat, angleUnit, target);
      return m1.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }
  // Step 8: e^(r*i*Pi) with r rational --> cos(pi*r) + i*sin(pi*r)
  if (!letPowerAtRoot && isNthRootOfUnity()) {
    Expression m = childAtIndex(1);
    Expression i = m.childAtIndex(m.numberOfChildren()-2);
    static_cast<Multiplication &>(m).removeChildAtIndexInPlace(m.numberOfChildren()-2);
    if (angleUnit == Preferences::AngleUnit::Degree) {
      m.replaceChildAtIndexInPlace(m.numberOfChildren()-1, Rational::Builder(180));
    }
    Expression cos = Cosine::Builder(m);
    m = m.shallowReduce(context, complexFormat, angleUnit, target);
    Expression sin = Sine::Builder(m.clone());
    Expression complexPart = Multiplication::Builder(sin, i);
    sin.shallowReduce(context, complexFormat, angleUnit, target);
    Expression a = Addition::Builder(cos, complexPart);
    cos.shallowReduce(context, complexFormat, angleUnit, target);
    complexPart.shallowReduce(context, complexFormat, angleUnit, target);
    replaceWithInPlace(a);
    return a.shallowReduce(context, complexFormat, angleUnit, target);
  }
  // Step 9: x^log(y,x)->y if y > 0
  if (childAtIndex(1).type() == ExpressionNode::Type::Logarithm) {
    if (childAtIndex(1).numberOfChildren() == 2 && childAtIndex(0).isIdenticalTo(childAtIndex(1).childAtIndex(1))) {
      // y > 0
      if (childAtIndex(1).childAtIndex(0).sign(&context) == ExpressionNode::Sign::Positive) {
        Expression result = childAtIndex(1).childAtIndex(0);
        replaceWithInPlace(result);
        return result;
      }
    }
    // 10^log(y)
    if (childAtIndex(1).numberOfChildren() == 1
        && childAtIndex(0).type() == ExpressionNode::Type::Rational
        && childAtIndex(0).convert<Rational>().isTen())
    {
      Expression result = childAtIndex(1).childAtIndex(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  /* Step 10: (a^b)^c -> a^(b*c)
   * This rule is not generally true: ((-2)^2)^(1/2) != (-2)^(2*1/2) = -2
   * This rule is true if:
   * - a > 0
   * - in Real: when b and c are integers
   * - in other modes: when c is integer
   * (Warning: in real mode only c integer is not enough:
   * ex: ((-2)^(1/2))^2 = unreal != -2)
   */
  if (childAtIndex(0).type() == ExpressionNode::Type::Power) {
    Power p = childAtIndex(0).convert<Power>();
    // Check if a > 0 or c is Integer
    bool aPositive = p.childAtIndex(0).sign(&context) == ExpressionNode::Sign::Positive;
    bool cInteger = (childAtIndex(1).type() == ExpressionNode::Type::Rational
          && childAtIndex(1).convert<Rational>().integerDenominator().isOne());
    if (aPositive || cInteger) {
      // Check that the complex format is not Real or that b is an integer
      bool bInteger = (p.childAtIndex(1).type() == ExpressionNode::Type::Rational && p.childAtIndex(1).convert<Rational>().integerDenominator().isOne());
      if (aPositive || complexFormat != Preferences::ComplexFormat::Real || bInteger) {
        return simplifyPowerPower(context, complexFormat, angleUnit, target);
      }
    }
  }
  // Step 11: (a*b*c*...)^r ?
  if (!letPowerAtRoot && childAtIndex(0).type() == ExpressionNode::Type::Multiplication) {
    Multiplication m = childAtIndex(0).convert<Multiplication>();
    // Case 1: (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().integerDenominator().isOne()) {
      return simplifyPowerMultiplication(context, complexFormat, angleUnit, target);
    }
    // Case 2: (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a not -1
    for (int i = 0; i < m.numberOfChildren(); i++) {
      // a is signed and a != -1
      if (m.childAtIndex(i).sign(&context) != ExpressionNode::Sign::Unknown
          && (m.childAtIndex(i).type() != ExpressionNode::Type::Rational
            || !m.childAtIndex(i).convert<Rational>().isMinusOne()))
      {
        Expression r = childAtIndex(1);
        Expression rCopy = r.clone();
        Expression factor = m.childAtIndex(i);

        // (sign(a)*b*...)^r
        if (factor.sign(&context) == ExpressionNode::Sign::Negative) {
          m.replaceChildAtIndexInPlace(i, Rational::Builder(-1));
          factor = factor.setSign(ExpressionNode::Sign::Positive, &context, complexFormat, angleUnit, target);
        } else {
          m.removeChildAtIndexInPlace(i);
        }
        m.shallowReduce(context, complexFormat, angleUnit, target);

        // |a|^r
        Power p = Power::Builder(factor, rCopy);

        // |a|^r*(sign(a)*b*...)^r
        Power thisRef = *this;
        Multiplication root = Multiplication::Builder(p);
        replaceWithInPlace(root);
        root.addChildAtIndexInPlace(thisRef, 1, 1);
        p.shallowReduce(context, complexFormat, angleUnit, target);
        thisRef.shallowReduce(context, complexFormat, angleUnit, target);
        return root.shallowReduce(context, complexFormat, angleUnit, target);
      }
    }
  }
  // Step 12: a^(p/q+c+...) -> Rational::Builder(a^p)*a^(1/q+c+...) with a rational and a != 0 and p, q integers
  if (!letPowerAtRoot
      && childAtIndex(0).type() == ExpressionNode::Type::Rational
      && !childAtIndex(0).convert<Rational>().isZero()
      && childAtIndex(1).type() == ExpressionNode::Type::Addition)
  {
    Addition a = childAtIndex(1).convert<Addition>();
    // Check is b is rational
    if (a.childAtIndex(0).type() == ExpressionNode::Type::Rational) {
      const Rational rationalIndex = a.childAtIndex(0).convert<Rational>();
      if (rationalIndex.unsignedIntegerNumerator().isOne() && !rationalIndex.integerDenominator().isOne()) {
        /* Do not reduce a^(1/q+c+...) to avoid potential infinite loop:
         * a^(1/q+c+...) --> a^(1/q)*a^(c+...) --> a^(1/q+c+...)*/
        /* TODO: do something more sensible here:
         * - add rule (-rational)^x --> (-1)^x*rational^x so we only consider
         *   positive rational or (-1)
         * - change simplifyRationalRationalPower to be able to detect when no
         *   rational was extracted (ie 2^(1/2) --> 2^(1/2)) to avoid applying
         *   this rule in that case
         * Once this is done, we can reduce 4^(1/2+a) --> 2*4^a which is not
         * done so far to avoir the infinite loop:
         * 2^(1/2+s) --> 2^(1/2)*2^s --> 2^(1/2+s)... */
        return *this;
      }
      const Rational rationalBase = childAtIndex(0).convert<Rational>();
      if (RationalExponentShouldNotBeReduced(rationalBase, rationalIndex)) {
        return *this;
      }
      Power p1 = Power::Builder(childAtIndex(0).clone(), a.childAtIndex(0));
      Power thisRef = *this;
      childAtIndex(1).convert<Addition>().removeChildAtIndexInPlace(0); // p2 = a^(c+...)
      // if addition had only 2 children
      childAtIndex(1).convert<Addition>().squashUnaryHierarchyInPlace();
      Multiplication m = Multiplication::Builder(p1);
      replaceWithInPlace(m);
      m.addChildAtIndexInPlace(thisRef, 1, 1);
      p1.simplifyRationalRationalPower(context, complexFormat, angleUnit, target);
      return m.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }

  // Step 13: (a0+a1+...am)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Multinome)
  if (!letPowerAtRoot
      && childAtIndex(1).type() == ExpressionNode::Type::Rational
      && !childAtIndex(1).convert<Rational>().signedIntegerNumerator().isZero()
      && childAtIndex(1).convert<Rational>().integerDenominator().isOne()
      && childAtIndex(0).type() == ExpressionNode::Type::Addition)
  {
    // Exponent n
    Rational nr = childAtIndex(1).convert<Rational>();
    Integer n = nr.unsignedIntegerNumerator();
    /* If n is above 25, the resulting sum would have more than
     * k_maxNumberOfTermsInExpandedMultinome terms so we do not expand it. */
    if (Integer(k_maxNumberOfTermsInExpandedMultinome).isLowerThan(n) || n.isOne()) {
      return *this;
    }
    int clippedN = n.extractedInt(); // Authorized because n < k_maxNumberOfTermsInExpandedMultinome
    assert(clippedN > 0);

    // Number of terms in addition m
    int m = childAtIndex(0).numberOfChildren();
    /* The multinome (a0+a2+...+a(m-1))^n has BinomialCoefficient(n+m-1,n) terms;
     * we expand the multinome only when the number of terms in the resulting
     * sum has less than k_maxNumberOfTermsInExpandedMultinome terms. */
    if (k_maxNumberOfTermsInExpandedMultinome < BinomialCoefficientNode::compute(static_cast<double>(clippedN), static_cast<double>(clippedN+m-1))) {
      return *this;
    }

    Expression result = childAtIndex(0);
    Expression a = result.clone();
    for (int i = 2; i <= clippedN; i++) {
      // result = result * (a0+a1+...+a(m-1) in its expanded form
      if (result.type() == ExpressionNode::Type::Addition) {
        // We need a 'double' distribution and newA will hold the new expanded form
        Expression newA = Addition::Builder();
        for (int j = 0; j < a.numberOfChildren(); j++) {
          Expression m = Multiplication::Builder(result.clone(), a.childAtIndex(j).clone()).distributeOnOperandAtIndex(0, context, complexFormat, angleUnit, target);
          if (newA.type() == ExpressionNode::Type::Addition) {
            static_cast<Addition &>(newA).addChildAtIndexInPlace(m, newA.numberOfChildren(), newA.numberOfChildren());
          } else {
            newA = Addition::Builder(newA, m);
          }
          newA = newA.shallowReduce(context, complexFormat, angleUnit, target);
        }
        result.replaceWithInPlace(newA);
        result = newA;
      } else {
        // Just distribute result on a
        Multiplication m = Multiplication::Builder(a.clone(), result.clone());
        Expression distributedM = m.distributeOnOperandAtIndex(0, context, complexFormat, angleUnit, target);
        result.replaceWithInPlace(distributedM);
        result = distributedM;
        result = result.shallowReduce(context, complexFormat, angleUnit, target);
      }
    }
    if (nr.sign() == ExpressionNode::Sign::Negative) {
      nr.replaceWithInPlace(Rational::Builder(-1));
      return shallowReduce(context, complexFormat, angleUnit, target);
    } else {
      replaceWithInPlace(result);
      return result;
    }
  }
#if 0
  /* We could use the Newton formula instead which is quicker but not immediate
   * to implement in the general case (Newton multinome). */
  // (a+b)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Newton)
  if (!letPowerAtRoot && childAtIndex(1)->type() == ExpressionNode::Type::Rational && static_cast<const Rational *>(childAtIndex(1))->denominator().isOne() && childAtIndex(0)->type() == ExpressionNode::Type::Addition && childAtIndex(0)->numberOfChildren() == 2) {
    Rational * nr = static_cast<Rational *>(childAtIndex(1));
    Integer n = nr->numerator();
    n.setNegative(false);
    if (Integer(k_maxExpandedBinome).isLowerThan(n) || n.isOne()) {
      return this;
    }
    int clippedN = n.extractedInt(); // Authorized because n < k_maxExpandedBinome < k_maxNValue
    Expression * x0 = childAtIndex(0)->childAtIndex(0);
    Expression * x1 = childAtIndex(0)->childAtIndex(1);
    Addition * a = new Addition::Builder();
    for (int i = 0; i <= clippedN; i++) {
      Rational * r = new Rational::Builder(static_cast<int>(BinomialCoefficient::compute(static_cast<double>(i), static_cast<double>(clippedN))));
      Power * p0 = new Power::Builder(x0->clone(), new Rational::Builder(i), false);
      Power * p1 = new Power::Builder(x1->clone(), new Rational::Builder(clippedN-i), false);
      const Expression * operands[3] = {r, p0, p1};
      Multiplication * m = new Multiplication::Builder(operands, 3, false);
      p0->shallowReduce(context, complexFormat, angleUnit, target);
      p1->shallowReduce(context, complexFormat, angleUnit, target);
      a->addOperand(m);
      m->shallowReduce(context, complexFormat, angleUnit, target);
    }
    if (nr->sign(&context) == Sign::Negative) {
      nr->replaceWith(new Rational::Builder(-1), true);
      childAtIndex(0)->replaceWith(a, true)->shallowReduce(context, complexFormat, angleUnit, target);
      return shallowReduce(context, complexFormat, angleUnit, target);
    } else {
      return replaceWith(a, true)->shallowReduce(context, complexFormat, angleUnit, target);
    }
  }
#endif
  return *this;
}

Expression Power::shallowBeautify(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // Step 1: X^-y -> 1/(X->shallowBeautify)^y
  Expression p = denominator(context, complexFormat, angleUnit);
  // If the denominator is initialized, the index of the power is of form -y
  if (!p.isUninitialized()) {
    Division d = Division::Builder(Rational::Builder(1), p);
    p.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    replaceWithInPlace(d);
    return d.shallowBeautify(context, complexFormat, angleUnit, target);
  }
  // Step 2: Turn a^(1/n) into root(a, n)
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().signedIntegerNumerator().isOne()) {
    Integer index = childAtIndex(1).convert<Rational>().integerDenominator();
    // Special case: a^(1/2) --> sqrt(a)
    if (index.isEqualTo(Integer(2))) {
      Expression result = SquareRoot::Builder(childAtIndex(0));
      replaceWithInPlace(result);
      return result;
    }
    Expression result = NthRoot::Builder(childAtIndex(0), Rational::Builder(index));
    replaceWithInPlace(result);
    return result;
  }

  /* Optional Step 3: if the ReductionTarget is the System, turn a^(p/q) into
   * (root(a, q))^p
   * Indeed, root(a, q) can have a real root which is not the principale angle
   * but that we want to return in real complex format. This special case is
   * handled in NthRoot approximation but not in Power approximation. */
  if (target == ExpressionNode::ReductionTarget::System && childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    Integer p = childAtIndex(1).convert<Rational>().signedIntegerNumerator();
    Integer q = childAtIndex(1).convert<Rational>().integerDenominator();
    Expression nthRoot = q.isOne() ? childAtIndex(0) : NthRoot::Builder(childAtIndex(0), Rational::Builder(q));
    Expression result = p.isOne() ? nthRoot : Power::Builder(nthRoot, Rational::Builder(p));
    replaceWithInPlace(result);
    return result;
  }

  // Step 4: +(a,b)^c ->(+(a,b))^c and *(a,b)^c ->(*(a,b))^c
  if (childAtIndex(0).type() == ExpressionNode::Type::Addition
      || childAtIndex(0).type() == ExpressionNode::Type::Multiplication)
  {
    Parenthesis p = Parenthesis::Builder(childAtIndex(0));
    replaceChildAtIndexInPlace(0, p);
  }
  return *this;
}

// Private

// Simplification
Expression Power::denominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  // Clone the power
  Expression clone = Power::Builder(childAtIndex(0).clone(), childAtIndex(1).clone());
  // If the power is of form x^(-y), denominator should be x^y
  Expression positiveIndex = clone.childAtIndex(1).makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
  if (!positiveIndex.isUninitialized()) {
    // if y was -1, clone is now x^1, denominator is then only x
    // we cannot shallowReduce the clone as it is not attached to its parent yet
    if (positiveIndex.isRationalOne()) {
      return clone.childAtIndex(0);
    }
    return clone;
  }
  return Expression();
}

Expression Power::simplifyPowerPower(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // this is p^e = (a^b)^e, we want a^(b*e)
  Expression p = childAtIndex(0);
  Multiplication m = Multiplication::Builder(p.childAtIndex(1), childAtIndex(1));
  replaceChildAtIndexInPlace(0, p.childAtIndex(0));
  replaceChildAtIndexInPlace(1, m);
  m.shallowReduce(context, complexFormat, angleUnit, target);
  return shallowReduce(context, complexFormat, angleUnit, target);
}

Expression Power::simplifyPowerMultiplication(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // this is m^r= (a*b*c*...)^r, we want a^r * b^r *c^r * ...
  Expression m = childAtIndex(0);
  Expression r = childAtIndex(1);
  for (int index = 0; index < m.numberOfChildren(); index++) {
    Power p = Power::Builder(m.childAtIndex(index).clone(), r.clone()); // We copy r and factor to avoid inheritance issues
    m.replaceChildAtIndexInPlace(index, p);
    p.shallowReduce(context, complexFormat, angleUnit, target);
  }
  replaceWithInPlace(m);
  return m.shallowReduce(context, complexFormat, angleUnit, target);
}

Expression Power::simplifyRationalRationalPower(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // this is a^b with a, b rationals
  Rational a = childAtIndex(0).convert<Rational>();
  Rational b = childAtIndex(1).convert<Rational>();
  if (b.integerDenominator().isOne()) {
    Rational r = Rational::IntegerPower(a, b.signedIntegerNumerator());
    if (r.numeratorOrDenominatorIsInfinity()) {
      return Power::Builder(a, b);
    }
    replaceWithInPlace(r);
    return r;
  }
  Expression n;
  Expression d;
  if (b.sign() == ExpressionNode::Sign::Negative) {
    b.setSign(ExpressionNode::Sign::Positive);
    n = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, false, context, complexFormat, angleUnit, target);
    d = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, true, context, complexFormat, angleUnit, target);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, false, context, complexFormat, angleUnit, target);
    d = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, true, context, complexFormat, angleUnit, target);
  }
  Multiplication m = Multiplication::Builder(n, d);
  replaceWithInPlace(m);
  return m.shallowReduce(context, complexFormat, angleUnit, target);
}

Expression Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  assert(!i.isZero());
  assert(r.sign() == ExpressionNode::Sign::Positive);
  if (i.isOne()) {
    return Rational::Builder(1);
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors < 0) {
    /* We could not break i in prime factors (it might take either too many
     * factors or too much time). */
    Expression rClone = r.clone().setSign(isDenominator ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive, &context, complexFormat, angleUnit, target);
    return Power::Builder(Rational::Builder(i), rClone);
  }

  Integer r1(1);
  Integer r2(1);
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    Integer n = Integer::Multiplication(coefficients[index], r.signedIntegerNumerator());
    IntegerDivision div = Integer::Division(n, r.integerDenominator());
    r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    r2 = Integer::Multiplication(r2, Integer::Power(factors[index], div.remainder));
  }
  if (r2.isOverflow() || r1.isOverflow()) {
    // we overflow Integer at one point: we abort
    return Power::Builder(Rational::Builder(i), r.clone());
  }
  Rational p1 = Rational::Builder(r2);
  Integer oneExponent = isDenominator ? Integer(-1) : Integer(1);
  Integer rDenominator = r.integerDenominator();
  Rational p2 = Rational::Builder(oneExponent, rDenominator);
  Power p = Power::Builder(p1, p2);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  Integer one(1);
  Rational r3 = isDenominator ? Rational::Builder(one, r1) : Rational::Builder(r1);
  Multiplication m = Multiplication::Builder();
  m.addChildAtIndexInPlace(r3, 0, 0);
  if (!r2.isOne()) {
    m.addChildAtIndexInPlace(p, 1, 1);
  }
  if (i.isNegative()) {
    if (complexFormat == Preferences::ComplexFormat::Real) {
      /* On real numbers (-1)^(p/q) =
       * - 1 if p is even
       * - -1 if p and q are odd
       * - has no real solution otherwise */
      if (!r.unsignedIntegerNumerator().isEven()) {
        if (r.integerDenominator().isEven()) {
          return Unreal::Builder();
        } else {
          m.addChildAtIndexInPlace(Rational::Builder(-1), 0, m.numberOfChildren());
        }
      }
    } else {
      /* On complex numbers, we pick the first root (-1)^(p/q) = e^(i*pi*p/q) */
      Expression exp = CreateComplexExponent(r, context, complexFormat, angleUnit, target);
      m.addChildAtIndexInPlace(exp, m.numberOfChildren(), m.numberOfChildren());
      exp.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }
  return m.shallowReduce(context, complexFormat, angleUnit, target);
}

Expression Power::removeSquareRootsFromDenominator(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  Expression result;
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational
      && childAtIndex(1).type() == ExpressionNode::Type::Rational
      && (childAtIndex(1).convert<Rational>().isHalf()
        || childAtIndex(1).convert<Rational>().isMinusHalf()))
  {
    /* We are considering a term of the form sqrt(p/q) (or 1/sqrt(p/q)), with p
     * and q integers. We'll turn those into sqrt(p*q)/q (or sqrt(p*q)/p). */
    Rational castedChild0 = childAtIndex(0).convert<Rational>();
    Rational castedChild1 = childAtIndex(1).convert<Rational>();
    Integer p = castedChild0.signedIntegerNumerator();
    assert(!p.isZero()); // We eliminated case of form 0^(-1/2) at first step of shallowReduce
    Integer q = castedChild0.integerDenominator();
    // We do nothing for terms of the form sqrt(p)
    if (!q.isOne() || castedChild1.isMinusHalf()) {
      Integer pq = Integer::Multiplication(p, q);
      if (pq.isOverflow()) {
        return result;
      }
      Power sqrt = Power::Builder(Rational::Builder(pq), Rational::Builder(1, 2));
      Integer one(1);
      if (castedChild1.isHalf()) {
        result = Multiplication::Builder(Rational::Builder(one, q), sqrt);
      } else {
        result = Multiplication::Builder(Rational::Builder(one, p), sqrt); // We use here the assertion that p != 0
      }
      sqrt.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    }
  } else if (childAtIndex(1).type() == ExpressionNode::Type::Rational
      && childAtIndex(1).convert<Rational>().isMinusOne()
      && childAtIndex(0).type() == ExpressionNode::Type::Addition
      && childAtIndex(0).numberOfChildren() == 2
      && TermIsARationalSquareRootOrRational(childAtIndex(0).childAtIndex(0))
      && TermIsARationalSquareRootOrRational(childAtIndex(0).childAtIndex(1)))
  {
    /* We're considering a term of the form
     *
     * 1/(n1/d1*sqrt(p1/q1) + n2/d2*sqrt(p2/q2))
     *
     * and we want to turn it into
     *
     *  n1*q2*d1*d2^2*sqrt(p1*q1) - n2*q1*d2*d1^2*sqrt(p2*q2)
     * -------------------------------------------------------
     *          n1^2*d2^2*p1*q2 - n2^2*d1^2*p2*q1
     */
    const Rational f1 = RationalFactorInExpression(childAtIndex(0).childAtIndex(0));
    const Rational f2 = RationalFactorInExpression(childAtIndex(0).childAtIndex(1));
    const Rational r1 = RadicandInExpression(childAtIndex(0).childAtIndex(0));
    const Rational r2 = RadicandInExpression(childAtIndex(0).childAtIndex(1));
    Integer n1 = f1.signedIntegerNumerator();
    Integer d1 = f1.integerDenominator();
    Integer p1 = r1.signedIntegerNumerator();
    Integer q1 = r1.integerDenominator();
    Integer n2 = f2.signedIntegerNumerator();
    Integer d2 = f2.integerDenominator();
    Integer p2 = r2.signedIntegerNumerator();
    Integer q2 = r2.integerDenominator();

    // Compute the denominator = n1^2*d2^2*p1*q2 - n2^2*d1^2*p2*q1
    Integer denominator = Integer::Subtraction(
        Integer::Multiplication(
          Integer::Multiplication(
            Integer::Power(n1, Integer(2)),
            Integer::Power(d2, Integer(2))),
          Integer::Multiplication(p1, q2)),
        Integer::Multiplication(
          Integer::Multiplication(
            Integer::Power(n2, Integer(2)),
            Integer::Power(d1, Integer(2))),
          Integer::Multiplication(p2, q1)));

    // Compute the numerator
    Integer pq1 = Integer::Multiplication(p1, q1);
    Integer pq2 = Integer::Multiplication(p2, q2);
    Power sqrt1 = Power::Builder(Rational::Builder(pq1), Rational::Builder(1, 2));
    Power sqrt2 = Power::Builder(Rational::Builder(pq2), Rational::Builder(1, 2));
    Integer factor1 = Integer::Multiplication(
        Integer::Multiplication(n1, d1),
        Integer::Multiplication(Integer::Power(d2, Integer(2)), q2));
    Multiplication m1 = Multiplication::Builder(Rational::Builder(factor1), sqrt1);
    Integer factor2 = Integer::Multiplication(
        Integer::Multiplication(n2, d2),
        Integer::Multiplication(Integer::Power(d1, Integer(2)), q1));
    Multiplication m2 = Multiplication::Builder(Rational::Builder(factor2), sqrt2);
    Expression numerator;
    if (denominator.isNegative()) {
      numerator = Subtraction::Builder(m2, m1);
      denominator.setNegative(false);
    } else {
      numerator = Subtraction::Builder(m1, m2);
    }
    if (denominator.isOverflow() || factor1.isOverflow() || factor2.isOverflow() || pq1.isOverflow() || pq2.isOverflow()) {
      return result; // Escape
    }
    numerator = numerator.deepReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
    Integer one(1);
    result = Multiplication::Builder(numerator, Rational::Builder(one, denominator));
  }

  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    result = result.shallowReduce(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::User);
  }
  return result;
}
bool Power::parentIsALogarithmOfSameBase() const {
  Expression p = parent();
  if (p.isUninitialized()) {
    return false;
  }
  if (p.type() == ExpressionNode::Type::Logarithm && p.childAtIndex(0) == *this) {
    // parent = log(10^x)
    if (p.numberOfChildren() == 1) {
      if (childAtIndex(0).type() == ExpressionNode::Type::Rational && childAtIndex(0).convert<Rational>().isTen()) {
        return true;
      }
      return false;
    }
    // parent = log(x^y,x)
    if (childAtIndex(0).isIdenticalTo(p.childAtIndex(1))) {
      return true;
    }
  }
  // parent = ln(e^x)
  if (p.type() == ExpressionNode::Type::NaperianLogarithm
      && p.childAtIndex(0) == *this
      && childAtIndex(0).type() == ExpressionNode::Type::Constant
      && childAtIndex(0).convert<Constant>().isExponential())
  {
    return true;
  }
  return false;
}

bool Power::isNthRootOfUnity() const {
  // We check we are equal to e^(i*pi) or e^(i*pi*rational)
  if (childAtIndex(0).type() != ExpressionNode::Type::Constant || !childAtIndex(0).convert<Constant>().isExponential()) {
    return false;
  }
  if (childAtIndex(1).type() != ExpressionNode::Type::Multiplication) {
    return false;
  }
  if (childAtIndex(1).numberOfChildren() < 2 || childAtIndex(1).numberOfChildren() > 3) {
    return false;
  }
  const Expression i = childAtIndex(1).childAtIndex(childAtIndex(1).numberOfChildren()-2);
  if (i.type() != ExpressionNode::Type::Constant || !static_cast<const Constant &>(i).isIComplex()) {
    return false;
  }
  const Expression pi = childAtIndex(1).childAtIndex(childAtIndex(1).numberOfChildren()-1);
  if (pi.type() != ExpressionNode::Type::Constant || !static_cast<const Constant &>(pi).isPi()) {
    return false;
  }
  if (numberOfChildren() == 2) {
    return true;
  }
  if (childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    return true;
  }
  return false;
}

Expression Power::equivalentExpressionUsingStandardExpression() const {
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    if (childAtIndex(1).convert<Rational>().isMinusOne()) {
      return Division::Builder(Rational::Builder(1), childAtIndex(0).clone());
    }
    if (childAtIndex(1).convert<Rational>().isHalf()) {
      return SquareRoot::Builder(childAtIndex(0).clone());
    }
    if (childAtIndex(1).convert<Rational>().isMinusHalf()) {
      return Division::Builder(Rational::Builder(1), SquareRoot::Builder(childAtIndex(0).clone()));
    }
  }
  return Expression();
}

Expression Power::CreateComplexExponent(const Expression & r, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  // Returns e^(i*pi*r)
  const Constant exp = Constant::Builder(UCodePointScriptSmallE);
  Constant iComplex = Constant::Builder(UCodePointMathematicalBoldSmallI);
  const Constant pi = Constant::Builder(UCodePointGreekSmallLetterPi);
  Multiplication mExp = Multiplication::Builder(iComplex, pi, r.clone());
  iComplex.shallowReduce(context, complexFormat, angleUnit, target);
  Power p = Power::Builder(exp, mExp);
  mExp.shallowReduce(context, complexFormat, angleUnit, target);
  return p;
#if 0
  const Constant iComplex = Constant::Builder(UCodePointMathematicalBoldSmallI);
  const Constant pi = Constant::Builder(UCodePointGreekSmallLetterPi);
  Expression op = Multiplication::Builder(pi, r).shallowReduce(context, complexFormat, angleUnit, false);
  Cosine cos = Cosine(op).shallowReduce(context, complexFormat, angleUnit, false);;
  Sine sin = Sine(op).shallowReduce(context, complexFormat, angleUnit, false);
  Expression m = Multiplication::Builder(iComplex, sin);
  Expression a = Addition::Builder(cos, m);
  const Expression * multExpOperands[3] = {pi, r->clone()};
#endif
}

bool Power::TermIsARationalSquareRootOrRational(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Rational) {
    return true;
  }
  if (e.type() == ExpressionNode::Type::Power
      && e.childAtIndex(0).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(1).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(1).convert<Rational>().isHalf())
  {
    return true;
  }
  if (e.type() == ExpressionNode::Type::Multiplication
      && e.numberOfChildren() == 2
      && e.childAtIndex(0).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(1).type() == ExpressionNode::Type::Power
      && e.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(1).childAtIndex(1).type() == ExpressionNode::Type::Rational
      && e.childAtIndex(1).childAtIndex(1).convert<Rational>().isHalf())
  {
    return true;
  }
  return false;
}

const Rational Power::RadicandInExpression(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Rational) {
    return Rational::Builder(1);
  } else if (e.type() == ExpressionNode::Type::Power) {
    assert(e.type() == ExpressionNode::Type::Power);
    assert(e.childAtIndex(0).type() == ExpressionNode::Type::Rational);
    return e.childAtIndex(0).convert<Rational>();
  } else {
    assert(e.type() == ExpressionNode::Type::Multiplication);
    assert(e.childAtIndex(1).type() == ExpressionNode::Type::Power);
    assert(e.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Rational);
    return e.childAtIndex(1).childAtIndex(0).convert<Rational>();
  }
}

const Rational Power::RationalFactorInExpression(const Expression & e) {
  if (e.type() == ExpressionNode::Type::Rational) {
    return static_cast<const Rational &>(e);
  } else if (e.type() == ExpressionNode::Type::Power) {
    return Rational::Builder(1);
  } else {
    assert(e.type() == ExpressionNode::Type::Multiplication);
    assert(e.childAtIndex(0).type() == ExpressionNode::Type::Rational);
    return e.childAtIndex(0).convert<Rational>();
  }
}

bool Power::RationalExponentShouldNotBeReduced(const Rational & b, const Rational & r) {
  if (r.isMinusOne()) {
    return false;
  }
  /* We check that the simplification does not involve too complex power of
   * integers (ie 3^999, 120232323232^50) that would take too much time to
   * compute:
   *  - we cap the exponent at k_maxExactPowerMatrix
   *  - we cap the resulting power at DBL_MAX
   * The complexity of computing a power of rational is mainly due to computing
   * the GCD of the resulting numerator and denominator. Euclide algorithm's
   * complexity is apportionned to the number of decimal digits in the smallest
   * integer. */
  Integer maxIntegerExponent = r.unsignedIntegerNumerator();
  if (Integer::NaturalOrder(maxIntegerExponent, Integer(k_maxExactPowerMatrix)) > 0) {
    return true;
  }

  double index = maxIntegerExponent.approximate<double>();
  double powerNumerator = std::pow(b.unsignedIntegerNumerator().approximate<double>(), index);
  double powerDenominator = std::pow(b.integerDenominator().approximate<double>(), index);
  if (std::isnan(powerNumerator) || std::isnan(powerDenominator) || std::isinf(powerNumerator) || std::isinf(powerDenominator)) {
    return true;
  }
  return false;
}


template Complex<float> PowerNode::compute<float>(std::complex<float>, std::complex<float>, Preferences::ComplexFormat);
template Complex<double> PowerNode::compute<double>(std::complex<double>, std::complex<double>, Preferences::ComplexFormat);

}
