#include <poincare/dependency.h>
#include <poincare/symbol_abstract.h>

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
  /* If one of the element in the matrix is undefined (unreal), the whole matrix will
   * be reduced to Undefined (Unreal). */
  Expression dependencies = childAtIndex(1);
  if (dependencies.isUndefined()) {
    /* dependencies is either Undefined or Unreal. */
    replaceWithInPlace(dependencies);
    return dependencies;
  }
  assert(dependencies.type() == ExpressionNode::Type::Matrix);
  char variables[k_maxNumberOfVariables * SymbolAbstract::k_maxNameSize];

  int numberOfUselessDependencies = 0;
  int totalNumberOfDependencies = numberOfDependencies();
  for (int i = 0; i < totalNumberOfDependencies; i++) {
    Expression e = dependencies.childAtIndex(i);
    variables[0] = '\0';
    int n = e.getVariables(reductionContext.context(), [](const char * s, Context * c) { return true; }, variables, SymbolAbstract::k_maxNameSize);
    if (n <= 0) {
      /* This dependency contains no symbol or function. If it is undefined,
       * the whole expression is undefined; otherwise we remove it. */
      Expression approximation = e.approximate<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true);
      if (approximation.isUndefined()) {
        replaceWithInPlace(approximation);
        return approximation;
      }
      numberOfUselessDependencies++;
    }
  }

  /* FIXME: Remove all useless dependencies one by one instead of batching. */
  if (childAtIndex(0).isUndefined() || numberOfDependencies() == numberOfUselessDependencies) {
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
  /* If dependencies is not a Matrix, it is either Undef or Unreal. As such,
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

}
