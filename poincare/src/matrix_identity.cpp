#include <poincare/matrix_identity.h>
#include <poincare/matrix_complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>
#include <limits.h>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixIdentity::s_functionHelper;

int MatrixIdentityNode::numberOfChildren() const { return MatrixIdentity::s_functionHelper.numberOfChildren(); }

Expression MatrixIdentityNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return MatrixIdentity(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Layout MatrixIdentityNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixIdentity(this), floatDisplayMode, numberOfSignificantDigits, MatrixIdentity::s_functionHelper.name());
}

int MatrixIdentityNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixIdentity::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixIdentityNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  T r = input.toScalar(); // Undefined if the child is not real
  if (!std::isnan(r) && !std::isinf(r) && r > 0 // The child is defined and positive
      && std::ceil(r) == std::floor(r) // The child is an integer
      &&  r < ((float) INT_MAX)) // The child is not too big
  {
    return MatrixComplex<T>::CreateIdentity((int)r);
  }
  return Complex<T>::Undefined();
}


Expression MatrixIdentity::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
    return *this;
  }
}

}
