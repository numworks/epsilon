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

template <typename T> Evaluation<T> RandintNode::templateApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  if (std::isnan(a) || std::isnan(b) || a != std::round(a) || b != std::round(b) || a > b || std::isinf(a) || std::isinf(b)) {
    return Complex<T>::Undefined();

  }
  T result = std::floor(Random::random<T>()*(b+1.0-a)+a);
  return Complex<T>::Builder(result);
}

Expression RandintNode::shallowReduce(ReductionContext reductionContext) {
  return Randint(this).shallowReduce(reductionContext);
}

Expression Randint::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = Expression::defaultShallowReduce();
  if (e.isUndefined()) {
    return e;
  }
  float eval = approximateToScalar<float>(reductionContext.context() , reductionContext.complexFormat() , reductionContext.angleUnit() );
  Expression result;
  if (std::isnan(eval)) {
    result = Undefined::Builder();
  } else if (std::isinf(eval)) {
    result = Infinity::Builder(eval < 0);
  } else {
    result = Rational::Builder(Integer((int)eval));
  }
  replaceWithInPlace(result);
  return result;
}

}
