#include <poincare/dependency.h>
#include <poincare/undefined.h>

namespace Poincare {

// DependencyNode

Expression DependencyNode::shallowReduce(ReductionContext reductionContext) {
  return Dependency(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> DependencyNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * dependencies = childAtIndex(1);
  if (dependencies->type() == Type::Undefined || dependencies->type() == Type::Unreal) {
    return Complex<T>::Undefined();
  }
  assert(dependencies->type() == ExpressionNode::Type::Matrix);
  for (int i = 0; i < dependencies->numberOfChildren(); i++) {
    if (dependencies->childAtIndex(i)->approximate(static_cast<T>(1), approximationContext).isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return childAtIndex(0)->approximate(static_cast<T>(1), approximationContext);
}

// Dependency

Expression Dependency::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  /* If one of the element in the matrix is undefined, the whole matrix will
   * be reduced to Undefined. */
  Expression dependencies = childAtIndex(1);
  if (dependencies.isUndefined()) {
    replaceWithInPlace(dependencies);
    return dependencies;
  }
  assert(dependencies.type() == ExpressionNode::Type::Matrix);
  if (reductionContext.target() == ExpressionNode::ReductionTarget::SystemForAnalysis
   || !dependencies.hasExpression(
        [](const Expression e, const void * context) {
        return e.type() == ExpressionNode::Type::Symbol || e.type() == ExpressionNode::Type::Function;
        }, reductionContext.context()))
  {
    /* The dependencies do not depend on any variables and are not undef, we
     * can remove them */
    Expression trueExpression = childAtIndex(0);
    replaceWithInPlace(trueExpression);
    return trueExpression;
  }
  return *this;
}

void Dependency::addDependency(Expression newDependency) {
  Expression dependencies = childAtIndex(1);
  if (dependencies.type() == ExpressionNode::Type::Matrix) {
    Matrix matrixChild = static_cast<Matrix &>(dependencies);
    matrixChild.addChildAtIndexInPlace(newDependency.clone(), numberOfDependencies(), numberOfDependencies());
  }
}

}
