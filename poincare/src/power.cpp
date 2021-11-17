#include <poincare/power.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
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
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/trigonometry.h>
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

ExpressionNode::NullStatus PowerNode::nullStatus(Context * context) const {
  // In practice, calling nullStatus on a reduced power always returns Unknown.
  ExpressionNode * base = childAtIndex(0);
  ExpressionNode::NullStatus baseNullStatus = base->nullStatus(context);
  ExpressionNode * index = childAtIndex(1);
  ExpressionNode::NullStatus indexNullStatus = index->nullStatus(context);
  if (indexNullStatus == ExpressionNode::NullStatus::Null && baseNullStatus == ExpressionNode::NullStatus::NonNull) {
    // x^0 is non null
    return ExpressionNode::NullStatus::NonNull;
  }
  if (indexNullStatus == ExpressionNode::NullStatus::NonNull && baseNullStatus == ExpressionNode::NullStatus::Null && index->sign(context) == Sign::Positive ) {
    // 0^+x is null
    return ExpressionNode::NullStatus::Null;
  }
  // Nothing else can be assumed because base and index could be infinite.
  return ExpressionNode::NullStatus::Unknown;
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
  if (childAtIndex(1)->type() == ExpressionNode::Type::Rational) {
    RationalNode * r = static_cast<RationalNode *>(childAtIndex(1));
    if (!r->isInteger() || Number(r).sign() == Sign::Negative) {
      return -1;
    }
    Integer numeratorInt = r->signedNumerator();
    if (!numeratorInt.isExtractable()) {
      return -1;
    }
    op0Deg *= numeratorInt.extractedInt();
    return op0Deg;
  }
  return -1;
}

Expression PowerNode::removeUnit(Expression * unit) {
  return Power(this).removeUnit(unit);
}

int PowerNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
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
   * Let's determine when the arg [Pi] (or arg [Pi/2]) is negligeable:
   * With c = r*e^(iθ) and d = x+iy, c^d = r^x*e^(yθ)*e^i(yln(r)+xθ)
   * so arg(c^d) = y*ln(r)+xθ.
   * We consider that arg[π] is negligeable if it is negligeable compared to
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
  if (childAtIndex(0)->type() == Type::Constant && static_cast<const ConstantNode *>(childAtIndex(0))->isExponential() && indexOfChild(child) == 1) {
    return static_cast<const ExpressionNode *>(child)->type() != Type::Parenthesis;
  }
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

int PowerNode::simplificationOrderGreaterType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  int baseComparison = SimplificationOrder(childAtIndex(0), e, ascending, ignoreParentheses);
  if (baseComparison != 0) {
    return baseComparison;
  }
  Rational one = Rational::Builder(1);
  return SimplificationOrder(childAtIndex(1), one.node(), ascending, ignoreParentheses);
}

int PowerNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  assert(e->numberOfChildren() > 0);
  int baseComparison = SimplificationOrder(childAtIndex(0), e->childAtIndex(0), ascending, ignoreParentheses);
  if (baseComparison != 0) {
    return baseComparison;
  }
  assert(e->numberOfChildren() > 1);
  return SimplificationOrder(childAtIndex(1), e->childAtIndex(1), ascending, ignoreParentheses);
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
  if (childAtIndex(0).type() == ExpressionNode::Type::Symbol
      && strcmp(childAtIndex(0).convert<Symbol>().name(), symbolName) == 0
      && childAtIndex(1).type() == ExpressionNode::Type::Rational)
  {
    Rational r = childAtIndex(1).convert<Rational>();
    if (!r.isInteger() || r.sign() == ExpressionNode::Sign::Negative) {
      return -1;
    }
    Integer num = r.unsignedIntegerNumerator();
    if (!num.isExtractable()) {
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

static bool isSquare(Expression e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return false;
  }
  Expression c = e.childAtIndex(1);
  return c.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(c).isTwo();
}

static int indexOfChildWithSquare(Expression e) {
  /* We look for a square to reduce a^2+2ab+b^2.
   * We always need to look in the three children, because there are such
   * expressions with only one square (e.g. (π+1)^2 = π^2+2π+1).
   * We also look in children that are products, as they can contain squares
   * (e.g. (2π+1)^2 = 4π^2+4π+1) */
  assert(e.type() == ExpressionNode::Type::Addition);
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression c = e.childAtIndex(i);
    if (isSquare(c)) {
      return i;
    }
    if (c.type() == ExpressionNode::Type::Multiplication) {
      int n2 = c.numberOfChildren();
      for (int j = 0; j < n2; j++) {
        if (isSquare(c.childAtIndex(j))) {
          return i;
        }
      }
    }
  }
  return -1;
}

Expression Power::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Context * context = reductionContext.context();
  Expression base = childAtIndex(0);
  Expression index = childAtIndex(1);
  ExpressionNode::Type baseType = base.type();
  ExpressionNode::Type indexType = index.type();

  /* Step 1
   * Simple cases where the power is undefined. */
  Expression indexUnit;
  index = index.removeUnit(&indexUnit);
  if (index.deepIsMatrix(context)
   || !indexUnit.isUninitialized() || index.isUndefined()
   || (base.hasUnit() && !(index.type() == ExpressionNode::Type::Rational)))
  {
    return replaceWithUndefinedInPlace();
  }

  /* Step 2
   * Handle matrices in the base.
   * We already know there are no matrices in the index. */
  if (base.deepIsMatrix(context)) {
    if (indexType != ExpressionNode::Type::Rational || !static_cast<Rational &>(index).isInteger()) {
      return replaceWithUndefinedInPlace();
    }
    if (baseType == ExpressionNode::Type::Matrix) {
      Matrix matrixBase = static_cast<Matrix &>(base);
      int baseNumberOfRows = matrixBase.numberOfRows();
      if (baseNumberOfRows != matrixBase.numberOfColumns()) {
        return replaceWithUndefinedInPlace();
      }
      Integer integerIndex = static_cast<Rational &>(index).signedIntegerNumerator();
      if (integerIndex.isNegative()) {
        index.setSign(ExpressionNode::Sign::Positive, reductionContext);
        Expression e = shallowReduce(reductionContext);
        if (e.type() == ExpressionNode::Type::Power) {
          /* The power cannot be reduced, return to avoid an infinite loop. */
          e.childAtIndex(1).setSign(ExpressionNode::Sign::Negative, reductionContext);
          return e;
        }
        Expression temp = Undefined::Builder();
        MatrixInverse inv = MatrixInverse::Builder(temp);
        e.replaceWithInPlace(inv);
        temp.replaceWithInPlace(e);
        return inv.shallowReduce(reductionContext);
      }
      if (Integer::NaturalOrder(integerIndex, Integer(k_maxExactPowerMatrix)) > 0) {
        return *this;
      }
      /* Ok because 0 < index < k_maxExactPowerMatrix */
      int i = integerIndex.extractedInt();
      Expression result = Matrix::CreateIdentity(baseNumberOfRows);
      /* TODO: implement a quick exponentiation */
      for (int j = 0; j < i; j++) {
        result = Multiplication::Builder(result, matrixBase.clone()).shallowReduce(reductionContext);
      }
      replaceWithInPlace(result);
      return result;
    }
  }

  /* Step 3
   * Trivial simplifications when base or index is 0, 1 or infinity. */
  Expression trivialResult;
  ExpressionNode::Sign baseSign = base.sign(context);
  ExpressionNode::Sign indexSign = index.sign(context);
  ExpressionNode::NullStatus indexNull = index.nullStatus(context);
  if (base.type() == ExpressionNode::Type::Infinity) {
    if (indexNull == ExpressionNode::NullStatus::Null) {
      // inf^0 -> undef
      return replaceWithUndefinedInPlace();
    }
    switch (indexSign) {
      case ExpressionNode::Sign::Negative:
        // inf^-x -> 0
        trivialResult = Rational::Builder(0);
        break;
      case ExpressionNode::Sign::Positive:
        // +inf^+x -> +inf
        // -inf^+x -> +inf * (-1)^+x
        trivialResult = Infinity::Builder(false);
        if (baseSign == ExpressionNode::Sign::Negative) {
          Power p = Power::Builder(Rational::Builder(-1), index);
          trivialResult = Multiplication::Builder(p, trivialResult);
          p.shallowReduce(reductionContext);
        }
        break;
      default:
        break;
    }
  } else if (baseType == ExpressionNode::Type::Rational && static_cast<Rational &>(base).isOne() && !index.recursivelyMatches(Expression::IsInfinity, context)) {
    // 1^x -> 1
    trivialResult = Rational::Builder(1);
  } else {
    ExpressionNode::NullStatus baseNull = base.nullStatus(context);
    if (baseNull == ExpressionNode::NullStatus::Null) {
      if (indexSign == ExpressionNode::Sign::Negative || indexNull == ExpressionNode::NullStatus::Null) {
        // 0^0 or 0^-x -> undef
        trivialResult = Undefined::Builder();
      } else if (indexSign == ExpressionNode::Sign::Positive && indexNull == ExpressionNode::NullStatus::NonNull) {
        // 0^+x -> 0
        trivialResult = Rational::Builder(0);
      }
    } else if (indexNull == ExpressionNode::NullStatus::Null && (baseNull == ExpressionNode::NullStatus::NonNull || reductionContext.target() == ExpressionNode::ReductionTarget::User)) {
      // x^0 -> 1
      trivialResult = Rational::Builder(1);
    }
  }
  if (!trivialResult.isUninitialized()) {
    replaceWithInPlace(trivialResult);
    return trivialResult.shallowReduce(reductionContext);
  }

  /* Step 4
   * Handle logarithms of same base.
   * log(x^y, x) is handled by the parent logarithm, but x^log(y,x) is handled
   * here. */
  Expression p = parent();
  if (!p.isUninitialized() && p.indexOfChild(*this) == 0 && isLogarithmOfSameBase(p)) {
    return *this;
  }
  if (isLogarithmOfSameBase(index)) {
    Expression e = index.childAtIndex(0);
    if (e.sign(context) == ExpressionNode::Sign::Positive) {
      replaceWithInPlace(e);
      return e;
    }
  }

  /* Step 5
   * Handle complex numbers. */
  bool baseIsReal = base.isReal(context);
  bool baseIsCartesian = baseType == ExpressionNode::Type::ComplexCartesian;
  bool indexIsReal = index.isReal(context);
  bool indexIsCartesian = indexType == ExpressionNode::Type::ComplexCartesian;
  if (!(baseIsReal || indexIsReal || baseIsCartesian || indexIsCartesian)) {
    /* Step 5.1
     * If both children are true unresolved complexes, the result is not
     * reduced.
     * TODO: Handle this reduction. */
    return *this;
  }
  if (baseIsCartesian && indexType == ExpressionNode::Type::Rational) {
    /* Step 5.2
     * Some simple reduction to avoid using the arctangent because of the
     * formula r*e^(i*th))^(a+ib) = r^a*e(-th*b)*e^(b*ln(r)+th*a. */
    ComplexCartesian complexBase = static_cast<ComplexCartesian &>(base);
    Rational rationalIndex = static_cast<Rational &>(index);
    ComplexCartesian res;
    if (rationalIndex.isMinusOne()) {
      res = complexBase.inverse(reductionContext);
    } else if (rationalIndex.isHalf()) {
      res = complexBase.squareRoot(reductionContext);
    } else if (rationalIndex.isMinusHalf()) {
      res = complexBase.squareRoot(reductionContext).inverse(reductionContext);
    } else if (rationalIndex.isInteger() && rationalIndex.unsignedIntegerNumerator().isLowerThan(Integer(10))) {
      res = complexBase.powerInteger(rationalIndex.unsignedIntegerNumerator().extractedInt(), reductionContext);
      if (indexSign == ExpressionNode::Sign::Negative) {
        res = res.inverse(reductionContext);
      }
    }
    if (!res.isUninitialized()) {
      replaceWithInPlace(res);
      return res.shallowReduce(reductionContext);
    }
  }
  if ((baseIsReal && indexIsCartesian) || (baseIsCartesian && indexIsReal) || (baseIsCartesian && indexIsCartesian)) {
    /* Step 5.3
     * General case, where both children are cartesian or real, with at least
     * one complex. */
    ComplexCartesian complexBase = baseIsCartesian ? static_cast<ComplexCartesian &>(base) : ComplexCartesian::Builder(base, Rational::Builder(0));
    ComplexCartesian complexIndex = indexIsCartesian ? static_cast<ComplexCartesian &>(index) : ComplexCartesian::Builder(index, Rational::Builder(0));
    Expression result = complexBase.power(complexIndex, reductionContext);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  /* Step 6
   * Reduce nth roots of unity to their trigonometric form
   * exp(r*i*pi) -> cos(r*pi)+i*sin(r*pi) */
  if (isNthRootOfUnity()) {
    Expression angle = Trigonometry::PiExpressionInAngleUnit(reductionContext.angleUnit());
    if (index.numberOfChildren() == 3) {
      angle = Multiplication::Builder(index.childAtIndex(0), angle).shallowReduce(reductionContext);
    }
    Expression cosE = Cosine::Builder(angle.clone());
    Expression sinE = Sine::Builder(angle);
    Expression imPart = Multiplication::Builder(index.childAtIndex(index.numberOfChildren() - 2), sinE);
    sinE.shallowReduce(reductionContext);
    Expression trigForm = Addition::Builder(cosE, imPart);
    cosE.shallowReduce(reductionContext);
    imPart.shallowReduce(reductionContext);
    replaceWithInPlace(trigForm);
    return trigForm.shallowReduce(reductionContext);
  }

  /* Step 7
   * Break down a sum in the index if the base and a term in the index are
   * rational.
   * r^(s+a+...) -> r^s*r^(a+...) */
  if (baseType == ExpressionNode::Type::Rational && indexType == ExpressionNode::Type::Addition && index.childAtIndex(0).type() == ExpressionNode::Type::Rational) {
    Rational rationalBase = static_cast<Rational &>(base);
    Addition additionIndex = static_cast<Addition &>(index);
    Rational rationalIndex = index.childAtIndex(0).convert<Rational>();
    Expression p1 = PowerRationalRational(rationalBase, rationalIndex, reductionContext);
    if ((rationalIndex.unsignedIntegerNumerator().isOne() && !rationalIndex.isInteger()) || p1.isUninitialized()) {
      /* Escape here to avoid infinite loops with the multiplication.
       * TODO: do something more sensible here:
       * - add rule (-rational)^x --> (-1)^x*rational^x so we only consider
       *   positive rational or (-1)
       * - change simplifyRationalRationalPower to be able to detect when no
       *   rational was extracted (ie 2^(1/2) --> 2^(1/2)) to avoid applying
       *   this rule in that case */
      return *this;
    }
    additionIndex.removeChildAtIndexInPlace(0);
    /* If the addition had only 2 children. */
    additionIndex.squashUnaryHierarchyInPlace();
    Multiplication m = Multiplication::Builder(p1);
    Expression thisRef = *this;
    replaceWithInPlace(m);
    m.addChildAtIndexInPlace(thisRef, 1, 1);
    return m.shallowReduce(reductionContext);
  }

  /* From this point onward, all simplifications assume index is rational. */
  if (indexType != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational rationalIndex = static_cast<Rational &>(index);
  if (rationalIndex.isOne()) {
    replaceWithInPlace(base);
    return base;
  }

  /* Step 8
   * Handle the simple case of r^s, whith r and s rational. */
  if (baseType == ExpressionNode::Type::Rational) {
    Rational rationalBase = static_cast<Rational &>(base);
    Expression e = PowerRationalRational(rationalBase, rationalIndex, reductionContext);
    if (e.isUninitialized()) {
      return *this;
    }
    replaceWithInPlace(e);
    return e;
  }

  /* Step 9
   * Depending on the target, the constant 𝐢 is not always reduced to a
   * cartesian, but its rational power can be simplified nonetheless. */
  if (baseType == ExpressionNode::Type::Constant && static_cast<Constant &>(base).isIComplex()) {
    Expression result;
    Integer numerator = rationalIndex.signedIntegerNumerator();
    Integer four(4);
    Integer rem = Integer::Division(numerator, four).remainder;
    if (rem.isZero()) {
      result = Rational::Builder(1);
    } else if (rem.isOne()) {
      result = base;
    } else if (rem.isTwo()) {
      result = Rational::Builder(-1);
    } else if (rem.isThree()) {
      result = Multiplication::Builder({ Rational::Builder(-1), base });
    }
    Integer one(1);
    Integer denominator = rationalIndex.integerDenominator();
    Power p = Power::Builder(result, Rational::Builder(one, denominator));
    replaceWithInPlace(p);
    return std::move(p);
  }

  /* Step 10
   * Distribute power over the multiplication */
  int baseChildren = base.numberOfChildren();
  if (baseType == ExpressionNode::Type::Multiplication) {
    if (rationalIndex.isInteger()) {
      // Case 1: (a*b*c*...)^n = a^n*b^n*c^n*... if n integer
      for (int i = 0; i < baseChildren; i++) {
        Expression p = Power::Builder(base.childAtIndex(i), index.clone());
        base.replaceChildAtIndexInPlace(i, p);
        p.shallowReduce(reductionContext);
      }
      replaceWithInPlace(base);
      return base.shallowReduce(reductionContext);
    }
    // Case 2: (a*b*...)^r -> |a|^r*(sign(a)*b*...)^r if a not -1
    Multiplication multiplicationBase = static_cast<Multiplication &>(base);
    for (int i = 0; i < baseChildren; i++) {
      Expression child = base.childAtIndex(i);
      ExpressionNode::Sign childSign = child.sign(context);
      if (childSign != ExpressionNode::Sign::Unknown && (child.type() != ExpressionNode::Type::Rational || !static_cast<Rational &>(child).isMinusOne())) {
        if (childSign == ExpressionNode::Sign::Negative) {
          multiplicationBase.replaceChildAtIndexInPlace(i, Rational::Builder(-1));
        } else {
          multiplicationBase.removeChildAtIndexInPlace(i);
        }
        multiplicationBase.shallowReduce(reductionContext);
        Power p = Power::Builder(child, index.clone());
        child.setSign(ExpressionNode::Sign::Positive, reductionContext);
        Multiplication m = Multiplication::Builder(p);
        p.shallowReduce(reductionContext);
        Power thisRef = *this;
        replaceWithInPlace(m);
        m.addChildAtIndexInPlace(thisRef, 1, 1);
        thisRef.shallowReduce(reductionContext);
        return m.shallowReduce(reductionContext);
      }
    }
  }

  /* Step 11
   * Merge with the base if it is a power: (a^b)^c -> a^(b*c)
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
   * Indeed, we have to apply the rule (a^b)^c -> a^(b*c) as soon as c is -1. */
  if (baseType == ExpressionNode::Type::Power) {
    Expression a = base.childAtIndex(0);
    bool apply = rationalIndex.isMinusOne()
      || a.sign(context) == ExpressionNode::Sign::Positive
      || a.approximateToScalar<float>(context, reductionContext.complexFormat(), reductionContext.angleUnit(), true) > 0.f;
    if (!apply && rationalIndex.isInteger()) {
      if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
        Expression approximation = base.approximate<float>(context, reductionContext.complexFormat(), reductionContext.angleUnit(), true);
        if (approximation.type() == ExpressionNode::Type::Unreal) {
          replaceWithInPlace(approximation);
          return approximation;
        }
        /* The inner power is undefined, it can be 'x^(1/2)' for instance. We
         * don't want to simplify this as it could be unreal with x = -2 but
         * also real with x = 2.
         * Testing if the approximation is real is a dirty trick to filter out
         * variables but not units. */
        apply = approximation.type() != ExpressionNode::Type::Undefined || a.isReal(context);
      } else {
        apply = true;
      }
    }
    if (apply) {
      Multiplication m = Multiplication::Builder(base.childAtIndex(1), index);
      replaceChildAtIndexInPlace(0, base.childAtIndex(0));
      replaceChildAtIndexInPlace(1, m);
      m.shallowReduce(reductionContext);
      return shallowReduce(reductionContext);
    }
  }

  /* Step 12
   * Handle the case where base is an addition. */
  if (baseType == ExpressionNode::Type::Addition) {
    /* Step 12.1
     * Develop the multinome if index is an integer, unless the target is
     * SystemForApproximation, as developping would increase the number of
     * operations and thus reduce precision. */
    if (rationalIndex.isInteger() && !rationalIndex.isMinusOne() && !rationalIndex.isZero()
     && reductionContext.target() != ExpressionNode::ReductionTarget::SystemForApproximation) {
      Integer n = rationalIndex.unsignedIntegerNumerator();
      /* If n is above 25, the resulting sum would have more than
       * k_maxNumberOfTermsInExpandedMultinome terms so we do not expand it. */
      if (Integer(k_maxNumberOfTermsInExpandedMultinome).isLowerThan(n)) {
        return *this;
      }
      // Allowed because n < k_maxNumberOfTermsInExpandedMultinome
      int clippedN = n.extractedInt();
      assert(clippedN > 0);

      /* The multinome (a0+a2+...+a(m-1))^n has BinomialCoefficient(n+m-1,n)
       * terms ; we expand the multinome only when the number of terms in the
       * resulting sum has less than k_maxNumberOfTermsInExpandedMultinome
       * terms. */
      if (k_maxNumberOfTermsInExpandedMultinome < BinomialCoefficientNode::compute(static_cast<double>(clippedN), static_cast<double>(clippedN + baseChildren - 1))) {
        return *this;
      }

      Expression result = base;
      Expression a = result.clone();
      for (int i = 2; i <= clippedN; i++) {
        /* result = result * (a0+a1+...+a(m-1) in its expanded form */
        if (result.type() == ExpressionNode::Type::Addition) {
          /* We need a 'double' distribution and newA will hold the new
           * expanded form. */
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
          /* Just distribute result on a */
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

    /* Step 12.2
     * Special simplifications available on sums of two square roots. */
    Expression a, b, c, d;
    if (baseChildren == 2
     && SquareRoot::SplitRadical(base.childAtIndex(0).clone(), &a, &b)
     && SquareRoot::SplitRadical(base.childAtIndex(1).clone(), &c, &d))
    {
      /* Step 12.2.1
       * √(a√b + c√d) */
      if (rationalIndex.isHalf() || rationalIndex.isMinusHalf()) {
        Expression e = SquareRoot::ReduceNestedRadicals(a, b, c, d, reductionContext);
        if (!e.isUninitialized()) {
          if (rationalIndex.isNegative()) {
            Power p = Power::Builder(e, Rational::Builder(-1));
            replaceWithInPlace(p);
            return p.shallowReduce(reductionContext);
          } else {
            replaceWithInPlace(e);
            return e;
          }
        }
      }
      /* Step 12.2.2
       * 1/(a√b + c√d) */
      if (rationalIndex.isMinusOne()) {
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
        assert(a.type() == ExpressionNode::Type::Rational
            && b.type() == ExpressionNode::Type::Rational
            && c.type() == ExpressionNode::Type::Rational
            && d.type() == ExpressionNode::Type::Rational);
        Integer n1 = static_cast<Rational &>(a).signedIntegerNumerator();
        Integer d1 = static_cast<Rational &>(a).integerDenominator();
        Integer p1 = static_cast<Rational &>(b).signedIntegerNumerator();
        Integer q1 = static_cast<Rational &>(b).integerDenominator();
        Integer n2 = static_cast<Rational &>(c).signedIntegerNumerator();
        Integer d2 = static_cast<Rational &>(c).integerDenominator();
        Integer p2 = static_cast<Rational &>(d).signedIntegerNumerator();
        Integer q2 = static_cast<Rational &>(d).integerDenominator();

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
          return Expression(); // Escape
        }
        numerator = numerator.deepReduce(reductionContext);
        Integer one(1);
        Expression result = Multiplication::Builder(numerator, Rational::Builder(one, denominator));
        replaceWithInPlace(result);
        return result.shallowReduce(reductionContext);
      }
    }

    /* Step 12.3
     * Try to reduce √(a^2±2ab+b^2) */
    if (baseChildren == 3 && (rationalIndex.isHalf() || rationalIndex.isMinusHalf())) {
      int squareIndex = indexOfChildWithSquare(base);
      if (squareIndex < 0 || squareIndex >= 3) {
        return *this;
      }
      Expression firstTerm = Power::Builder(base.childAtIndex(squareIndex).clone(), Rational::Builder(1, 2)).shallowReduce(reductionContext);
      Expression secondTerm;
      for (int i = 0; i < 3; i++) {
        if (i == squareIndex) {
          continue;
        }
        int j = 3 - i - squareIndex;
        secondTerm = Power::Builder(base.childAtIndex(i).clone(), Rational::Builder(1, 2)).shallowReduce(reductionContext);
        Expression m = Multiplication::Builder(Rational::Builder(2), firstTerm.clone(), secondTerm.clone()).shallowReduce(reductionContext);
        if (m.isIdenticalTo(base.childAtIndex(j))) {
          break;
        }
        m = Multiplication::Builder(m, Rational::Builder(-1)).shallowReduce(reductionContext);
        if (m.isIdenticalTo(base.childAtIndex(j))) {
          secondTerm = Multiplication::Builder(Rational::Builder(-1), secondTerm).shallowReduce(reductionContext);
          break;
        } else {
          secondTerm = Expression();
        }
      }
      if (secondTerm.isUninitialized()) {
        return *this;
      }
      Expression result = Addition::Builder(firstTerm, secondTerm);
      if (result.approximateToScalar<float>(context, reductionContext.complexFormat(), reductionContext.angleUnit(), true) < 0.f) {
        Multiplication m = Multiplication::Builder(Rational::Builder(-1), result);
        result.shallowReduce(reductionContext);
        result = m;
      }
      if (rationalIndex.isMinusHalf()) {
        Power p = Power::Builder(result, Rational::Builder(-1));
        result.shallowReduce(reductionContext);
        result = p;
      }
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }

  return *this;
}

Expression Power::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  // Step 1: X^-y -> 1/(X->shallowBeautify)^y
  Expression p = denominator(*reductionContext);
  // If the denominator is initialized, the index of the power is of form -y
  if (!p.isUninitialized()) {
    if (Trigonometry::isDirectTrigonometryFunction(p)) {
      // Replace this inverse with denominator's advanced equivalent.
      return Trigonometry::replaceWithAdvancedFunction(*this, p);
    }
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
  derivedFromExponent.addChildAtIndexInPlace(exponent.clone(), 2, 2);
  derivedFromExponent.derivateChildAtIndexInPlace(2, reductionContext, symbol, symbolValue);

  derivedFromBase.addChildAtIndexInPlace(exponent.clone() , 0, 0);
  derivedFromBase.addChildAtIndexInPlace(Power::Builder(
    base.clone(),
    Subtraction::Builder(exponent.clone(), Rational::Builder(1))
    ), 1, 1);
  derivedFromBase.addChildAtIndexInPlace(base.clone(), 2, 2);
  derivedFromBase.derivateChildAtIndexInPlace(2, reductionContext, symbol, symbolValue);

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

Expression Power::PowerRationalRational(Rational base, Rational index, ExpressionNode::ReductionContext reductionContext) {
  assert(!base.numeratorOrDenominatorIsInfinity() && !index.numeratorOrDenominatorIsInfinity());
  /* Handle this case right now to always reduce to Unreal if needed. */
  if (base.isNegative()) {
    Multiplication res = Multiplication::Builder();
    /* Compute -1^(a/b) */
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      /* On real numbers (-1)^(a/b) =
       * - 1 if a is even
       * - -1 if a and b are odd
       * - has no real solution otherwise */
      if (!index.unsignedIntegerNumerator().isEven()) {
        if (index.integerDenominator().isEven()) {
          return Unreal::Builder();
        } else {
          res.addChildAtIndexInPlace(Rational::Builder(-1), 0, res.numberOfChildren());
        }
      }
    } else {
      /* On complex numbers, we pick the first root (-1)^(a/b) = e^(i*pi*a/b)
       * We set the sign of index to positive to get the principal root. */
      Rational indexClone = index.clone().convert<Rational>();
      indexClone.setSign(ExpressionNode::Sign::Positive);
      Expression exp = CreateComplexExponent(indexClone, reductionContext);
      res.addChildAtIndexInPlace(exp, res.numberOfChildren(), res.numberOfChildren());
      exp.shallowReduce(reductionContext);
    }
    base.setSign(ExpressionNode::Sign::Positive);
    Expression res2 = PowerRationalRational(base, index, reductionContext);
    /* PowerRationalRational doesn't alter its arguments: restore the sign of
     * the base. */
    base.setSign(ExpressionNode::Sign::Negative);
    if (res2.isUninitialized()) {
      base.setSign(ExpressionNode::Sign::Negative);
      return Expression();
    } else {
      res.addChildAtIndexInPlace(res2, res.numberOfChildren(), res.numberOfChildren());
      return res.shallowReduce(reductionContext);
    }
  }
  if (RationalExponentShouldNotBeReduced(base, index)) {
    return Expression();
  }
  if (base.isZero()) {
    if (index.isNegative() || index.isZero()) {
      return Undefined::Builder();
    } else {
      return Rational::Builder(0);
    }
  }
  assert(!base.isZero());
  if (index.isZero()) {
    return Rational::Builder(1);
  }
  if (index.isNegative()) {
    base = Rational::IntegerPower(base, Integer(-1));
    if (index.isMinusOne()) {
      return std::move(base);
    }
    index.setSign(ExpressionNode::Sign::Positive);
    Expression res = PowerRationalRational(base, index, reductionContext);
    /* PowerRationalRational doesn't alter its arguments: restore the sign of
     * the index. */
    index.setSign(ExpressionNode::Sign::Negative);
    return res;
  }
  assert(!index.isNegative());
  /* We are handling an expression of the form (p/q)^(a/b), with a and b
   * positive. To avoid irrational numbers in the denominator, we turn it
   * into : (1/q)^(a+c)/b * (p^a * q^c)^(1/b), where c is the smallest positive
   * integer such that (a+c)/b is an integer. */
  Integer p = base.signedIntegerNumerator(), q = base.integerDenominator();
  Integer a = index.signedIntegerNumerator(), b = index.integerDenominator();
  assert(!a.isNegative());

  IntegerDivision divAB = Integer::Division(a, b);
  Integer c, d;
  if (divAB.remainder.isZero()) {
    c = Integer(0);
    d = divAB.quotient;
  } else {
    c = Integer::Subtraction(b, divAB.remainder);
    d = Integer::Addition(Integer(1), divAB.quotient);
  }
  d.setNegative(true);
  Rational f1 = Rational::IntegerPower(Rational::Builder(q), d);
  Expression f2 = PowerIntegerRational(p, index, reductionContext);
  Expression f3 = PowerIntegerRational(q, Rational::Builder(c, b), reductionContext);
  Multiplication m = Multiplication::Builder({ f1, f2, f3 });
  return m.shallowReduce(reductionContext);
}

Expression Power::PowerIntegerRational(Integer base, Rational index, ExpressionNode::ReductionContext reductionContext) {
  assert(!index.isNegative());
  if (base.isZero()) {
    if (index.isZero()) {
      return Undefined::Builder();
    } else {
      return Rational::Builder(0);
    }
  }
  assert(!base.isZero());
  if (index.isZero()) {
    return Rational::Builder(1);
  }
  Integer a = index.signedIntegerNumerator(), b = index.integerDenominator();
  /* This methods reduces base^(a/b) to the form i1*i2^(a*g/b), where i1 and i2
   * are integers, and g is an integer that divides b.
   * I.e. We get the largest factor possible out of the root. */

  Arithmetic arithmetic;
  int numberOfPrimeFactors = arithmetic.PrimeFactorization(base);
  if (numberOfPrimeFactors < 0) {
    /* Prime factorization failed. */
    return Power::Builder(Rational::Builder(base), index);
  }
  Integer i1(1), i2(1), g(b);
  /* g is defined as gcd(b, a*k_1, a*k_2, ...) where the k_i are the
    * coefficients in the prime factorization of base. As such, we need to
    * iterate over the coefficients twice: once to compute g, and once to
    * compute i1 and i2. */
  for (int j = 0; j < numberOfPrimeFactors; j++) {
    IntegerDivision div = Integer::Division(Integer::Multiplication(*arithmetic.coefficientAtIndex(j), a), b);
    g = Arithmetic::GCD(g, div.remainder);
  }
  for (int j = 0; j < numberOfPrimeFactors; j++) {
    IntegerDivision div = Integer::Division(Integer::Multiplication(*arithmetic.coefficientAtIndex(j), a), b);
    IntegerDivision div2 = Integer::Division(div.remainder, g);
    assert(div2.remainder.isZero());
    i1 = Integer::Multiplication(i1, Integer::Power(*arithmetic.factorAtIndex(j), div.quotient));
    i2 = Integer::Multiplication(i2, Integer::Power(*arithmetic.factorAtIndex(j), div2.quotient));
  }

  if (i2.isOverflow() || i1.isOverflow()) {
    return Power::Builder(Rational::Builder(base), index);
  }
  Integer b2 = Integer::Division(b, g).quotient;

  Multiplication m = Multiplication::Builder(Rational::Builder(i1));
  if (!i2.isOne()) {
    Integer one(1);
    m.addChildAtIndexInPlace(Power::Builder(Rational::Builder(i2), Rational::Builder(one, b2)), 1, 1);
  }
  return m.shallowReduce(reductionContext);
}

bool Power::isLogarithmOfSameBase(Expression e) const {
  if (e.isUninitialized()) {
    return false;
  }
  Expression base = childAtIndex(0);
  if (e.type() == ExpressionNode::Type::Logarithm) {
    /* Because e is a logarthim, we simplified e.childAtIndex(1) before
     * e.childAtIndex(0), so e.childAtIndex(1) can be compared to base even if
     * e is our parent. */
    if (e.numberOfChildren() == 1) {
      return base.type() == ExpressionNode::Type::Rational
          && static_cast<Rational &>(base).isTen();
    } else {
      assert(e.numberOfChildren() == 2);
      return base.isIdenticalTo(e.childAtIndex(1));
    }
  }
  return e.type() == ExpressionNode::Type::NaperianLogarithm
    && base.type() == ExpressionNode::Type::Constant
    && static_cast<Constant &>(base).isExponential();
}

bool Power::isNthRootOfUnity() const {
  /* Nth roots of unity are expression of the form exp(i*pi) or exp(r*i*pi)
   * with r rational.  */
  Expression base = childAtIndex(0);
  Expression index = childAtIndex(1);
  int n = index.numberOfChildren();
  if (base.type() != ExpressionNode::Type::Constant
      || index.type() != ExpressionNode::Type::Multiplication
      || n < 2 || n > 3
      || !static_cast<Constant &>(base).isExponential())
  {
    return false;
  }
  const Expression i = index.childAtIndex(n - 2);
  const Expression pi = index.childAtIndex(n - 1);
  if (i.type() != ExpressionNode::Type::Constant
      || !static_cast<const Constant &>(i).isIComplex()
      || pi.type() != ExpressionNode::Type::Constant
      || !static_cast<const Constant &>(pi).isPi())
  {
    return false;
  }
  return n == 2 || index.childAtIndex(0).type() == ExpressionNode::Type::Rational;
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

bool Power::RationalExponentShouldNotBeReduced(const Rational & b, const Rational & r) {
  if (r.isMinusOne()) {
    return false;
  }
  /* We check that the simplification does not involve too complex power of
   * integers (ie 3^999, 120232323232^50) that would take too much time to
   * compute:
   *  - we cap the exponent at k_maxExactPowerMatrix
   *  - we cap the resulting power at FLT_MAX, as setting it any higher would
   *    prevent the drawing of some curves.
   * The complexity of computing a power of rational is mainly due to computing
   * the GCD of the resulting numerator and denominator. Euclide algorithm's
   * complexity is apportionned to the number of decimal digits in the smallest
   * integer. */
  Integer maxIntegerExponent = r.unsignedIntegerNumerator();
  if (Integer::NaturalOrder(maxIntegerExponent, Integer(k_maxExactPowerMatrix)) > 0) {
    return true;
  }

  float index = maxIntegerExponent.approximate<float>();
  float powerNumerator = std::pow(b.unsignedIntegerNumerator().approximate<float>(), index);
  float powerDenominator = std::pow(b.integerDenominator().approximate<float>(), index);
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
