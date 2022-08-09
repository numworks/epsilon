#include <poincare/binomial_coefficient.h>
#include <poincare/binomial_coefficient_layout.h>
#include <poincare/approximation_helper.h>
#include <poincare/letter_c_with_sub_and_superscript_layout.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <poincare/preferences.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

int BinomialCoefficientNode::numberOfChildren() const { return BinomialCoefficient::s_functionHelper.numberOfChildren(); }

Expression BinomialCoefficientNode::shallowReduce(const ReductionContext& reductionContext) {
  return BinomialCoefficient(this).shallowReduce(reductionContext);
}

Layout BinomialCoefficientNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  Layout child0 = childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  Layout child1 = childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  if (Preferences::sharedPreferences->combinatoricSymbols() == Preferences::CombinatoricSymbols::Default) {
    return BinomialCoefficientLayout::Builder(child0, child1);
  } else {
    return LetterCWithSubAndSuperscriptLayout::Builder(child0, child1);
  }
}

int BinomialCoefficientNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> BinomialCoefficientNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 2);
        T n = ComplexNode<T>::ToScalar(c[0]);
        T k = ComplexNode<T>::ToScalar(c[1]);
        return Complex<T>::Builder(compute(k, n));
      });
}

template<typename T>
T BinomialCoefficientNode::compute(T k, T n) {
  if (std::isnan(n) || std::isnan(k) || k != std::round(k)) {
    return NAN;
  }
  if (k < 0) {
    return 0;
  }
  // Generalized definition allows any n value
  bool generalized = (n != std::round(n) || n < k);
  // Take advantage of symmetry
  k = (!generalized && k > (n - k)) ? n - k : k;

  T result = 1;
  for (T i = 0; i < k; i++) {
    result *= (n - i) / (k - i);
    if (std::isinf(result) || std::isnan(result)) {
      return result;
    }
  }
  // If not generalized, the output must be round
  return generalized ? result : std::round(result);
}


Expression BinomialCoefficient::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);

  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  if (!r1.isInteger()) {
    // Our factorial is only defined on integers.
    return replaceWithUndefinedInPlace();
  }

  if (r1.isNegative() || r1.isZero()) {
    // By convention, reduce to 0 if k is strictly negative, 1 if k is null.
    Rational result = r1.isZero() ? Rational::Builder(1) : Rational::Builder(0);
    replaceWithInPlace(result);
    return std::move(result);
  }

  if (!r0.isInteger()) {
    // Generalized binomial coefficient (n is not an integer)
    return *this;
  }

  Integer n = r0.signedIntegerNumerator();
  Integer k = r1.signedIntegerNumerator();
  /* Check for situations where there should be no reduction in order to avoid
   * too long computation and a huge result. The binomial coefficient will be
   * approximatively evaluated later. */
  if (n.isLowerThan(k)) {
    // Generalized binomial coefficient (n < k)
    if (!n.isNegative()) {
      // When n is an integer and 0 <= n < k, binomial(n,k) is 0.
      Expression res = Rational::Builder(0);
      replaceWithInPlace(res);
      return res;
    }
    if (Integer(k_maxNValue).isLowerThan(Integer::Subtraction(k, n))) {
      return *this;
    }
  } else if (Integer(k_maxNValue).isLowerThan(n)) {
    return *this;
  }
  Rational result = Rational::Builder(1);
  Integer kBis = Integer::Subtraction(n, k);
  // Take advantage of symmetry if n >= k
  k = !n.isLowerThan(k) && kBis.isLowerThan(k) ? kBis : k;
  // Authorized because k < k_maxNValue
  int clippedK = k.extractedInt();
  for (int i = 0; i < clippedK; i++) {
    Integer nMinusI = Integer::Subtraction(n, Integer(i));
    Integer kMinusI = Integer::Subtraction(k, Integer(i));
    Rational factor = Rational::Builder(nMinusI, kMinusI);
    result = Rational::Multiplication(result, factor);
  }
  // As we cap the n < k_maxNValue = 300, result < binomial(300, 150) ~10^89
  // If n was negative, k - n < k_maxNValue, result < binomial(-150,150) ~10^88
  assert(!result.numeratorOrDenominatorIsInfinity());
  replaceWithInPlace(result);
  return std::move(result);
}

template double BinomialCoefficientNode::compute(double k, double n);
template float BinomialCoefficientNode::compute(float k, float n);

}
