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
#include <poincare/parenthesis.h>
#include <poincare/sine.h>
#include <poincare/square_root.h>
#include <poincare/symbol.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <poincare/serialization_helper.h>
#include <cmath>
#include <math.h>
#include <ion.h>
#include <assert.h>

namespace Poincare {

// Properties
ExpressionNode::Sign PowerNode::sign() const {
  if (Expression::shouldStopProcessing()) {
    return Sign::Unknown;
  }
  if (childAtIndex(0)->sign() == Sign::Positive && childAtIndex(1)->sign() != Sign::Unknown) {
    return Sign::Positive;
  }
  if (childAtIndex(0)->sign() == Sign::Negative && childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (r->denominator().isOne()) {
      assert(!Integer::Division(r->signedNumerator(), Integer(2)).remainder.isInfinity());
      if (Integer::Division(r->signedNumerator(), Integer(2)).remainder.isZero()) {
        return Sign::Positive;
      } else {
        return Sign::Negative;
      }
    }
  }
  return Sign::Unknown;
}

Expression PowerNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Power(this).setSign(s, context, angleUnit);
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
    if (!r->denominator().isOne() || r->sign() == Sign::Negative) {
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

// Private

template<typename T>
Complex<T> PowerNode::compute(const std::complex<T> c, const std::complex<T> d) {
  std::complex<T> result;
  if (c.imag() == 0.0 && d.imag() == 0.0 && (c.real() > 0.0 || std::round(d.real()) == d.real())) {
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
  return Complex<T>(ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument(result));
}

// Layout

Layout PowerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  ExpressionNode * indiceOperand = childAtIndex(1);
  // Delete eventual parentheses of the indice in the pretty print
  if (indiceOperand->type() == ExpressionNode::Type::Parenthesis) {
    indiceOperand = indiceOperand->childAtIndex(0);
  }
  HorizontalLayout result = HorizontalLayout();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(VerticalOffsetLayout(
        indiceOperand->createLayout(floatDisplayMode, numberOfSignificantDigits),
        VerticalOffsetLayoutNode::Type::Superscript),
      result.numberOfChildren(),
      result.numberOfChildren(),
      nullptr);
  return result;
}

// Serialize

bool PowerNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && static_cast<const ExpressionNode *>(child)->sign() == Sign::Negative) {
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

Expression PowerNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Power(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression PowerNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Power(this).shallowBeautify(context, angleUnit);
}

int PowerNode::simplificationOrderGreaterType(const ExpressionNode * e, bool canBeInterrupted) const {
  int baseComparison = SimplificationOrder(childAtIndex(0), e, canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one(1);
  return SimplificationOrder(childAtIndex(1), one.node(), canBeInterrupted);
}

int PowerNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->numberOfChildren() > 0);
  int baseComparison = SimplificationOrder(childAtIndex(0), e->childAtIndex(0), canBeInterrupted);
  if (baseComparison != 0) {
    return baseComparison;
  }
  assert(e->numberOfChildren() > 1);
  return SimplificationOrder(childAtIndex(1), e->childAtIndex(1), canBeInterrupted);
}

Expression PowerNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Power(this).denominator(context, angleUnit);
}

// Evaluation
template<typename T> MatrixComplex<T> PowerNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n) {
  return MatrixComplex<T>::Undefined();
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> d) {
  if (m.numberOfRows() != m.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  T power = Complex<T>(d).toScalar();
  if (std::isnan(power) || std::isinf(power) || power != (int)power || std::fabs(power) > k_maxApproximatePowerMatrix) {
    return MatrixComplex<T>::Undefined();
  }
  if (power < 0) {
    MatrixComplex<T> inverse = m.inverse();
    if (inverse.isUninitialized()) {
      return MatrixComplex<T>::Undefined();
    }
    Complex<T> minusC = Complex<T>(-d);
    MatrixComplex<T> result = PowerNode::computeOnMatrixAndComplex(inverse, minusC.stdComplex());
    return result;
  }
  MatrixComplex<T> result = MatrixComplex<T>::createIdentity(m.numberOfRows());
  // TODO: implement a quick exponentiation
  for (int k = 0; k < (int)power; k++) {
    if (Expression::shouldStopProcessing()) {
      return MatrixComplex<T>::Undefined();
    }
    result = MultiplicationNode::computeOnMatrices<T>(result, m);
  }
  return result;
}

template<typename T> MatrixComplex<T> PowerNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  return MatrixComplex<T>::Undefined();
}

// Power
Power::Power(Expression base, Expression exponent) : Expression(TreePool::sharedPool()->createTreeNode<PowerNode>()) {
  replaceChildAtIndexInPlace(0, base);
  replaceChildAtIndexInPlace(1, exponent);
}

Expression Power::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  assert(s == ExpressionNode::Sign::Positive);
  assert(childAtIndex(0).sign() == ExpressionNode::Sign::Negative);
  Expression result = Power(childAtIndex(0).setSign(ExpressionNode::Sign::Positive, context, angleUnit), childAtIndex(1));
  replaceWithInPlace(result);
  return result;
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
        coefficients[i] = Rational(0);
      }
      coefficients[n] = Rational(1);
      return n;
    }
  }
  return -1;
}

Expression Power::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {

  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }

#if MATRIX_EXACT_REDUCING
#if 0
  /* Step 0: get rid of matrix */
  if (childAtIndex(1)->type() == ExpressionNode::Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
  if (childAtIndex(0)->type() == ExpressionNode::Type::Matrix) {
    Matrix * mat = static_cast<Matrix *>(childAtIndex(0));
    if (childAtIndex(1)->type() != ExpressionNode::Type::Rational || !static_cast<const Rational *>(childAtIndex(1))->denominator().isOne()) {
      return replaceWith(new Undefined(), true);
    }
    Integer exponent = static_cast<const Rational *>(childAtIndex(1))->numerator();
    if (mat->numberOfRows() != mat->numberOfColumns()) {
      return replaceWith(new Undefined(), true);
    }
    if (exponent.isNegative()) {
      childAtIndex(1)->setSign(Sign::Positive, context, angleUnit);
      Expression * newMatrix = shallowReduce(context, angleUnit);
      Expression * parent = newMatrix->parent();
      MatrixInverse * inv = new MatrixInverse(newMatrix, false);
      parent->replaceOperand(newMatrix, inv, false);
      return inv;
    }
    if (Integer::NaturalOrder(exponent, Integer(k_maxExactPowerMatrix)) > 0) {
      return this;
    }
    int exp = exponent.extractedInt(); // Ok, because 0 < exponent < k_maxExactPowerMatrix
    Matrix * id = Matrix::createIdentity(mat->numberOfRows());
    if (exp == 0) {
      return replaceWith(id, true);
    }
    Multiplication * result = new Multiplication(id, mat->clone());
    // TODO: implement a quick exponentiation
    for (int k = 1; k < exp; k++) {
      result->addOperand(mat->clone());
    }
    replaceWith(result, true);
    return result->shallowReduce(context, angleUnit);
  }
#endif
#endif

  /* Step 0: if both children are true complexes, the result is undefined. We
   * can assert that evaluations are Complex, as matrix are not simplified */

  Evaluation<float> c0Approximated = childAtIndex(0).node()->approximate(1.0f, context, angleUnit);
  Evaluation<float> c1Approximated = childAtIndex(1).node()->approximate(1.0f, context, angleUnit);
  Complex<float> c0 = static_cast<Complex<float>&>(c0Approximated);
  Complex<float> c1 = static_cast<Complex<float>&>(c1Approximated);
  bool bothChildrenComplexes = c0.imag() != 0 && c1.imag() != 0 && !std::isnan(c0.imag()) && !std::isnan(c1.imag());
  bool nonComplexNegativeChild0 = c0.imag() == 0 && c0.real() < 0;
  if (bothChildrenComplexes) {
    return *this;
  }

  /* Step 1: We handle simple cases as x^0, x^1, 0^x and 1^x first for 2 reasons:
   * - we can assert this step that there is no division by 0:
   *   for instance, 0^(-2)->undefined
   * - we save computational time by early escaping for these cases. */
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    const Rational b = childAtIndex(1).convert<Rational>();
    // x^0
    if (b.isZero()) {
      // 0^0 = undef
      if (childAtIndex(0).type() == ExpressionNode::Type::Rational && childAtIndex(0).convert<Rational>().isZero()) {
        Expression result = Undefined();
        replaceWithInPlace(result);
        return result;
      }
      /* Warning: in all other cases but 0^0, we replace x^0 by one. This is
       * almost always true except when x = 0. However, not substituting x^0 by
       * one would prevent from simplifying many expressions like x/x->1. */
      Expression result = Rational(1);
      replaceWithInPlace(result);
      return result;
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
      if (childAtIndex(1).sign() == ExpressionNode::Sign::Positive) {
        Expression result = Rational(0);
        replaceWithInPlace(result);
        return result;
      }
      if (childAtIndex(1).sign() == ExpressionNode::Sign::Negative) {
        Expression result = Undefined(); // TODO: could be infinity?
        replaceWithInPlace(result);
        return result;
      }
    }
    // 1^x
    if (a.isOne()) {
      Expression result = Rational(1);
      replaceWithInPlace(result);
      return result;
    }
  }

  /* Step 2: We look for square root and sum of square roots (two terms maximum
   * so far) at the denominator and move them to the numerator. */
  {
    Expression r = removeSquareRootsFromDenominator(context, angleUnit);
    if (!r.isUninitialized()) {
      return r;
    }
  }

  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    const Rational b = childAtIndex(1).convert<Rational>();
    // i^(p/q)
    if (childAtIndex(0).type() == ExpressionNode::Type::Constant && childAtIndex(0).convert<Constant>().isIComplex()) {
      Number r = Number::Multiplication(b, Rational(1, 2));
      Expression result = CreateComplexExponent(r);
      replaceWithInPlace(result);
      return result.shallowReduce(context, angleUnit);
    }
  }

  // (±inf)^x
  if (childAtIndex(0).type() == ExpressionNode::Type::Infinity) {
    Expression result;
    if (childAtIndex(1).sign() == ExpressionNode::Sign::Negative) {
      // --> 0 if x < 0
      result = Rational(0);
    } else if (childAtIndex(1).sign() == ExpressionNode::Sign::Positive) {
      // --> (±inf) if x > 0
      result = Infinity(false);
      if (childAtIndex(0).sign() == ExpressionNode::Sign::Negative) {
        // (-inf)^x --> (-1)^x*inf
        Power p(Rational(-1), childAtIndex(1));
        result = Multiplication(p, result);
        p.shallowReduce(context, angleUnit);
      }
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result.shallowReduce(context, angleUnit);
    }
  }

  bool letPowerAtRoot = parentIsALogarithmOfSameBase();
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational a = childAtIndex(0).convert<Rational>();
    // p^q with p, q rationals
    // TODO: maybe reduce Number^Rational?
    if (!letPowerAtRoot && childAtIndex(1).type() == ExpressionNode::Type::Rational) {
      Rational exp = childAtIndex(1).convert<Rational>();
      if (RationalExponentShouldNotBeReduced(a, exp)) {
        return *this;
      }
      return simplifyRationalRationalPower(context, angleUnit);
    }
  }
  // (a)^(1/2) with a < 0 --> i*(-a)^(1/2)
  if (!letPowerAtRoot
      && nonComplexNegativeChild0
      && childAtIndex(1).type() == ExpressionNode::Type::Rational
      && childAtIndex(1).convert<Rational>().isHalf())
  {
    Expression m0 = Multiplication(Rational(-1), childAtIndex(0));
    replaceChildAtIndexInPlace(0, m0);
    m0.shallowReduce(context, angleUnit);
    Multiplication m1 = Multiplication();
    replaceWithInPlace(m1);
    m1.addChildAtIndexInPlace(Constant(Ion::Charset::IComplex), 0, 0);
    m1.addChildAtIndexInPlace(*this, 1, 1);
    shallowReduce(context, angleUnit);
    return m1.shallowReduce(context, angleUnit);
  }
  // e^(i*Pi*r) with r rational
  if (!letPowerAtRoot && isNthRootOfUnity()) {
    Expression m = childAtIndex(1);
    Expression i = m.childAtIndex(m.numberOfChildren()-1);
    static_cast<Multiplication &>(m).removeChildAtIndexInPlace(m.numberOfChildren()-1);
    if (angleUnit == Preferences::AngleUnit::Degree) {
      m.replaceChildAtIndexInPlace(m.numberOfChildren()-1, Rational(180));
    }
    Expression cos = Cosine::Builder(m);
    m = m.shallowReduce(context, angleUnit);
    Expression sin = Sine::Builder(m.clone());
    Expression complexPart = Multiplication(sin, i);
    sin.shallowReduce(context, angleUnit);
    Expression a = Addition(cos, complexPart);
    cos.shallowReduce(context, angleUnit);
    complexPart.shallowReduce(context, angleUnit);
    replaceWithInPlace(a);
    return a.shallowReduce(context, angleUnit);
  }
  // x^log(y,x)->y if y > 0
  if (childAtIndex(1).type() == ExpressionNode::Type::Logarithm) {
    if (childAtIndex(1).numberOfChildren() == 2 && childAtIndex(0).isIdenticalTo(childAtIndex(1).childAtIndex(1))) {
      // y > 0
      if (childAtIndex(1).childAtIndex(0).sign() == ExpressionNode::Sign::Positive) {
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
  // (a^b)^c -> a^(b*c) if a > 0 or c is integer
  if (childAtIndex(0).type() == ExpressionNode::Type::Power) {
    Power p = childAtIndex(0).convert<Power>();
    // Check if a > 0 or c is Integer
    if (p.childAtIndex(0).sign() == ExpressionNode::Sign::Positive
        || (childAtIndex(1).type() == ExpressionNode::Type::Rational
          && childAtIndex(1).convert<Rational>().integerDenominator().isOne()))
    {
      return simplifyPowerPower(context, angleUnit);
    }
  }
  // (a*b*c*...)^r ?
  if (!letPowerAtRoot && childAtIndex(0).type() == ExpressionNode::Type::Multiplication) {
    Multiplication m = childAtIndex(0).convert<Multiplication>();
    // (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().integerDenominator().isOne()) {
      return simplifyPowerMultiplication(context, angleUnit);
    }
    // (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a not -1
    for (int i = 0; i < m.numberOfChildren(); i++) {
      // a is signed and a != -1
      if (m.childAtIndex(i).sign() != ExpressionNode::Sign::Unknown
          && (m.childAtIndex(i).type() != ExpressionNode::Type::Rational
            || !m.childAtIndex(i).convert<Rational>().isMinusOne()))
      {
        Expression r = childAtIndex(1);
        Expression rCopy = r.clone();
        Expression factor = m.childAtIndex(i);

        // (sign(a)*b*...)^r
        if (factor.sign() == ExpressionNode::Sign::Negative) {
          m.replaceChildAtIndexInPlace(i, Rational(-1));
          factor = factor.setSign(ExpressionNode::Sign::Positive, context, angleUnit);
        } else {
          m.removeChildAtIndexInPlace(i);
        }
        m.shallowReduce(context, angleUnit);

        // |a|^r
        Power p = Power(factor, rCopy);

        // |a|^r*(sign(a)*b*...)^r
        Power thisRef = *this;
        Multiplication root = Multiplication(p);
        replaceWithInPlace(root);
        root.addChildAtIndexInPlace(thisRef, 1, 1);
        p.shallowReduce(context, angleUnit);
        thisRef.shallowReduce(context, angleUnit);
        return root.shallowReduce(context, angleUnit);
      }
    }
  }
  // a^(b+c+...) -> Rational(a^b)*a^c with a and b rational and a != 0
  if (!letPowerAtRoot
      && childAtIndex(0).type() == ExpressionNode::Type::Rational
      && !childAtIndex(0).convert<Rational>().isZero()
      && childAtIndex(1).type() == ExpressionNode::Type::Addition)
  {
    Addition a = childAtIndex(1).convert<Addition>();
    // Check is b is rational
    if (a.childAtIndex(0).type() == ExpressionNode::Type::Rational) {
      const Rational rationalBase = childAtIndex(0).convert<Rational>();
      const Rational rationalIndex = a.childAtIndex(0).convert<Rational>();
      if (RationalExponentShouldNotBeReduced(rationalBase, rationalIndex)) {
        return *this;
      }
      Power p1 = Power(childAtIndex(0).clone(), a.childAtIndex(0).clone());
      Power thisRef = *this;
      childAtIndex(1).convert<Addition>().removeChildAtIndexInPlace(0); // p2 = a^(c+...)
      // if addition had only 2 children
      childAtIndex(1).convert<Addition>().squashUnaryHierarchyInPlace();
      Multiplication m = Multiplication(p1);
      replaceWithInPlace(m);
      m.addChildAtIndexInPlace(thisRef, 1, 1);
      p1.simplifyRationalRationalPower(context, angleUnit);
      return m.shallowReduce(context, angleUnit);
    }
  }

  // (a0+a1+...am)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Multinome)
  if (!letPowerAtRoot
      && childAtIndex(1).type() == ExpressionNode::Type::Rational
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
        Expression newA = Addition();
        for (int j = 0; j < a.numberOfChildren(); j++) {
          Expression m = Multiplication(result.clone(), a.childAtIndex(j).clone()).distributeOnOperandAtIndex(0, context, angleUnit);
          if (newA.type() == ExpressionNode::Type::Addition) {
            static_cast<Addition &>(newA).addChildAtIndexInPlace(m, newA.numberOfChildren(), newA.numberOfChildren());
          } else {
            newA = Addition(newA, m);
          }
          newA = newA.shallowReduce(context, angleUnit);
        }
        result.replaceWithInPlace(newA);
        result = newA;
      } else {
        // Just distribute result on a
        Multiplication m = Multiplication(a.clone(), result.clone());
        Expression distributedM = m.distributeOnOperandAtIndex(0, context, angleUnit);
        result.replaceWithInPlace(distributedM);
        result = distributedM;
        result = result.shallowReduce(context, angleUnit);
      }
    }
    if (nr.sign() == ExpressionNode::Sign::Negative) {
      nr.replaceWithInPlace(Rational(-1));
      return shallowReduce(context, angleUnit);
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
    Addition * a = new Addition();
    for (int i = 0; i <= clippedN; i++) {
      Rational * r = new Rational(static_cast<int>(BinomialCoefficient::compute(static_cast<double>(i), static_cast<double>(clippedN))));
      Power * p0 = new Power(x0->clone(), new Rational(i), false);
      Power * p1 = new Power(x1->clone(), new Rational(clippedN-i), false);
      const Expression * operands[3] = {r, p0, p1};
      Multiplication * m = new Multiplication(operands, 3, false);
      p0->shallowReduce(context, angleUnit);
      p1->shallowReduce(context, angleUnit);
      a->addOperand(m);
      m->shallowReduce(context, angleUnit);
    }
    if (nr->sign() == Sign::Negative) {
      nr->replaceWith(new Rational(-1), true);
      childAtIndex(0)->replaceWith(a, true)->shallowReduce(context, angleUnit);
      return shallowReduce(context, angleUnit);
    } else {
      return replaceWith(a, true)->shallowReduce(context, angleUnit);
    }
  }
#endif
  return *this;
}

Expression Power::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  // X^-y -> 1/(X->shallowBeautify)^y
  if (childAtIndex(1).sign() == ExpressionNode::Sign::Negative) {
    Expression p = denominator(context, angleUnit);
    Division d = Division(Rational(1), p);
    p.shallowReduce(context, angleUnit);
    replaceWithInPlace(d);
    return d.shallowBeautify(context, angleUnit);
  }
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().signedIntegerNumerator().isOne()) {
    Integer index = childAtIndex(1).convert<Rational>().integerDenominator();
    if (index.isEqualTo(Integer(2))) {
      Expression result = SquareRoot::Builder(childAtIndex(0));
      replaceWithInPlace(result);
      return result;
    }
    Expression result = NthRoot::Builder(childAtIndex(0), Rational(index));
    replaceWithInPlace(result);
    return result;
  }

  // +(a,b)^c ->(+(a,b))^c and *(a,b)^c ->(*(a,b))^c
  if (childAtIndex(0).type() == ExpressionNode::Type::Addition
      || childAtIndex(0).type() == ExpressionNode::Type::Multiplication)
  {
    Parenthesis p = Parenthesis(childAtIndex(0));
    replaceChildAtIndexInPlace(0, p);
  }
  return *this;
}

// Private

// Simplification
Expression Power::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  if (childAtIndex(1).sign() == ExpressionNode::Sign::Negative) {
    Expression positivePowerClone = Power(childAtIndex(0).clone(), childAtIndex(1).clone().setSign(ExpressionNode::Sign::Positive, context, angleUnit));
    if (positivePowerClone.childAtIndex(1).type() == ExpressionNode::Type::Rational && positivePowerClone.childAtIndex(1).convert<Rational>().isOne()) {
      return positivePowerClone.childAtIndex(0);
    }
    return positivePowerClone;
  }
  return Expression();
}

Expression Power::simplifyPowerPower(Context& context, Preferences::AngleUnit angleUnit) {
  // this is p^e = (a^b)^e, we want a^(b*e)
  Expression p = childAtIndex(0);
  Multiplication m(p.childAtIndex(1), childAtIndex(1));
  replaceChildAtIndexInPlace(0, p.childAtIndex(0));
  replaceChildAtIndexInPlace(1, m);
  m.shallowReduce(context, angleUnit);
  return shallowReduce(context, angleUnit);
}

Expression Power::simplifyPowerMultiplication(Context& context, Preferences::AngleUnit angleUnit) {
  // this is m^r= (a*b*c*...)^r, we want a^r * b^r *c^r * ...
  Expression m = childAtIndex(0);
  Expression r = childAtIndex(1);
  for (int index = 0; index < m.numberOfChildren(); index++) {
    Power p = Power(m.childAtIndex(index).clone(), r.clone()); // We copy r and factor to avoid inheritance issues
    m.replaceChildAtIndexInPlace(index, p);
    p.shallowReduce(context, angleUnit);
  }
  replaceWithInPlace(m);
  return m.shallowReduce(context, angleUnit);
}

Expression Power::simplifyRationalRationalPower(Context& context, Preferences::AngleUnit angleUnit) {
  // this is a^b with a, b rationals
  Rational a = childAtIndex(0).convert<Rational>();
  Rational b = childAtIndex(1).convert<Rational>();
  if (b.integerDenominator().isOne()) {
    Rational r = Rational::IntegerPower(a, b.signedIntegerNumerator());
    if (r.numeratorOrDenominatorIsInfinity()) {
      return Power(a, b);
    }
    replaceWithInPlace(r);
    return r;
  }
  Expression n;
  Expression d;
  if (b.sign() == ExpressionNode::Sign::Negative) {
    b.setSign(ExpressionNode::Sign::Positive);
    n = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, false, context, angleUnit);
    d = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, true, context, angleUnit);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, false, context, angleUnit);
    d = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, true, context, angleUnit);
  }
  Multiplication m = Multiplication(n, d);
  replaceWithInPlace(m);
  return m.shallowReduce(context, angleUnit);
}

Expression Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, Context & context, Preferences::AngleUnit angleUnit) {
  assert(!i.isZero());
  assert(r.sign() == ExpressionNode::Sign::Positive);
  if (i.isOne()) {
    return Rational(1);
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors < 0) {
    /* We could not break i in prime factors (it might take either too many
     * factors or too much time). */
    Expression rClone = r.clone().setSign(isDenominator ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive, context, angleUnit);
    return Power(Rational(i), rClone);
  }

  Integer r1(1);
  Integer r2(1);
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    Integer n = Integer::Multiplication(coefficients[index], r.signedIntegerNumerator());
    IntegerDivision div = Integer::Division(n, r.integerDenominator());
    r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    r2 = Integer::Multiplication(r2, Integer::Power(factors[index], div.remainder));
  }
  if (r2.isInfinity() || r1.isInfinity()) {
    // we overflow Integer at one point: we abort
    return Power(Rational(i), r.clone());
  }
  Rational p1 = Rational(r2);
  Integer oneExponent = isDenominator ? Integer(-1) : Integer(1);
  Integer rDenominator = r.integerDenominator();
  Rational p2 = Rational(oneExponent, rDenominator);
  Power p = Power(p1, p2);
  if (r1.isEqualTo(Integer(1)) && !i.isNegative()) {
    return p;
  }
  Integer one(1);
  Rational r3 = isDenominator ? Rational(one, r1) : Rational(r1);
  Multiplication m;
  m.addChildAtIndexInPlace(r3, 0, 0);
  if (!r2.isOne()) {
    m.addChildAtIndexInPlace(p, 1, 1);
  }
  if (i.isNegative()) {
    Expression exp = CreateComplexExponent(r);
    m.addChildAtIndexInPlace(exp, m.numberOfChildren(), m.numberOfChildren());
    exp.shallowReduce(context, angleUnit);
  }
  m.sortChildrenInPlace(PowerNode::SimplificationOrder, false);
  return m;
}

Expression Power::removeSquareRootsFromDenominator(Context & context, Preferences::AngleUnit angleUnit) {
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
      if (pq.isInfinity()) {
        return result;
      }
      Power sqrt = Power(Rational(pq), Rational(1, 2));
      Integer one(1);
      if (castedChild1.isHalf()) {
        result = Multiplication(Rational(one, q), sqrt);
      } else {
        result = Multiplication(Rational(one, p), sqrt); // We use here the assertion that p != 0
      }
      sqrt.shallowReduce(context, angleUnit);
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
    Power sqrt1 = Power(Rational(pq1), Rational(1, 2));
    Power sqrt2 = Power(Rational(pq2), Rational(1, 2));
    Integer factor1 = Integer::Multiplication(
        Integer::Multiplication(n1, d1),
        Integer::Multiplication(Integer::Power(d2, Integer(2)), q2));
    Multiplication m1 = Multiplication(Rational(factor1), sqrt1);
    Integer factor2 = Integer::Multiplication(
        Integer::Multiplication(n2, d2),
        Integer::Multiplication(Integer::Power(d1, Integer(2)), q1));
    Multiplication m2 = Multiplication(Rational(factor2), sqrt2);
    Expression numerator;
    if (denominator.isNegative()) {
      numerator = Subtraction(m2, m1);
      denominator.setNegative(false);
    } else {
      numerator = Subtraction(m1, m2);
    }
    if (denominator.isInfinity() || factor1.isInfinity() || factor2.isInfinity() || pq1.isInfinity() || pq2.isInfinity()) {
      return result; // Escape
    }
    numerator = numerator.deepReduce(context, angleUnit);
    Integer one(1);
    result = Multiplication(numerator, Rational(one, denominator));
  }

  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    result = result.shallowReduce(context, angleUnit);
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
  const Expression i = childAtIndex(1).childAtIndex(childAtIndex(1).numberOfChildren()-1);
  if (i.type() != ExpressionNode::Type::Constant || !static_cast<const Constant &>(i).isIComplex()) {
    return false;
  }
  const Expression pi = childAtIndex(1).childAtIndex(childAtIndex(1).numberOfChildren()-2);
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

Expression Power::CreateComplexExponent(const Expression & r) {
  // Returns e^(i*pi*r)
  const Constant exp = Constant(Ion::Charset::Exponential);
  const Constant iComplex = Constant(Ion::Charset::IComplex);
  const Constant pi = Constant(Ion::Charset::SmallPi);
  Multiplication mExp = Multiplication(iComplex, pi, r.clone());
  mExp.sortChildrenInPlace(PowerNode::SimplificationOrder, false);
  return Power(exp, mExp);
#if 0
  const Constant iComplex = Constant(Ion::Charset::IComplex);
  const Constant pi = Constant(Ion::Charset::SmallPi);
  Expression op = Multiplication(pi, r).shallowReduce(context, angleUnit);
  Cosine cos = Cosine(op).shallowReduce(context, angleUnit);;
  Sine sin = Sine(op).shallowReduce(context, angleUnit);
  Expression m = Multiplication(iComplex, sin);
  Expression a = Addition(cos, m);
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
    return Rational(1);
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
    return Rational(1);
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


template Complex<float> PowerNode::compute<float>(std::complex<float>, std::complex<float>);
template Complex<double> PowerNode::compute<double>(std::complex<double>, std::complex<double>);

}
