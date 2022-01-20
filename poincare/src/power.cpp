#include <poincare/power.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/based_integer.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/constant.h>
#include <poincare/cosine.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/infinity.h>
#include <poincare/matrix.h>
#include <poincare/matrix_identity.h>
#include <poincare/matrix_inverse.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/nth_root.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/serialization_helper.h>
#include <poincare/sine.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <poincare/vertical_offset_layout.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <math.h>
#include <utility>

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
    if (r->isInteger()) {
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

Expression PowerNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive);
  return Power(this).setSign(s, reductionContext);
}

int PowerNode::polynomialDegree(Context * context, const char * symbolName) const {
  int deg = ExpressionNode::polynomialDegree(context, symbolName);
  if (deg == 0) {
    return deg;
  }
  int op0Deg = childAtIndex(0)->polynomialDegree(context, symbolName);
  if (op0Deg < 0) {
    return -1;
  }
  Integer i;
  bool foundInteger = false;
  if (childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (!r->isInteger() || Number(r).sign() == Sign::Negative) {
      return -1;
    }
    foundInteger = true;
    i = r->signedNumerator();
  }
  else if(childAtIndex(1)->type() == ExpressionNode::Type::BasedInteger) {
    BasedIntegerNode * b = static_cast<BasedIntegerNode *>(childAtIndex(1));
    if (Number(b).sign() == Sign::Negative) {
      return -1;
    }
    foundInteger = true;
    i = b->integer();
  }

  if (foundInteger) {
    if (!i.isExtractable()) {
      return -1;
    }
    op0Deg *= i.extractedInt();
    return op0Deg;
  }

  return -1;
}

Expression PowerNode::removeUnit(Expression * unit) {
  return Power(this).removeUnit(unit);
}

int PowerNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  return Power(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

bool PowerNode::isReal(Context * context) const {
  Expression base(childAtIndex(0));
  Expression index(childAtIndex(1));
  // Both base and index are real and:
  // - either base > 0
  // - or index is an integer
  if (base.isReal(context) &&
      index.isReal(context) &&
      (base.sign(context) == Sign::Positive ||
       (index.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(index).isInteger()))) {
    return true;
  }
  return false;
}

bool PowerNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  if (childIndex == 0) {

    if ((child.isNumber() && static_cast<const Number &>(child).sign() == Sign::Negative)
       || (child.type() == Type::Rational && !static_cast<const Rational &>(child).isInteger()))
    {
      /* ^(-2.3, 4) --> (-2.3)^{4}
       * ^(2/3, 4) --> (2/3)^{4}   */
      return true;
    }
    if (child.type() == Type::Conjugate) {
      return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
    }
    // ^(2+3,4) --> (2+3)^{4}
    Type types[] = {Type::Power, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition};
    return child.isOfType(types, 6);
  }
  return false;
}

// Private

template<typename T>
Complex<T> PowerNode::computeNotPrincipalRealRootOfRationalPow(const std::complex<T> c, T p, T q) {
  // Assert p and q are in fact integers
  assert(std::round(p) == p);
  assert(std::round(q) == q);
  /* Try to find a real root of c^(p/q) with p, q integers. We ignore cases
   * where the principal root is real as these cases are handled generically
   * later (for instance 1232^(1/8) which has a real principal root is not
   * handled here). */
  if (c.imag() == (T)0.0 && std::pow((T)-1.0, q) < (T)0.0) {
    /* If c real and q odd integer (q odd if (-1)^q = -1), a real root does
     * exist (which is not necessarily the principal root)!
     * For q even integer, a real root does not necessarily exist (example:
     * -2 ^(1/2)). */
    std::complex<T> absc = c;
    absc.real(std::fabs(absc.real()));
    // compute |c|^(p/q) which is a real
    Complex<T> absCPowD = PowerNode::compute(absc, std::complex<T>(p/q), Preferences::ComplexFormat::Real);
    /* As q is odd, c^(p/q) = (sign(c)^(1/q))^p * |c|^(p/q)
     *                      = sign(c)^p         * |c|^(p/q)
     *                      = -|c|^(p/q) iff c < 0 and p odd */
    return c.real() < (T)0.0 && std::pow((T)-1.0, p) < (T)0.0 ? Complex<T>::Builder(-absCPowD.stdComplex()) : absCPowD;
  }
  return Complex<T>::Undefined();
}

template<typename T>
Complex<T> PowerNode::compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) {
  std::complex<T> result;
  if (c.imag() == (T)0.0 && d.imag() == (T)0.0 && c.real() != (T)0.0 && (c.real() > (T)0.0 || std::round(d.real()) == d.real())) {
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
   * part and if arg ~ Pi/2 [Pi], we discard the residual real part.
   * Let's determine when the arg [Pi] (or arg [Pi/2]) is negligible:
   * With c = r*e^(iθ) and d = x+iy, c^d = r^x*e^(yθ)*e^i(yln(r)+xθ)
   * so arg(c^d) = y*ln(r)+xθ.
   * We consider that arg[π] is negligible if it is negligible compared to
   * norm(d) = sqrt(x^2+y^2) and ln(r) = ln(norm(c)).*/
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c, d, false));
}

// Layout

Layout PowerNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  ExpressionNode * indiceOperand = childAtIndex(1);
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(VerticalOffsetLayout::Builder(
        indiceOperand->createLayout(floatDisplayMode, numberOfSignificantDigits),
        VerticalOffsetLayoutNode::Position::Superscript),
      result.numberOfChildren(),
      result.numberOfChildren(),
      nullptr);
  return std::move(result);
}

// Serialize

bool PowerNode::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  Type types[] = {Type::Power, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 6);
}

int PowerNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "^");
}

// Simplify

Expression PowerNode::shallowReduce(ReductionContext reductionContext) {
  return Power(this).shallowReduce(reductionContext);
}

Expression PowerNode::shallowBeautify(ReductionContext * reductionContext) {
  return Power(this).shallowBeautify(reductionContext);
}

int PowerNode::simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  int baseComparison = SimplificationOrder(childAtIndex(0), e, ascending, canBeInterrupted, ignoreParentheses);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one = Rational::Builder(1);
  return SimplificationOrder(childAtIndex(1), one.node(), ascending, canBeInterrupted, ignoreParentheses);
}

int PowerNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  assert(e->numberOfChildren() > 0);
  int baseComparison = SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending, canBeInterrupted, ignoreParentheses);
  if (baseComparison != 0) {
    return baseComparison;
  }
  assert(e->numberOfChildren() > 1);
  return SimplificationOrder(childAtIndex(1), e->childAtIndex(1), ascending, canBeInterrupted, ignoreParentheses);
}

Expression PowerNode::denominator(ReductionContext reductionContext) const {
  return Power(this).denominator(reductionContext);
}

bool PowerNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return Power(this).derivate(reductionContext, symbol, symbolValue);
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

template<typename T> Evaluation<T> PowerNode::templatedApproximate(ApproximationContext approximationContext) const {
  /* Special case: c^(p/q) with p, q integers
   * In real mode, c^(p/q) might have a real root which is not the principal
   * root. We return this value in that case to avoid returning "unreal". */
  if (approximationContext.complexFormat() == Preferences::ComplexFormat::Real) {
    Evaluation<T> base = childAtIndex(0)->approximate(T(), approximationContext);
    if (base.type() != EvaluationNode<T>::Type::Complex) {
      goto defaultApproximation;
    }
    std::complex<T> c = static_cast<Complex<T> &>(base).stdComplex();
    T p = NAN;
    T q = NAN;
    // If the power has been reduced, we look for a rational index
    if (childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
      const RationalNode * r = static_cast<const RationalNode *>(childAtIndex(1));
      p = r->signedNumerator().approximate<T>();
      q = r->denominator().approximate<T>();
    }
    /* If the power has been simplified (reduced + beautified), we look for an
     * index of the for Division(Rational,Rational). */
    if (childAtIndex(1)->type() == ExpressionNode::Type::Division && childAtIndex(1)->childAtIndex(0)->type() == ExpressionNode::Type::Rational && childAtIndex(1)->childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
      const RationalNode * pRat = static_cast<const RationalNode *>(childAtIndex(1)->childAtIndex(0));
      const RationalNode * qRat = static_cast<const RationalNode *>(childAtIndex(1)->childAtIndex(1));
      if (!pRat->denominator().isOne() || !qRat->denominator().isOne()) {
        goto defaultApproximation;
      }
      p = pRat->signedNumerator().approximate<T>();
      q = qRat->signedNumerator().approximate<T>();
    }
    /* We don't handle power that haven't been reduced or simplified as the
     * index can take to many forms and still be equivalent to p/q,
     * with p, q integers. */
    if (std::isnan(p) || std::isnan(q)) {
      goto defaultApproximation;
    }
    Complex<T> result = computeNotPrincipalRealRootOfRationalPow(c, p, q);
    if (!result.isUndefined()) {
      return std::move(result);
    }
  }
defaultApproximation:
  return ApproximationHelper::MapReduce<T>(this, approximationContext, compute<T>, computeOnComplexAndMatrix<T>, computeOnMatrixAndComplex<T>, computeOnMatrices<T>);
}

// Power

Expression Power::setSign(ExpressionNode::Sign s, ExpressionNode::ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive);
  if (childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
    Expression result = Power::Builder(childAtIndex(0).setSign(ExpressionNode::Sign::Positive, reductionContext), childAtIndex(1));
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }
  return *this;
}

int Power::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg <= 0) {
    return Expression::defaultGetPolynomialCoefficients(context, symbolName, coefficients);
  }
  /* Here we only consider the case x^4 as privateGetPolynomialCoefficients is
   * supposed to be called after reducing the expression. */
  int n;
  bool foundInteger = false;
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    Rational r = childAtIndex(1).convert<Rational>();
    if (!r.isInteger() || r.sign() == ExpressionNode::Sign::Negative) {
      return -1;
    }
    Integer num = r.unsignedIntegerNumerator();
    if (!num.isExtractable()) {
      return -1;
    }
    foundInteger = true;
    n = num.extractedInt();
  }
  else if(childAtIndex(1).type() == ExpressionNode::Type::BasedInteger) {
    BasedInteger b = childAtIndex(1).convert<BasedInteger>();
    if (Number(b).sign() == ExpressionNode::Sign::Negative) {
      return -1;
    }
    foundInteger = true;
    Integer i = b.integer();
    if (!i.isExtractable()) {
      return -1;
    }
    n = i.extractedInt();
  }
  
  if (childAtIndex(0).type() == ExpressionNode::Type::Symbol
      && strcmp(childAtIndex(0).convert<Symbol>().name(), symbolName) == 0
      && foundInteger)
  {
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

Expression Power::removeUnit(Expression * unit) {
  Expression childUnit;
  Expression child = childAtIndex(0).node()->removeUnit(&childUnit);
  if (!childUnit.isUninitialized()) {
    // Reduced power containing unit are of form "unit^i" with i integer
    assert(child.isRationalOne());
    assert(childUnit.type() == ExpressionNode::Type::Unit);
    Power p = *this;
    replaceWithInPlace(child);
    p.replaceChildAtIndexInPlace(0, childUnit);
    *unit = p;
    return child;
  }
  return *this;
}

Expression Power::shallowReduce(ExpressionNode::ReductionContext reductionContext) {

  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

  Expression base = childAtIndex(0);
  Expression index = childAtIndex(1);

  // Step 1: Handle the units
  {
    Expression indexUnit;
    index = index.removeUnit(&indexUnit);
    if (!indexUnit.isUninitialized() || index.isUndefined()) {
      // There must be no unit nor undefined in the exponent
      return replaceWithUndefinedInPlace();
    }
    assert(index == childAtIndex(1));
    if (base.hasUnit()) {
      if (index.type() != ExpressionNode::Type::Rational) {
        // The exponent must be an Rational
        return replaceWithUndefinedInPlace();
      }
    }
  }

  // Step 2: Handle matrices
  if (index.deepIsMatrix(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }
  ExpressionNode::Type baseType = base.type();
  ExpressionNode::Type indexType = index.type();
  if (base.deepIsMatrix(reductionContext.context())) {
    if (indexType != ExpressionNode::Type::Rational || !static_cast<Rational &>(index).isInteger()) {
      return replaceWithUndefinedInPlace();
    }
    if (baseType == ExpressionNode::Type::Matrix) {
      Matrix matrixBase = static_cast<Matrix &>(base);
      if (matrixBase.numberOfRows() != matrixBase.numberOfColumns()) {
        return replaceWithUndefinedInPlace();
      }
      Integer exponent = static_cast<Rational &>(index).signedIntegerNumerator();
      if (exponent.isNegative()) {
        index.setSign(ExpressionNode::Sign::Positive, reductionContext);
        Expression reducedPositiveExponentMatrix = shallowReduce(reductionContext);
        if (reducedPositiveExponentMatrix.type() == ExpressionNode::Type::Power) {
          /* The shallowReduce did not work, stop here so we do not get in an
           * infinite loop. */
          static_cast<Power &>(reducedPositiveExponentMatrix).childAtIndex(1).setSign(ExpressionNode::Sign::Negative, reductionContext);
          return reducedPositiveExponentMatrix;
        }
        Expression dummyExpression = Undefined::Builder();
        MatrixInverse inv = MatrixInverse::Builder(dummyExpression);
        reducedPositiveExponentMatrix.replaceWithInPlace(inv);
        inv.replaceChildInPlace(dummyExpression, reducedPositiveExponentMatrix);
        return inv.shallowReduce(reductionContext);
      }
      if (Integer::NaturalOrder(exponent, Integer(k_maxExactPowerMatrix)) > 0) {
        return *this;
      }
      int exp = exponent.extractedInt(); // Ok, because 0 < exponent < k_maxExactPowerMatrix
      if (exp == 0) {
        Matrix id = Matrix::CreateIdentity(matrixBase.numberOfRows());
        replaceWithInPlace(id);
        return std::move(id);
      }
      if (exp == 1) {
        replaceWithInPlace(matrixBase);
        return std::move(matrixBase);
      }
      Expression result = matrixBase.clone();
      // TODO: implement a quick exponentiation
      for (int k = 1; k < exp; k++) {
        result = Multiplication::Builder(result, matrixBase.clone());
        result = result.shallowReduce(reductionContext);
      }
      assert(!result.isUninitialized());
      replaceWithInPlace(result);
      return result;
    }
  }

  Expression power = *this;
  /* Step 3: if both children are true unresolved complexes, the result is not
   * simplified. TODO? */
  if (!base.isReal(reductionContext.context())
      && baseType != ExpressionNode::Type::ComplexCartesian
      && !index.isReal(reductionContext.context())
      && indexType != ExpressionNode::Type::ComplexCartesian)
  {
    return *this;
  }

  /* Step 4: Handle simple cases as x^0, x^1, 0^x and 1^x first, for 2 reasons:
   * - we can assert after this step that there is no division by 0:
   *   for instance, 0^(-2)->undefined
   * - we save computational time by early escaping for these cases. */
  if (indexType == ExpressionNode::Type::Rational) {
    const Rational rationalIndex = static_cast<Rational &>(index);
    // x^0
    if (rationalIndex.isZero()) {
      // 0^0 = undef or (±inf)^0 = undef
      if (base.nullStatus(reductionContext.context()) == ExpressionNode::NullStatus::Null || baseType == ExpressionNode::Type::Infinity) {
        return replaceWithUndefinedInPlace();
      }
      // x^0
      if (reductionContext.target() == ExpressionNode::ReductionTarget::User
          || base.isNumber()
          || baseType == ExpressionNode::Type::Constant)
      {
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
    if (rationalIndex.isOne()) {
      Expression result = childAtIndex(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  if (baseType == ExpressionNode::Type::Rational) {
    Rational rationalBase = static_cast<Rational &>(base);
    // 0^x
    if (rationalBase.isZero()) {
      // 0^x with x > 0 = 0
      ExpressionNode::Sign indexSign = index.sign(reductionContext.context());
      if (indexSign == ExpressionNode::Sign::Positive) {
        Expression result = Rational::Builder(0);
        replaceWithInPlace(result);
        return result;
      } else if (indexSign == ExpressionNode::Sign::Negative) {
        // 0^x with x < 0 = undef
        return replaceWithUndefinedInPlace();
      }
    }
    // 1^x = 1 if x != ±inf
    if (rationalBase.isOne() && !index.recursivelyMatches(Expression::IsInfinity, reductionContext.context())) {
      Expression result = Rational::Builder(1);
      replaceWithInPlace(result);
      return result;
    }
  }

  /* We do not apply some rules to a^b if the parent node is a logarithm of same
   * base a. In this case there is a simplication of form
   * ln(e^(3^(1/2))->3^(1/2). */
  bool letPowerAtRoot = parentIsALogarithmOfSameBase();

  /* Step 5: we now bubble up ComplexCartesian, we handle different cases:
   * At least, one child is a ComplexCartesian and the other is either a
   * ComplexCartesian or real. */

  ComplexCartesian complexBase;
  ComplexCartesian complexIndex;
  ComplexCartesian result;
  /* First, (x+iy)^q with q special values
   * For q = -1, 1/2, -1/2, n with n integer < 10, we avoid introducing arctangent
   * by using the formula (r*e^(i*th))^(a+ib) = r^a*e(-th*b)*e^(b*ln(r)+th*a).
   * Instead, we rather use the cartesian form of the base and the index. */
  if (!letPowerAtRoot && baseType == ExpressionNode::Type::ComplexCartesian) {
    complexBase = static_cast<ComplexCartesian &>(base);
    if (indexType == ExpressionNode::Type::Rational) {
      Rational r = static_cast<Rational &>(index);
      if (r.isMinusOne()) {
        // (x+iy)^(-1)
        result = complexBase.inverse(reductionContext);
      } else if (r.isHalf()) {
        // (x+iy)^(1/2)
        result = complexBase.squareRoot(reductionContext);
      } else if (r.isMinusHalf()) {
        // (x+iy)^(-1/2)
        result = complexBase.squareRoot(reductionContext).inverse(reductionContext);
      } else if (r.isInteger() && r.unsignedIntegerNumerator().isLowerThan(Integer(10))) {
        if (r.sign() == ExpressionNode::Sign::Positive) {
          // (x+iy)^n, n integer positive n < 10
          result = complexBase.powerInteger(r.unsignedIntegerNumerator().extractedInt(), reductionContext);
        } else {
          // (x+iy)^(-n), n integer positive n < 10
          assert(r.sign() == ExpressionNode::Sign::Negative);
          result = complexBase.powerInteger(r.unsignedIntegerNumerator().extractedInt(), reductionContext).inverse(reductionContext);
        }
      }
      if (!result.isUninitialized()) {
        replaceWithInPlace(result);
        return result.shallowReduce(reductionContext);
      }
    }
  }
  // All other cases where one child at least is a ComplexCartesian
  if (!letPowerAtRoot
      && ((base.isReal(reductionContext.context())
          && indexType == ExpressionNode::Type::ComplexCartesian)
        || (baseType == ExpressionNode::Type::ComplexCartesian
          && index.isReal(reductionContext.context()))
        || (baseType == ExpressionNode::Type::ComplexCartesian
          && indexType == ExpressionNode::Type::ComplexCartesian)))
  {
    complexBase = baseType == ExpressionNode::Type::ComplexCartesian ? static_cast<ComplexCartesian &>(base) : ComplexCartesian::Builder(base, Rational::Builder(0));
    complexIndex = indexType == ExpressionNode::Type::ComplexCartesian ? static_cast<ComplexCartesian &>(index) : ComplexCartesian::Builder(index, Rational::Builder(0));
    result = complexBase.power(complexIndex, reductionContext);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  /* Step 6: We look for square root and sum of square roots (two terms maximum
   * so far) at the denominator and move them to the numerator. */
  if (reductionContext.target() == ExpressionNode::ReductionTarget::User) {
    Expression r = removeSquareRootsFromDenominator(reductionContext);
    if (!r.isUninitialized()) {
      return r;
    }
  }

#if 0
  /* This was implemented when complex were not handled with the
   * ComplexCartesian node. It was used for instance to turn
   * sqrt(i) -> i^(1/2)
   *         -> e(i*π/4)        HERE!
   *         -> cos(π/4)+i*sin(π/4)
   *         -> sqrt(2)/2+i*sqrt(2)/2
   *
   * However, now, ComplexCartesian class handles power of ComplexCartesian
   * when the target of simplification is the User. When the target of
   * simplification is the system, i^(p/q) is a shorter expression than
   * e^(i*Pi*p/2q) so we rather keep it in the first form.
   * We thereby don't need the rule anymore!
   *  */
  /* Step 4: we simplify i^(p/q) = e^(i*Pi*p/2q) */
  /*if (indexType == ExpressionNode::Type::Rational) {
    const Rational rationalIndex = static_cast<Rational &>(index);
    // i^(p/q)
    if (baseType == ExpressionNode::Type::Constant && static_cast<Constant &>(base).isIComplex()) {
      Number r = Number::Multiplication(rationalIndex, Rational::Builder(1, 2));
      Expression result = CreateComplexExponent(r, reductionContext);
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }*/
#endif

  // Step 7: (±inf)^x = 0 or ±inf
  if (baseType == ExpressionNode::Type::Infinity) {
    Expression result;
    ExpressionNode::Sign indexSign = index.sign(reductionContext.context());
    if (indexSign == ExpressionNode::Sign::Negative) {
      // --> 0 if x < 0
      result = Rational::Builder(0);
    } else if (indexSign == ExpressionNode::Sign::Positive) {
      // --> (±inf) if x > 0
      result = Infinity::Builder(false);
      if (base.sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
        // (-inf)^x --> (-1)^x*inf
        Power p = Power::Builder(Rational::Builder(-1), childAtIndex(1));
        result = Multiplication::Builder(p, result);
        p.shallowReduce(reductionContext);
      }
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }

  // Step 8: p^q with p, q rationals --> a*b^c*exp(i*pi*d) with a, b, c, d rationals
  if (!letPowerAtRoot && baseType == ExpressionNode::Type::Rational) {
    Rational rationalBase = static_cast<Rational &>(base);
    // p^q with p, q rationals
    // TODO: maybe reduce Number^Rational?
    if (indexType == ExpressionNode::Type::Rational) {
      Rational rationalIndex = static_cast<Rational &>(index);
      if (RationalExponentShouldNotBeReduced(rationalBase, rationalIndex)) {
        return *this;
      }
      return simplifyRationalRationalPower(reductionContext);
    }
  }
  /* Step 9: (a)^(1/2) --> i*(-a)^(1/2)
   * WARNING: this rule true only if:
   * - a real: (-1*i)^(1/2) != i*i^(1/2)
   * - a is negative: (-(-2))^(1/2) != -2^(1/2)
   * We apply this rule only when a is a negative numeral */
  if (!letPowerAtRoot
      && base.isNumber()
      && indexType == ExpressionNode::Type::Rational
      && index.convert<Rational>().isHalf())
  {
    Expression m0 = base.makePositiveAnyNegativeNumeralFactor(reductionContext);
    if (!m0.isUninitialized()) {
      replaceChildAtIndexInPlace(0, m0);
      /* m0 doesn't need to be shallowReduce as
       * makePositiveAnyNegativeNumeralFactor returns a reduced expression */
      Multiplication m1 = Multiplication::Builder();
      replaceWithInPlace(m1);
      // Multiply m1 by i complex
      Constant i = Constant::Builder(UCodePointMathematicalBoldSmallI);
      m1.addChildAtIndexInPlace(i, 0, 0);
      i.shallowReduce(reductionContext);
      m1.addChildAtIndexInPlace(*this, 1, 1);
      shallowReduce(reductionContext);
      return m1.shallowReduce(reductionContext);
    }
  }
  // Step 10: e^(r*i*Pi) with r rational --> cos(pi*r) + i*sin(pi*r)
  if (!letPowerAtRoot && isNthRootOfUnity()) {
    Expression i = index.childAtIndex(index.numberOfChildren()-2);
    static_cast<Multiplication &>(index).removeChildAtIndexInPlace(index.numberOfChildren()-2);
    if (reductionContext.angleUnit() == Preferences::AngleUnit::Degree) {
      index.replaceChildAtIndexInPlace(index.numberOfChildren()-1, Rational::Builder(180));
    }
    Expression cos = Cosine::Builder(index);
    index = index.shallowReduce(reductionContext);
    Expression sin = Sine::Builder(index.clone());
    Expression complexPart = Multiplication::Builder(sin, i);
    sin.shallowReduce(reductionContext);
    Expression a = Addition::Builder(cos, complexPart);
    cos.shallowReduce(reductionContext);
    complexPart.shallowReduce(reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  // Step 11: x^log(y,x)->y if y > 0
  if (indexType == ExpressionNode::Type::Logarithm) {
    if (index.numberOfChildren() == 2 && base.isIdenticalTo(index.childAtIndex(1))) {
      // y > 0
      if (index.childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Positive) {
        Expression result = index.childAtIndex(0);
        replaceWithInPlace(result);
        return result;
      }
    }
    // 10^log(y)
    if (index.numberOfChildren() == 1
        && baseType == ExpressionNode::Type::Rational
        && static_cast<Rational &>(base).isTen())
    {
      Expression result = index.childAtIndex(0);
      replaceWithInPlace(result);
      return result;
    }
  }
  /* Step 12: (a^b)^c -> a^(b*c)
   * This rule is not generally true: ((-2)^2)^(1/2) != (-2)^(2*1/2) = -2
   * This rule is true if:
   * - a > 0
   * - c is an integer
   *
   * Warning 1: in real mode only c integer is not enough:
   * ex: ((-2)^(1/2))^2 = unreal != -2
   * We escape that case by returning 'unreal' if the a^b is complex.
   *
   * Warning 2: If we did not apply this rule on expressions of the form
   * (a^b)^(-1), we would end up in infinite loop when factorizing an addition
   * on the same denominator.
   * For ex:
   * 1+[tan(2)^1/2]^(-1) --> (tan(2)^1/2+tan(2)^1/2*[tan(2)^1/2]^(-1))/tan(2)^1/2
   *                     --> tan(2)+tan(2)*[tan(2)^1/2]^(-1)/tan(2)
   *                     --> tan(2)^(3/2)+tan(2)^(3/2)*[tan(2)^1/2]^(-1)/tan(2)^3/2
   *                     --> ...
   * Indeed, we have to apply the rule (a^b)^c -> a^(b*c) as soon as c is -1.
   */
  if (baseType == ExpressionNode::Type::Power) {
    Power powerBase = static_cast<Power &>(base);

    bool cInteger = indexType == ExpressionNode::Type::Rational && static_cast<Rational &>(index).isInteger();
    bool applyRule = powerBase.childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Positive // a > 0
                     || cInteger; // c integer
    bool cMinusOne = cInteger && static_cast<Rational &>(index).isMinusOne();
    /* If the complexFormat is real, we check that the inner power is defined
     * before applying the rule (a^b)^c -> a^(b*c). Otherwise, we return
     * 'unreal' or we do nothing.
     * We escape this additional check if c = -1 for two reasons:
     * - (a^b)^(-1) has to be reduced to avoid infinite loop discussed above;
     * - if a^b is unreal, a^(-b) also. */
    if (!cMinusOne && reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      Expression approximation = powerBase.approximate<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true);
      if (approximation.type() == ExpressionNode::Type::Unreal) {
        // The inner power is unreal, return "unreal"
        replaceWithInPlace(approximation);
        return approximation;
      } else if (approximation.type() == ExpressionNode::Type::Undefined && !powerBase.isReal(reductionContext.context())) { //TODO Quite dirty way to check this. We want to forbid x, but not a unit.
        /* The inner power is undefined, it can be 'x^(1/2)' for instance. We
         * don't want to simplify this as it could be unreal with x = -2 but
         * also real with x = 2. */
        applyRule = false;
      }
    }
    if (applyRule) {
      return simplifyPowerPower(reductionContext);
    }
  }

  // Step 13: (a*b*c*...)^r ?
  if (!letPowerAtRoot && baseType == ExpressionNode::Type::Multiplication) {
    Multiplication multiplicationBase = static_cast<Multiplication &>(base);
    // Case 1: (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
    if (indexType == ExpressionNode::Type::Rational && static_cast<Rational &>(index).isInteger()) {
      return simplifyPowerMultiplication(reductionContext);
    }
    // Case 2: (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a not -1
    for (int i = 0; i < multiplicationBase.numberOfChildren(); i++) {
      // a is signed and a != -1
      Expression baseChildI = multiplicationBase.childAtIndex(i);
      ExpressionNode::Sign baseChildISign = baseChildI.sign(reductionContext.context());
      if (baseChildISign != ExpressionNode::Sign::Unknown
          && (baseChildI.type() != ExpressionNode::Type::Rational
            || !static_cast<Rational &>(baseChildI).isMinusOne()))
      {
        Expression r = index;
        Expression rCopy = r.clone();
        // (sign(a)*b*...)^r
        if (baseChildISign == ExpressionNode::Sign::Negative) {
          multiplicationBase.replaceChildAtIndexInPlace(i, Rational::Builder(-1));
          baseChildI = baseChildI.setSign(ExpressionNode::Sign::Positive, reductionContext);
        } else {
          multiplicationBase.removeChildAtIndexInPlace(i);
        }
        multiplicationBase.shallowReduce(reductionContext);

        // |a|^r
        Power p = Power::Builder(baseChildI, rCopy);

        // |a|^r*(sign(a)*b*...)^r
        Power thisRef = *this;
        Multiplication root = Multiplication::Builder(p);
        replaceWithInPlace(root);
        root.addChildAtIndexInPlace(thisRef, 1, 1);
        p.shallowReduce(reductionContext);
        thisRef.shallowReduce(reductionContext);
        return root.shallowReduce(reductionContext);
      }
    }
  }
  /* Step 14: a^(p/q+c+...) -> Rational::Builder(a^p)*a^(1/q+c+...) with a
   * rational and a != 0 and p, q integers */
  if (!letPowerAtRoot
      && baseType == ExpressionNode::Type::Rational
      && !static_cast<Rational &>(base).isZero()
      && indexType == ExpressionNode::Type::Addition)
  {
    Addition additionIndex = static_cast<Addition &>(index);
    // Check is b is rational
    Expression additionIndexChild0 = additionIndex.childAtIndex(0);
    if (additionIndexChild0.type() == ExpressionNode::Type::Rational) {
      const Rational rationalIndex = static_cast<Rational &>(additionIndexChild0);
      if (rationalIndex.unsignedIntegerNumerator().isOne() && !rationalIndex.isInteger()) {
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
      const Rational rationalBase = static_cast<Rational &>(base);
      if (RationalExponentShouldNotBeReduced(rationalBase, rationalIndex)) {
        return *this;
      }
      Power p1 = Power::Builder(childAtIndex(0).clone(), additionIndex.childAtIndex(0));
      Power thisRef = *this;
      additionIndex.removeChildAtIndexInPlace(0); // p2 = a^(c+...)
      // if addition had only 2 children
      additionIndex.squashUnaryHierarchyInPlace();
      Multiplication m = Multiplication::Builder(p1);
      replaceWithInPlace(m);
      m.addChildAtIndexInPlace(thisRef, 1, 1);
      p1.simplifyRationalRationalPower(reductionContext);
      return m.shallowReduce(reductionContext);
    }
  }

  /* Step 15: (a0+a1+...am)^n with n integer
   *              -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Multinome)
   * We don't apply this rule when the target is the SystemForApproximation.
   * Indeed, developing the multinome is likely to increase the numbers of
   * operations and lead to precision loss. */
  if (!letPowerAtRoot
      && reductionContext.target() != ExpressionNode::ReductionTarget::SystemForApproximation
      && indexType == ExpressionNode::Type::Rational
      && !static_cast<Rational &>(index).signedIntegerNumerator().isZero()
      && static_cast<Rational &>(index).isInteger()
      && baseType == ExpressionNode::Type::Addition)
  {
    // Exponent n
    Rational rationalIndex = static_cast<Rational &>(index);
    Integer n = rationalIndex.unsignedIntegerNumerator();
    /* If n is above 25, the resulting sum would have more than
     * k_maxNumberOfTermsInExpandedMultinome terms so we do not expand it. */
    if (Integer(k_maxNumberOfTermsInExpandedMultinome).isLowerThan(n) || n.isOne()) {
      return *this;
    }
    int clippedN = n.extractedInt(); // Authorized because n < k_maxNumberOfTermsInExpandedMultinome
    assert(clippedN > 0);

    // Number of terms in addition m
    int m = base.numberOfChildren();
    /* The multinome (a0+a2+...+a(m-1))^n has BinomialCoefficient(n+m-1,n) terms;
     * we expand the multinome only when the number of terms in the resulting
     * sum has less than k_maxNumberOfTermsInExpandedMultinome terms. */
    if (k_maxNumberOfTermsInExpandedMultinome < BinomialCoefficientNode::compute(static_cast<double>(clippedN), static_cast<double>(clippedN+m-1))) {
      return *this;
    }

    Expression result = base;
    Expression a = result.clone();
    for (int i = 2; i <= clippedN; i++) {
      // result = result * (a0+a1+...+a(m-1) in its expanded form
      if (result.type() == ExpressionNode::Type::Addition) {
        // We need a 'double' distribution and newA will hold the new expanded form
        Expression newA = Addition::Builder();
        for (int j = 0; j < a.numberOfChildren(); j++) {
          Expression m = Multiplication::Builder(result.clone(), a.childAtIndex(j).clone()).distributeOnOperandAtIndex(0, reductionContext);
          if (newA.type() == ExpressionNode::Type::Addition) {
            static_cast<Addition &>(newA).addChildAtIndexInPlace(m, newA.numberOfChildren(), newA.numberOfChildren());
          } else {
            newA = Addition::Builder(newA, m);
          }
          newA = newA.shallowReduce(reductionContext);
        }
        result.replaceWithInPlace(newA);
        result = newA;
      } else {
        // Just distribute result on a
        Multiplication m = Multiplication::Builder(a.clone(), result.clone());
        Expression distributedM = m.distributeOnOperandAtIndex(0, reductionContext);
        result.replaceWithInPlace(distributedM);
        result = distributedM;
        result = result.shallowReduce(reductionContext);
      }
    }
    if (rationalIndex.sign() == ExpressionNode::Sign::Negative) {
      rationalIndex.replaceWithInPlace(Rational::Builder(-1));
      return shallowReduce(reductionContext);
    } else {
      replaceWithInPlace(result);
      return result;
    }
  }
#if 0
  /* We could use the Newton formula instead which is quicker but not immediate
   * to implement in the general case (Newton multinome). */
  // (a+b)^n with n integer -> a^n+?a^(n-1)*b+?a^(n-2)*b^2+...+b^n (Newton)
  if (!letPowerAtRoot && childAtIndex(1)->type() == ExpressionNode::Type::Rational && static_cast<const Rational *>(childAtIndex(1))->isInteger() && childAtIndex(0)->type() == ExpressionNode::Type::Addition && childAtIndex(0)->numberOfChildren() == 2) {
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
      p0->shallowReduce(reductionContext);
      p1->shallowReduce(reductionContext);
      a->addOperand(m);
      m->shallowReduce(reductionContext);
    }
    if (nr->sign(&context) == Sign::Negative) {
      nr->replaceWith(new Rational::Builder(-1), true);
      childAtIndex(0)->replaceWith(a, true)->shallowReduce(reductionContext);
      return shallowReduce(reductionContext);
    } else {
      return replaceWith(a, true)->shallowReduce(reductionContext);
    }
  }
#endif
  return *this;
}

Expression Power::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  // Step 1: X^-y -> 1/(X->shallowBeautify)^y
  Expression p = denominator(*reductionContext);
  // If the denominator is initialized, the index of the power is of form -y
  if (!p.isUninitialized()) {
    Division d = Division::Builder(Rational::Builder(1), p);
    replaceWithInPlace(d);
    p.shallowReduce(*reductionContext);
    return d.shallowBeautify(reductionContext);
  }
  // Step 2: Turn a^(1/n) into root(a, n), unless base is a unit
  if (childAtIndex(1).type() == ExpressionNode::Type::Rational && childAtIndex(1).convert<Rational>().signedIntegerNumerator().isOne() && childAtIndex(0).type() != ExpressionNode::Type::Unit) {
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

  // Step 4: Force Float(1) in front of an orphan Power of Unit
  if (parent().isUninitialized() && childAtIndex(0).type() == ExpressionNode::Type::Unit) {
    Multiplication m = Multiplication::Builder(Float<double>::Builder(1.0));
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(*this, 1, 1);
    return std::move(m);
  }

  return *this;
}

bool Power::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  /* Generalized power derivation formula
   * (f^g)` = (e^(g * ln(f)))`
   *       = (g * ln(f))` * f^g
   *        = (g`ln(f) + gf`/f) * f^g
   *        = g`ln(f)f^g + gf`f^(g-1)
   *
   * Valid whenever f,g are derivable and f > 0 */

  /* We might want to be able to derivate f^n when f <= 0 and n is a positive
   * integer */

  Expression base = childAtIndex(0);
  Expression exponent = childAtIndex(1);
  Multiplication derivedFromBase = Multiplication::Builder();
  Multiplication derivedFromExponent = Multiplication::Builder();

  derivedFromExponent.addChildAtIndexInPlace(NaperianLogarithm::Builder(base.clone()), 0, 0);
  derivedFromExponent.addChildAtIndexInPlace(clone(), 1, 1);
  derivedFromExponent.addChildAtIndexInPlace(Derivative::Builder(
    exponent.clone(),
    symbol.clone().convert<Symbol>(),
    symbolValue.clone()
    ), 2, 2);

  derivedFromBase.addChildAtIndexInPlace(exponent.clone() , 0, 0);
  derivedFromBase.addChildAtIndexInPlace(Power::Builder(
    base.clone(),
    Subtraction::Builder(exponent.clone(), Rational::Builder(1))
    ), 1, 1);
  derivedFromBase.addChildAtIndexInPlace(Derivative::Builder(
    base.clone(),
    symbol.clone().convert<Symbol>(),
    symbolValue.clone()
    ), 2, 2);

  Addition result = Addition::Builder(derivedFromBase, derivedFromExponent);
  replaceWithInPlace(result);
  return true;
}

// Private

// Simplification
Expression Power::denominator(ExpressionNode::ReductionContext reductionContext) const {
  if (childAtIndex(0).type() == ExpressionNode::Type::Unit) {
    return Expression();
  }
  Expression pow = clone();
  // If the power is of form x^(-y), denominator should be x^y
  Expression positiveIndex = pow.childAtIndex(1).makePositiveAnyNegativeNumeralFactor(reductionContext);
  if (positiveIndex.isUninitialized()) {
    return Expression();
  }
  // we cannot shallowReduce pow as it is not attached to its parent yet
  // if y was -1, pow is now x^1, denominator is then only x
  if (positiveIndex.isRationalOne()) {
    return pow.childAtIndex(0);
  }
  return pow;
}

Expression Power::simplifyPowerPower(ExpressionNode::ReductionContext reductionContext) {
  // this is p^e = (a^b)^e, we want a^(b*e)
  Expression p = childAtIndex(0);
  Multiplication m = Multiplication::Builder(p.childAtIndex(1), childAtIndex(1));
  replaceChildAtIndexInPlace(0, p.childAtIndex(0));
  replaceChildAtIndexInPlace(1, m);
  m.shallowReduce(reductionContext);
  return shallowReduce(reductionContext);
}

Expression Power::simplifyPowerMultiplication(ExpressionNode::ReductionContext reductionContext) {
  // this is m^r= (a*b*c*...)^r, we want a^r * b^r *c^r * ...
  Expression m = childAtIndex(0);
  Expression r = childAtIndex(1);
  for (int index = 0; index < m.numberOfChildren(); index++) {
    Power p = Power::Builder(m.childAtIndex(index).clone(), r.clone()); // We copy r and factor to avoid inheritance issues
    m.replaceChildAtIndexInPlace(index, p);
    p.shallowReduce(reductionContext);
  }
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

Expression Power::simplifyRationalRationalPower(ExpressionNode::ReductionContext reductionContext) {
  // this is a^b with a, b rationals
  Rational a = childAtIndex(0).convert<Rational>();
  Rational b = childAtIndex(1).convert<Rational>();
  if (b.isInteger()) {
    Rational r = Rational::IntegerPower(a, b.signedIntegerNumerator());
    if (r.numeratorOrDenominatorIsInfinity()) {
      return Power::Builder(a, b);
    }
    replaceWithInPlace(r);
    return std::move(r);
  }
  Expression n;
  Expression d;
  if (b.sign() == ExpressionNode::Sign::Negative) {
    b.setSign(ExpressionNode::Sign::Positive);
    n = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, false, reductionContext);
    d = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, true, reductionContext);
  } else {
    n = CreateSimplifiedIntegerRationalPower(a.signedIntegerNumerator(), b, false, reductionContext);
    d = CreateSimplifiedIntegerRationalPower(a.integerDenominator(), b, true, reductionContext);
  }
  Multiplication m = Multiplication::Builder(n, d);
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

Expression Power::CreateSimplifiedIntegerRationalPower(Integer i, Rational r, bool isDenominator, ExpressionNode::ReductionContext reductionContext) {
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
    Expression rClone = r.clone().setSign(isDenominator ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive, reductionContext);
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
    return std::move(p);
  }
  Integer one(1);
  Rational r3 = isDenominator ? Rational::Builder(one, r1) : Rational::Builder(r1);
  Multiplication m = Multiplication::Builder();
  m.addChildAtIndexInPlace(r3, 0, 0);
  if (!r2.isOne()) {
    m.addChildAtIndexInPlace(p, 1, 1);
  }
  if (i.isNegative()) {
    if (reductionContext.complexFormat()  == Preferences::ComplexFormat::Real) {
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
      r.setSign(isDenominator ? ExpressionNode::Sign::Negative : ExpressionNode::Sign::Positive);
      Expression exp = CreateComplexExponent(r, reductionContext);
      m.addChildAtIndexInPlace(exp, m.numberOfChildren(), m.numberOfChildren());
      exp.shallowReduce(reductionContext);
    }
  }
  return m.shallowReduce(reductionContext);
}

Expression Power::removeSquareRootsFromDenominator(ExpressionNode::ReductionContext reductionContext) {
  assert(reductionContext.target() == ExpressionNode::ReductionTarget::User);
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
      sqrt.shallowReduce(reductionContext);
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
    numerator = numerator.deepReduce(reductionContext);
    Integer one(1);
    result = Multiplication::Builder(numerator, Rational::Builder(one, denominator));
  }

  if (!result.isUninitialized()) {
    replaceWithInPlace(result);
    result = result.shallowReduce(reductionContext);
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
    /* Because p is a logarthim, we simplified p.childAtIndex(1) before
     * p.childAtIndex(0), so p.childAtIndex(1) can be compared to childAtIndex(0). */
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

Expression Power::CreateComplexExponent(const Expression & r, ExpressionNode::ReductionContext reductionContext) {
  // Returns e^(i*pi*r)
  const Constant exp = Constant::Builder(UCodePointScriptSmallE);
  Constant iComplex = Constant::Builder(UCodePointMathematicalBoldSmallI);
  const Constant pi = Constant::Builder(UCodePointGreekSmallLetterPi);
  Multiplication mExp = Multiplication::Builder(iComplex, pi, r.clone());
  iComplex.shallowReduce(reductionContext);
  Power p = Power::Builder(exp, mExp);
  mExp.shallowReduce(reductionContext);
  return std::move(p);
#if 0
  const Constant iComplex = Constant::Builder(UCodePointMathematicalBoldSmallI);
  const Constant pi = Constant::Builder(UCodePointGreekSmallLetterPi);
  Expression op = Multiplication::Builder(pi, r).shallowReduce(reductionContext, false);
  Cosine cos = Cosine(op).shallowReduce(reductionContext, false);;
  Sine sin = Sine(op).shallowReduce(reductionContext, false);
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
template Complex<double> PowerNode::computeNotPrincipalRealRootOfRationalPow<double>(std::complex<double>, double, double);
template Complex<float> PowerNode::computeNotPrincipalRealRootOfRationalPow<float>(std::complex<float>, float, float);

}
