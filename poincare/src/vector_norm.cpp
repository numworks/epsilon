#include <apps/exam_mode_configuration.h>
#include <apps/global_preferences.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <poincare/vector_norm.h>
#include <poincare/vector_norm_layout.h>

namespace Poincare {

constexpr Expression::FunctionHelper VectorNorm::s_functionHelper;

int VectorNormNode::numberOfChildren() const { return VectorNorm::s_functionHelper.numberOfChildren(); }

Expression VectorNormNode::shallowReduce(ReductionContext reductionContext) {
  return VectorNorm(this).shallowReduce(reductionContext);
}

Layout VectorNormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return VectorNormLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int VectorNormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.name());
}

template<typename T>
Evaluation<T> VectorNormNode::templatedApproximate(ApproximationContext approximationContext) const {
  if (ExamModeConfiguration::vectorsAreForbidden(GlobalPreferences::sharedGlobalPreferences()->examMode())) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  return Complex<T>::Builder(input.norm());
}


Expression VectorNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (ExamModeConfiguration::vectorsAreForbidden(GlobalPreferences::sharedGlobalPreferences()->examMode())) {
    return replaceWithUndefinedInPlace();
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild = static_cast<Matrix&>(c);
    // Norm is only defined on vectors only
    if (matrixChild.vectorType() == Array::VectorType::None) {
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
