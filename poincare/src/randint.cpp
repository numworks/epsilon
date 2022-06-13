#include <poincare/randint.h>
#include <poincare/complex.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/random.h>
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

int RandintNode::numberOfChildren() const { return Randint::s_functionHelper.numberOfChildren(); }

Layout RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Randint(this), floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.name());
}

int RandintNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.name());
}

Expression RandintNode::shallowReduce(const ReductionContext& reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

template <typename T> Evaluation<T> RandintNode::templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined) const {
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), approximationContext);
  if (inputIsUndefined) {
    *inputIsUndefined = aInput.isUndefined() || bInput.isUndefined();
  }
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 2);
        T a = ComplexNode<T>::ToScalar(c[0]);
        T b = ComplexNode<T>::ToScalar(c[1]);
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
        T result = std::floor(Random::random<T>()*(b+static_cast<T>(1.0)-a)+a);
        return Complex<T>::Builder(result);
      });
}

Expression Randint::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
