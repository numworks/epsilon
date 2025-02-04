#include <ion.h>
#include <poincare/layout.h>
#include <poincare/old/based_integer.h>
#include <poincare/old/complex.h>
#include <poincare/old/float.h>
#include <poincare/old/infinity.h>
#include <poincare/old/integer.h>
#include <poincare/old/randint.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

OExpression RandintNode::createExpressionWithTwoChildren() const {
  if (numberOfChildren() == 1) {
    return Randint::Builder(BasedInteger::Builder(k_defaultMinBound),
                            OExpression(childAtIndex(0)).clone());
  }
  assert(numberOfChildren() == 2);
  return Randint(this);
}

size_t RandintNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      createExpressionWithTwoChildren().node(), buffer, bufferSize,
      floatDisplayMode, numberOfSignificantDigits,
      Randint::s_functionHelper.aliasesList().mainAlias());
}

OExpression RandintNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> RandintNode::templateApproximate(
    const ApproximationContext& approximationContext,
    bool* inputIsUndefined) const {
  if (approximationContext.withinReduce()) {
    // Return NAN to prevent the reduction from assuming anything at this point
    return Complex<T>::Undefined();
  }
  Evaluation<T> aInput;
  Evaluation<T> bInput;
  if (numberOfChildren() == 1) {
    aInput = Complex<T>::Builder(static_cast<T>(k_defaultMinBound));
    bInput = childAtIndex(0)->approximate(T(), approximationContext);
  } else {
    assert(numberOfChildren() == 2);
    aInput = childAtIndex(0)->approximate(T(), approximationContext);
    bInput = childAtIndex(1)->approximate(T(), approximationContext);
  }

  if (inputIsUndefined) {
    *inputIsUndefined = aInput.isUndefined() || bInput.isUndefined();
  }
  return ApproximationHelper::Map<T>(
      this, approximationContext,
      [](const std::complex<T>* c, int numberOfComplexes,
         Preferences::ComplexFormat complexFormat,
         Preferences::AngleUnit angleUnit, void* ctx) -> std::complex<T> {
        T a;
        T b;
        if (numberOfComplexes == 1) {
          a = static_cast<T>(k_defaultMinBound);
          b = ComplexNode<T>::ToRealcalar(c[0]);
        } else {
          assert(numberOfComplexes == 2);
          a = ComplexNode<T>::ToRealScalar(c[0]);
          b = ComplexNode<T>::ToRealScalar(c[1]);
        }
        /* randint is undefined if:
         * - one of the bounds is NAN or INF
         * - one of the input cannot be represented by an integer
         *   (here we don't test a != std::round(a) because we want the inputs
         *   to hold all digits so to be representable as an int)
         *  */
        if (std::isnan(a) || std::isnan(b) || std::isinf(a) || std::isinf(b) ||
            a != static_cast<double_native_int_t>(a) ||
            b != static_cast<double_native_int_t>(b)) {
          return complexRealNAN<T>();
        }
        Integer integerResult =
            Integer::RandomInt(static_cast<double_native_int_t>(a),
                               static_cast<double_native_int_t>(b));
        if (integerResult.isOverflow()) {
          return complexRealNAN<T>();
        }
        return integerResult.approximate<T>();
      });
}

OExpression Randint::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
  // TODO: handle sum(randint(0,2),k,0,10) and sequence(randint(0,23),k,0,10)
#if 0
  OExpression c0 = childAtIndex(0);
  OExpression c1 = childAtIndex(1);

  if (c0.otype() != ExpressionNode::Type::Rational ||
      c1.otype() != ExpressionNode::Type::Rational) {
    return *this;
  }

  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  if (!r0.isInteger() || !r1.isInteger()) {
    // Randint is only defined on integers.
    return replaceWithUndefinedInPlace();
  }

  if (reductionContext.target() == ReductionTarget::SystemForApproximation) {
    /* The reduction is used for multiple approximations. Randint has to be
     * recomputed at every approximation. */
    return *this;
  }

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  Integer randint = Integer::RandomInt(a, b);
  if (randint.isOverflow()) {
    return replaceWithUndefinedInPlace();
  }
  Rational result = Rational::Builder(randint);
  replaceWithInPlace(result);
  return result;
#endif
}

}  // namespace Poincare
