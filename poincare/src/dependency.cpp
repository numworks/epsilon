#include <poincare/dependency.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

constexpr Expression::FunctionHelper Dependency::s_functionHelper;

// DependencyNode

Expression DependencyNode::shallowReduce(ReductionContext reductionContext) {
  return Dependency(this).shallowReduce(reductionContext);
}

int DependencyNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Dependency::s_functionHelper.name());
}

template<typename T> Evaluation<T> DependencyNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * dependencies = childAtIndex(1);
  if (dependencies->type() == Type::Undefined || dependencies->type() == Type::Nonreal) {
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
  /* If one of the element in the matrix is undefined (nonreal), the whole
   * matrix will be reduced to Undefined (Nonreal). */
  Expression dependencies = childAtIndex(1);
  if (dependencies.isUndefined()) {
    /* dependencies is either Undefined or Nonreal. */
    replaceWithInPlace(dependencies);
    return dependencies;
  }
  assert(dependencies.type() == ExpressionNode::Type::Matrix);

  int totalNumberOfDependencies = numberOfDependencies();
  int i = 0;
  while (i < totalNumberOfDependencies) {
    Expression e = dependencies.childAtIndex(i);
    Expression approximation = e.approximate<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true);
    if (approximation.isUndefined()) {
      /* We can't decide now that the main expression is undefined because its
       * depencencies are. Indeed, the dependency might involve unresolved
       * symbol/function/sequence (especially sequences that are not
       * approximated while in reducing routine). */
      i++;
    } else {
      static_cast<Matrix &>(dependencies).removeChildAtIndexInPlace(i);
      totalNumberOfDependencies--;
    }
  }

  if (childAtIndex(0).isUndefined() || totalNumberOfDependencies == 0) {
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
  /* If dependencies is not a Matrix, it is either Undef or Nonreal. As such,
   * the expression will be undefined: new dependencies will not change that,
   * and will disappear in the next reduction. */
}

void Dependency::extractDependencies(Matrix m) {
  assert(m.numberOfChildren() == 0 || m.numberOfRows() == 1);

  int previousNumberOfChildren = m.numberOfChildren();

  Expression dependencies = childAtIndex(1);
  if (dependencies.isUndefined()) {
    m.addChildAtIndexInPlace(dependencies, previousNumberOfChildren, previousNumberOfChildren);
    return;
  }

  assert(dependencies.type() == ExpressionNode::Type::Matrix);
  Matrix matrixChild = static_cast<Matrix &>(dependencies);
  assert (matrixChild.numberOfRows() == 1);
  int newNumberOfChildren = previousNumberOfChildren;
  int numberOfChildrenToDump = matrixChild.numberOfChildren();
  for (int i = 0; i < numberOfChildrenToDump; i++) {
    Expression child = matrixChild.childAtIndex(i);
    bool unique = true;
    int j = 0;
    while (j < previousNumberOfChildren && unique) {
      unique = !child.isIdenticalTo(m.childAtIndex(j++));
    }
    if (unique) {
      /* As matrixChild will be destroyed afterwards, we steal the child,
       * leaving a Ghost in its place. */
      m.addChildAtIndexInPlace(child, newNumberOfChildren, newNumberOfChildren);
      newNumberOfChildren++;
    }
  }

  replaceWithInPlace(childAtIndex(0));
}

Expression Dependency::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::Matrix);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Matrix) {
    // Second parameter must be a Matrix.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Matrix>());
}

}
