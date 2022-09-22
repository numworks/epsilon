#include <poincare/randint.h>
#include <poincare/based_integer.h>
#include <poincare/complex.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression RandintNode::createExpressionWithTwoChildren() const {
  if (numberOfChildren() == 1) {
    return Randint::Builder(BasedInteger::Builder(k_defaultMinBound), Expression(childAtIndex(0)).clone());
  }
  assert(numberOfChildren() == 2);
  return Randint(this);
}

Layout RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(createExpressionWithTwoChildren(), floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.aliasesList().mainAlias(), context);
}

int RandintNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(createExpressionWithTwoChildren().node(), buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.aliasesList().mainAlias());
}

Expression RandintNode::shallowReduce(const ReductionContext& reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

template <typename T> Evaluation<T> RandintNode::templateApproximate(const ApproximationContext& approximationContext, bool * inputIsUndefined) const {
  Evaluation<T> aInput; Evaluation<T> bInput;
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
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        T a; T b;
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
         * - the last bound is lesser than the first one
         * - one of the input cannot be represented by an integer
         *   (here we don't test a != std::round(a) because we want the inputs
         *   to hold all digits so to be representable as an int)
         * - the range between bounds is too large to be covered by all double
         *   between 0 and 1 - we can't map the integers of the range with all
         *   representable double numbers from 0 to 1.
         *  */
        if (std::isnan(a) || std::isnan(b) || std::isinf(a) || std::isinf(b)
            || a > b
            || a != (int)a || b != (int)b
            || (Float<T>::EpsilonLax()*(b+static_cast<T>(1.0)-a) > static_cast<T>(1.0))) {
          return Complex<T>::RealUndefined();
        }
        return Complex<T>::Builder(Randint::RandomInt(a, b));
      });
}

Expression Randint::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
  return *this;
}

}
