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
  // Check if the child is an integer
  if (input.type() == EvaluationNode<T>::Type::Complex) {
    // The child is a complex
    std::complex<T> c = static_cast<Complex<T>&>(input).stdComplex();
    T im = c.imag();
    T re = c.real();
    if ((im == 0.0 || std::isnan(im)) // The child is a real
        && (!std::isnan(re) && re > 0) // The child is positive
        && (std::ceil(re) ==  std::floor(re))) // The child is an integer
    {
      return MatrixComplex<T>::CreateIdentity((int)re);
    }
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
