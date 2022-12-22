#include <poincare/dependency.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
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

int DependencyNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  int result = childAtIndex(0)->getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i < result; i++) {
    coefficients[i] = Dependency::Builder(coefficients[i], Expression(childAtIndex(1)).clone().convert<List>());
  }
  return result;
}

int DependencyNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Dependency::s_functionHelper.aliasesList().mainAlias());
}

template<typename T> Evaluation<T> DependencyNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ExpressionNode * dependencies = childAtIndex(Dependency::k_indexOfDependenciesList);
  if (dependencies->type() == Type::Undefined || dependencies->type() == Type::Nonreal) {
    return Complex<T>::Undefined();
  }
  assert(dependencies->type() == ExpressionNode::Type::List);
  int childrenNumber = dependencies->numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    if (dependencies->childAtIndex(i)->approximate(static_cast<T>(1), approximationContext).isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return childAtIndex(0)->approximate(static_cast<T>(1), approximationContext);
}

// Dependency

void Dependency::deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext) {
  assert(numberOfChildren() == 2);

  /* Main expression is reduced with the same reduction target as the parent */
  childAtIndex(0).deepReduce(reductionContext);

  /* List of dependencies is reduced with target SystemForAnalysis */
  ExpressionNode::ReductionContext depContext = reductionContext;
  depContext.setTarget(ExpressionNode::ReductionTarget::SystemForAnalysis);
  childAtIndex(1).deepReduce(depContext);
}

Expression Dependency::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  /* Undefined and dependencies are bubbled-up from list of dependencies.
   * We do this here because we do not want to do this in List::shallowReduce
   * since most of lists do not want to bubble up their undef and dependencies.
   * (because {undef} != undef) */
  SimplificationHelper::defaultShallowReduce(childAtIndex(k_indexOfDependenciesList), &reductionContext);

  Expression e = SimplificationHelper::defaultShallowReduce(*this, &reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }

  Expression dependencies = childAtIndex(k_indexOfDependenciesList);
  assert(dependencies.type() == ExpressionNode::Type::List);
  int totalNumberOfDependencies = numberOfDependencies();
  int i = 0;
  while (i < totalNumberOfDependencies) {
    Expression e = dependencies.childAtIndex(i);
    if (DeepIsSymbolic(e, reductionContext.context(), reductionContext.symbolicComputation())) {
      /* If the dependency involves unresolved symbol/function/sequence,
       * the approximation of the dependency could be undef while the
       * whole expression is not, so the check is skipped.
       * */
      i++;
      continue;
    }

    Expression approximation = e.approximate<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true);
    if (approximation.isUndefined()) {
      return replaceWithUndefinedInPlace();
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

Expression Dependency::extractDependencies(List l) {
  int previousNumberOfChildren = l.numberOfChildren();

  Expression dependencies = childAtIndex(k_indexOfDependenciesList);
  if (dependencies.isUndefined()) {
    l.addChildAtIndexInPlace(dependencies, previousNumberOfChildren, previousNumberOfChildren);
    return *this;
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

  Expression newRef = childAtIndex(0);
  replaceWithInPlace(newRef);
  return newRef;
}

Expression Dependency::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::List) {
    // Second parameter must be a List.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<List>());
}

Expression Dependency::removeUselessDependencies(const ExpressionNode::ReductionContext& reductionContext) {
  // Step 1: Break dependencies into smaller expressions
  Expression dependenciesExpression = childAtIndex(k_indexOfDependenciesList);
  assert(dependenciesExpression.type() == ExpressionNode::Type::List);
  List dependencies = static_cast<List &>(dependenciesExpression);
  for (int i = 0; i < dependencies.numberOfChildren(); i++) {
    Expression depI = dependencies.childAtIndex(i);
    // dep(..,{x*y}) = dep(..,{x+y}) = dep(..,{x ,y})
    if (depI.type() == ExpressionNode::Type::Multiplication || depI.type() == ExpressionNode::Type::Addition) {
      if(depI.numberOfChildren() == 1) {
        depI.replaceWithInPlace(depI.childAtIndex(0));
      } else {
        dependencies.addChildAtIndexInPlace(depI.childAtIndex(0), dependencies.numberOfChildren(), dependencies.numberOfChildren());
        NAryExpression m = static_cast<NAryExpression &>(depI);
        m.removeChildAtIndexInPlace(0);
      }
      i--;
      continue;
    }
    // dep(..,{x^y}) = dep(..,{x}) if y > 0 and y != p/2*q
    if (depI.type() == ExpressionNode::Type::Power) {
      Power p = static_cast<Power &>(depI);
      if (p.typeOfDependency(reductionContext) == Power::DependencyType::None) {
        depI.replaceWithInPlace(depI.childAtIndex(0));
        i--;
        continue;
      }
    }
  }

  // ShallowReduce to remove defined dependencies ({x+3}->{x, 3}->{x})
  Expression e = shallowReduce(reductionContext);
  if (e.type() != ExpressionNode::Type::Dependency) {
    return e;
  }
  Dependency expandedDependency = static_cast<Dependency &>(e);
  Expression tempList = expandedDependency.childAtIndex(k_indexOfDependenciesList);
  List newDependencies = static_cast<List &>(tempList);

  /* Step 2: Remove duplicate dependencies and dependencies contained in others
   * {sqrt(x), sqrt(x), 1/sqrt(x)} -> {1/sqrt(x)} */
  for (int i = 0; i < newDependencies.numberOfChildren(); i++) {
    Expression depI = newDependencies.childAtIndex(i);
    for (int j = 0; j < newDependencies.numberOfChildren(); j++) {
      if (i == j) {
        continue;
      }
      if (newDependencies.childAtIndex(j).containsSameDependency(depI, reductionContext)) {
        newDependencies.removeChildAtIndexInPlace(i);
        i--;
        break;
      }
    }
  }

  Expression tempList2 = expandedDependency.childAtIndex(k_indexOfDependenciesList);
  newDependencies = static_cast<List &>(tempList2);
  /* Step 3: Remove dependencies already contained in main expression.
   * dep(x^2+1,{x}) -> x^2+1 */
  for (int i = 0; i < newDependencies.numberOfChildren(); i++) {
    Expression depI = newDependencies.childAtIndex(i);
    if (expandedDependency.childAtIndex(0).containsSameDependency(depI, reductionContext)) {
      newDependencies.removeChildAtIndexInPlace(i);
      i--;
    }
  }

  return expandedDependency.shallowReduce(reductionContext);
}

}
