#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <poincare/vector_norm.h>
#include <poincare/vector_norm_layout.h>

namespace Poincare {

int VectorNormNode::numberOfChildren() const { return VectorNorm::s_functionHelper.numberOfChildren(); }

Expression VectorNormNode::shallowReduce(const ReductionContext& reductionContext) {
  return VectorNorm(this).shallowReduce(reductionContext);
}

Layout VectorNormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return VectorNormLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int VectorNormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, VectorNorm::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> VectorNormNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  if (Poincare::Preferences::sharedPreferences()->vectorNormIsForbidden()) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  if (input.type() != EvaluationNode<T>::Type::MatrixComplex) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(static_cast<MatrixComplex<T>&>(input).norm());
}

Expression VectorNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (Poincare::Preferences::sharedPreferences()->vectorNormIsForbidden()) {
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
