#include <poincare/randint.h>
#include <poincare/complex.h>
#include <poincare/random.h>
#include <ion.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int RandintNode::numberOfChildren() const { return Randint::FunctionHelper()->numberOfChildren(); }

Layout RandintNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Randint(this), floatDisplayMode, numberOfSignificantDigits, Randint::FunctionHelper()->name());
}

int RandintNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Randint::FunctionHelper()->name());
}

template <typename T> Evaluation<T> RandintNode::templateApproximate(Context & context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  if (std::isnan(a) || std::isnan(b) || a != std::round(a) || b != std::round(b) || a > b) {
    return Complex<T>::Undefined();

  }
  T result = std::floor(Random::random<T>()*(b+1.0-a)+a);
  return Complex<T>(result);
}

constexpr Expression::FunctionHelper Randint::m_functionHelper = Expression::FunctionHelper("randint", 2, &Randint::UntypedBuilder);

}
