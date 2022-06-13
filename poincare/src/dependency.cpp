#include <poincare/dependency.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol_abstract.h>
#include <poincare/symbol.h>

namespace Poincare {

constexpr Expression::FunctionHelper Dependency::s_functionHelper;

// DependencyNode

Expression DependencyNode::shallowReduce(const ReductionContext& reductionContext) {
  return Dependency(this).shallowReduce(reductionContext);
}

bool DependencyNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  assert(false); return false;
}

int DependencyNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Dependency::s_functionHelper.name());
}

template<typename T> Evaluation<T> DependencyNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ExpressionNode * dependencies = childAtIndex(Dependency::k_indexOfDependenciesList);
  if (dependencies->type() == Type::Undefined || dependencies->type() == Type::Nonreal) {
    return Complex<T>::Undefined();
  }
  assert(dependencies->type() == ExpressionNode::Type::List);
  for (int i = 0; i < dependencies->numberOfChildren(); i++) {
    if (dependencies->childAtIndex(i)->approximate(static_cast<T>(1), approximationContext).isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return childAtIndex(0)->approximate(static_cast<T>(1), approximationContext);
}

// Dependency

Expression Dependency::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  // Undefined/Unreal nodes are bubbled-up
  SimplificationHelper::shallowReduceUndefined(childAtIndex(k_indexOfDependenciesList));
  Expression dependencies = childAtIndex(k_indexOfDependenciesList);
  if (dependencies.isUndefined()) {
    /* dependencies is either Undefined or Nonreal. */
    replaceWithInPlace(dependencies);
    return dependencies;
  }
  assert(dependencies.type() == ExpressionNode::Type::List);

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
      static_cast<List &>(dependencies).removeChildAtIndexInPlace(i);
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
  Expression dependencies = childAtIndex(k_indexOfDependenciesList);
  if (dependencies.type() == ExpressionNode::Type::List) {
    List listChild = static_cast<List &>(dependencies);
    listChild.addChildAtIndexInPlace(newDependency.clone(), numberOfDependencies(), numberOfDependencies());
  }
  /* If dependencies is not a List, it is either Undef or Nonreal. As such,
   * the expression will be undefined: new dependencies will not change that,
   * and will disappear in the next reduction. */
}

void Dependency::extractDependencies(List l) {
  int previousNumberOfChildren = l.numberOfChildren();

  Expression dependencies = childAtIndex(k_indexOfDependenciesList);
  if (dependencies.isUndefined()) {
    l.addChildAtIndexInPlace(dependencies, previousNumberOfChildren, previousNumberOfChildren);
    return;
  }

  assert(dependencies.type() == ExpressionNode::Type::List);
  List listChild = static_cast<List &>(dependencies);
  int newNumberOfChildren = previousNumberOfChildren;
  int numberOfChildrenToDump = listChild.numberOfChildren();
  for (int i = 0; i < numberOfChildrenToDump; i++) {
    Expression child = listChild.childAtIndex(i);
    bool unique = true;
    int j = 0;
    while (j < previousNumberOfChildren && unique) {
      unique = !child.isIdenticalTo(l.childAtIndex(j++));
    }
    if (unique) {
      /* As matrixChild will be destroyed afterwards, we steal the child,
       * leaving a Ghost in its place. */
      l.addChildAtIndexInPlace(child, newNumberOfChildren, newNumberOfChildren);
      newNumberOfChildren++;
    }
  }

  replaceWithInPlace(childAtIndex(0));
}

Expression Dependency::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::List) {
    // Second parameter must be a List.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<List>());
}

}
