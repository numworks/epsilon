#include <ion.h>
#include <poincare/based_integer.h>
#include <poincare/complex.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/randint.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression RandintNode::createExpressionWithTwoChildren() const {
  if (numberOfChildren() == 1) {
    return Randint::Builder(BasedInteger::Builder(k_defaultMinBound),
                            Expression(childAtIndex(0)).clone());
  }
  assert(numberOfChildren() == 2);
  return Randint(this);
}

Layout RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits,
                                 Context* context) const {
  return LayoutHelper::Prefix(
      createExpressionWithTwoChildren(), floatDisplayMode,
      numberOfSignificantDigits,
      Randint::s_functionHelper.aliasesList().mainAlias(), context);
}

int RandintNode::serialize(char* buffer, int bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      createExpressionWithTwoChildren().node(), buffer, bufferSize,
      floatDisplayMode, numberOfSignificantDigits,
      Randint::s_functionHelper.aliasesList().mainAlias());
}

Expression RandintNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> RandintNode::templateApproximate(
    const ApproximationContext& approximationContext,
    bool* inputIsUndefined) const {
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
         Preferences::AngleUnit angleUnit, void* ctx) {
        T a;
        T b;
        if (numberOfComplexes == 1) {
          a = static_cast<T>(k_defaultMinBound);
          b = ComplexNode<T>::ToScalar(c[0]);
        } else {
          assert(numberOfComplexes == 2);
          a = ComplexNode<T>::ToScalar(c[0]);
          b = ComplexNode<T>::ToScalar(c[1]);
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
          return Complex<T>::RealUndefined();
        }
        Integer integerResult =
            Integer::RandomInt(static_cast<double_native_int_t>(a),
                               static_cast<double_native_int_t>(b));
        if (integerResult.isOverflow()) {
          return Complex<T>::RealUndefined();
        }
        return Complex<T>::Builder(integerResult.approximate<T>());
      });
}

Expression Randint::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
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
  // TODO: handle sum(randint(0,2)) and {randint(0,23)}_k<=10
#if 0
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);

  if (c0.type() != ExpressionNode::Type::Rational ||
      c1.type() != ExpressionNode::Type::Rational) {
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
