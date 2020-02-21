#include <poincare/randint.h>
#include <poincare/complex.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/random.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/serialization_helper.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Randint::s_functionHelper;

int RandintNode::numberOfChildren() const { return Randint::s_functionHelper.numberOfChildren(); }

Layout RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Randint(this), floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.name());
}

int RandintNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Randint::s_functionHelper.name());
}

template <typename T> Evaluation<T> RandintNode::templateApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool * inputIsUndefined) const {
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  if (inputIsUndefined) {
    *inputIsUndefined = aInput.isUndefined() || bInput.isUndefined();
  }
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  /* randint is undefined if:
   * - one of the bounds is NAN or INF
   * - the last bound is lesser than the first one
   * - one of the input cannot be represented by an integer
   *   (here we don't test a != std::round(a) because we want the inputs to
   *   hold all digits so to be representable as an int)
   * - the range between bounds is too large to be covered by all double between
   *   0 and 1 - we can't map the integers of the range with all representable
   *   double numbers from 0 to 1.
   *  */
  if (std::isnan(a) || std::isnan(b) || std::isinf(a) || std::isinf(b)
      || a > b
      || a != (int)a || b != (int)b
      || (Expression::Epsilon<T>()*(b+(T)1.0-a) > (T)1.0)) {
    return Complex<T>::RealUndefined();
  }
  T result = std::floor(Random::random<T>()*(b+(T)1.0-a)+a);
  return Complex<T>::Builder(result);
}

Expression RandintNode::shallowReduce(ReductionContext reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

Expression Randint::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = Expression::defaultShallowReduce();
  e = e.defaultHandleUnitsInChildren();
  if (e.isUndefined()) {
    return e;
  }
  bool inputIsUndefined = false;
  double eval = static_cast<RandintNode *>(node())->templateApproximate<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), &inputIsUndefined).toScalar();
  if (inputIsUndefined) {
    /* The input might be NAN because we are reducing a function's expression
     * which depends on x. We thus do not want to replace too early with
     * undefined. */
    return *this;
  }
  Expression result = Number::DecimalNumber(eval);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

}
