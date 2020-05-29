#include <poincare/binomial_coefficient.h>
#include <poincare/binomial_coefficient_layout.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper BinomialCoefficient::s_functionHelper;

int BinomialCoefficientNode::numberOfChildren() const { return BinomialCoefficient::s_functionHelper.numberOfChildren(); }

Expression BinomialCoefficientNode::shallowReduce(ReductionContext reductionContext) {
  return BinomialCoefficient(this).shallowReduce(reductionContext.context());
}

Layout BinomialCoefficientNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return BinomialCoefficientLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int BinomialCoefficientNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.name());
}

template<typename T>
Complex<T> BinomialCoefficientNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> nInput = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> kInput = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T n = nInput.toScalar();
  T k = kInput.toScalar();
  return Complex<T>::Builder(compute(k, n));
}

template<typename T>
T BinomialCoefficientNode::compute(T k, T n) {
  if (std::isnan(n) || std::isnan(k) || k != std::round(k) || k < 0) {
    return NAN;
  }
  // Generalized definition allows any n value
  bool generalized = (n != std::round(n) || n < k);
  // Take advantage of symmetry
  k = (!generalized && k > (n - k)) ? n - k : k;

  T result = 1;
  for (int i = 0; i < k; i++) {
    result *= (n - (T)i) / (k - (T)i);
    if (std::isinf(result) || std::isnan(result)) {
      return result;
    }
  }
  // If not generalized, the output must be round
  return generalized ? result : std::round(result);
}


Expression BinomialCoefficient::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);

  if (c0.deepIsMatrix(context) || c1.deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }

  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  if (!r1.isInteger() || r1.isNegative()) {
    return replaceWithUndefinedInPlace();
  }

  if (!r0.isInteger()) {
    // Generalized binomial coefficient
    return *this;
  }

  Integer n = r0.signedIntegerNumerator();
  Integer k = r1.signedIntegerNumerator();
  if (n.isLowerThan(k)) {
    // Generalized binomial coefficient
    return *this;
  }
  /* If n is too big, we do not reduce in order to avoid too long computation.
   * The binomial coefficient will be approximatively evaluated later. */
  if (Integer(k_maxNValue).isLowerThan(n)) {
    return *this;
  }
  Rational result = Rational::Builder(1);
  Integer kBis = Integer::Subtraction(n, k);
  k = kBis.isLowerThan(k) ? kBis : k;
  int clippedK = k.extractedInt(); // Authorized because k < n < k_maxNValue
  for (int i = 0; i < clippedK; i++) {
    Integer nMinusI = Integer::Subtraction(n, Integer(i));
    Integer kMinusI = Integer::Subtraction(k, Integer(i));
    Rational factor = Rational::Builder(nMinusI, kMinusI);
    result = Rational::Multiplication(result, factor);
  }
  // As we cap the n < k_maxNValue = 300, result < binomial(300, 150) ~2^89
  assert(!result.numeratorOrDenominatorIsInfinity());
  replaceWithInPlace(result);
  return std::move(result);
}

template double BinomialCoefficientNode::compute(double k, double n);
template float BinomialCoefficientNode::compute(float k, float n);

}
