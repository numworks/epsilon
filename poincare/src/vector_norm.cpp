#include <poincare/vector_norm.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

constexpr Expression::FunctionHelper VectorNorm::s_functionHelper;

int VectorNormNode::numberOfChildren() const { return VectorNorm::s_functionHelper.numberOfChildren(); }

Expression VectorNormNode::shallowReduce(ReductionContext reductionContext) {
  return VectorNorm(this).shallowReduce(reductionContext);
}

Layout VectorNormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(VectorNorm(this), floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.name());
}

int VectorNormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.name());
}

template<typename T>
Evaluation<T> VectorNormNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  return Complex<T>::Builder(input.norm());
}


Expression VectorNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild = static_cast<Matrix&>(c);
    if (matrixChild.numberOfColumns() != 1) {
      // Norm is only defined on column matrices
      return replaceWithUndefinedInPlace();
    }
    Expression a = matrixChild.norm(reductionContext);
    replaceWithInPlace(a);
    return a.shallowReduce(reductionContext);
  }
  if (c.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  return replaceWithUndefinedInPlace();
}

}
