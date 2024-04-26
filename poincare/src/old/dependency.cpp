#include <poincare/old/dependency.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/symbol_abstract.h>

namespace Poincare {

constexpr OExpression::FunctionHelper Dependency::s_functionHelper;

// DependencyNode

OExpression DependencyNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return Dependency(this).shallowReduce(reductionContext);
}

bool DependencyNode::derivate(const ReductionContext &reductionContext,
                              Symbol symbol, OExpression symbolValue) {
  assert(false);
  return false;
}

int DependencyNode::getPolynomialCoefficients(
    Context *context, const char *symbolName,
    OExpression coefficients[]) const {
  int result = mainExpression()->getPolynomialCoefficients(context, symbolName,
                                                           coefficients);
  for (int i = 0; i < result; i++) {
    coefficients[i] = Dependency::Builder(
        coefficients[i],
        OExpression(dependenciesList()).clone().convert<OList>());
  }
  return result;
}

size_t DependencyNode::serialize(char *buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Dependency::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> DependencyNode::templatedApproximate(
    const ApproximationContext &approximationContext) const {
  ExpressionNode *dependencies =
      childAtIndex(Dependency::k_indexOfDependenciesList);
  if (dependencies->isUndefined()) {
    return Complex<T>::Undefined();
  }
  assert(dependencies->otype() == ExpressionNode::Type::OList);
  int childrenNumber = dependencies->numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    if (dependencies->childAtIndex(i)
            ->approximate(static_cast<T>(1), approximationContext)
            .isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return mainExpression()->approximate(static_cast<T>(1), approximationContext);
}

ExpressionNode *DependencyNode::mainExpression() const {
  return childAtIndex(Dependency::k_indexOfMainExpression);
}
ExpressionNode *DependencyNode::dependenciesList() const {
  return childAtIndex(Dependency::k_indexOfDependenciesList);
}

// Dependency

void Dependency::deepReduceChildren(const ReductionContext &reductionContext) {
  assert(numberOfChildren() == 2);

  /* Main expression is reduced with the same reduction target as the parent */
  mainExpression().deepReduce(reductionContext);

  /* OList of dependencies is reduced with target SystemForAnalysis */
  ReductionContext depContext = reductionContext;
  depContext.setTarget(ReductionTarget::SystemForAnalysis);
  dependenciesList().deepReduce(depContext);
}

OExpression Dependency::shallowReduce(ReductionContext reductionContext) {
  OExpression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::DefinedOnBooleans,
      SimplificationHelper::UnitReduction::KeepUnits,
      SimplificationHelper::MatrixReduction::DefinedOnMatrix,
      SimplificationHelper::ListReduction::DoNotDistributeOverLists,
      SimplificationHelper::PointReduction::DefinedOnPoint);
  if (!e.isUninitialized()) {
    return e;
  }

  OExpression dependencies = dependenciesList();
  assert(dependencies.otype() == ExpressionNode::Type::OList);
  int totalNumberOfDependencies = numberOfDependencies();
  int i = 0;
  while (i < totalNumberOfDependencies) {
    OExpression e = dependencies.childAtIndex(i);
    OExpression approximation;
    bool hasSymbolsOrRandom =
        e.deepIsSymbolic(reductionContext.context(),
                         reductionContext.symbolicComputation()) ||
        e.recursivelyMatches(OExpression::IsRandom, reductionContext.context());
    if (hasSymbolsOrRandom) {
      /* If the dependency involves unresolved symbol/function/sequence,
       * the approximation of the dependency could be undef while the
       * whole expression is not. We juste approximate everything but the symbol
       * in case the other parts of the expression make it undef/nonreal.
       * */
      approximation = e.clone().deepApproximateKeepingSymbols(reductionContext);
    } else {
      approximation =
          e.approximate<double>(ApproximationContext(reductionContext, true));
    }
    if (approximation.isUndefined()) {
      return replaceWithUndefinedInPlace();
    } else if (!hasSymbolsOrRandom) {
      static_cast<OList &>(dependencies).removeChildAtIndexInPlace(i);
      totalNumberOfDependencies--;
    } else {
      i++;
    }
  }

  if (mainExpression().isUndefined() || totalNumberOfDependencies == 0) {
    OExpression trueExpression = mainExpression();
    replaceWithInPlace(trueExpression);
    return trueExpression;
  }

  return *this;
}

void Dependency::addDependency(OExpression newDependency) {
  OExpression dependencies = dependenciesList();
  if (dependencies.otype() == ExpressionNode::Type::OList) {
    OList listChild = static_cast<OList &>(dependencies);
    listChild.addChildAtIndexInPlace(
        newDependency.clone(), numberOfDependencies(), numberOfDependencies());
  }
  /* If dependencies is not a OList, it is either Undef or Nonreal. As such,
   * the expression will be undefined: new dependencies will not change that,
   * and will disappear in the next reduction. */
}

OExpression Dependency::extractDependencies(OList l) {
  int previousNumberOfChildren = l.numberOfChildren();

  OExpression dependencies = dependenciesList();
  if (dependencies.isUndefined()) {
    l.addChildAtIndexInPlace(dependencies, previousNumberOfChildren,
                             previousNumberOfChildren);
    return *this;
  }

  assert(dependencies.otype() == ExpressionNode::Type::OList);
  OList listChild = static_cast<OList &>(dependencies);
  int newNumberOfChildren = previousNumberOfChildren;
  int numberOfChildrenToDump = listChild.numberOfChildren();
  for (int i = 0; i < numberOfChildrenToDump; i++) {
    OExpression child = listChild.childAtIndex(i);
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

  OExpression newRef = mainExpression();
  replaceWithInPlace(newRef);
  return newRef;
}

OExpression Dependency::UntypedBuilder(OExpression children) {
  assert(children.otype() == ExpressionNode::Type::OList);
  if (children.childAtIndex(1).otype() != ExpressionNode::Type::OList) {
    // Second parameter must be a OList.
    return OExpression();
  }
  return Builder(children.childAtIndex(0),
                 children.childAtIndex(1).convert<OList>());
}

OExpression Dependency::removeUselessDependencies(
    const ReductionContext &reductionContext) {
  // Step 1: Break dependencies into smaller expressions
  OExpression dependenciesExpression = dependenciesList();
  assert(dependenciesExpression.otype() == ExpressionNode::Type::OList);
  OList dependencies = static_cast<OList &>(dependenciesExpression);
  for (int i = 0; i < dependencies.numberOfChildren(); i++) {
    OExpression depI = dependencies.childAtIndex(i);
    // dep(..,{x*y}) = dep(..,{x+y}) = dep(..,{x ,y})
    if (depI.isOfType({ExpressionNode::Type::Multiplication,
                       ExpressionNode::Type::Addition})) {
      if (depI.numberOfChildren() == 1) {
        depI.replaceWithInPlace(depI.childAtIndex(0));
      } else {
        dependencies.addChildAtIndexInPlace(depI.childAtIndex(0),
                                            dependencies.numberOfChildren(),
                                            dependencies.numberOfChildren());
        NAryExpression m = static_cast<NAryExpression &>(depI);
        m.removeChildAtIndexInPlace(0);
      }
      i--;
      continue;
    }
    // dep(..,{x^y}) = dep(..,{x}) if y > 0 and y != p/2*q
    if (depI.otype() == ExpressionNode::Type::Power) {
      Power p = static_cast<Power &>(depI);
      if (p.typeOfDependency(reductionContext) == Power::DependencyType::None) {
        depI.replaceWithInPlace(depI.childAtIndex(0));
        i--;
        continue;
      }
    }
  }

  // ShallowReduce to remove defined dependencies ({x+3}->{x, 3}->{x})
  OExpression e = shallowReduce(reductionContext);
  if (e.otype() != ExpressionNode::Type::Dependency) {
    return e;
  }
  Dependency expandedDependency = static_cast<Dependency &>(e);
  OExpression tempList = expandedDependency.dependenciesList();
  OList newDependencies = static_cast<OList &>(tempList);

  /* Step 2: Remove duplicate dependencies and dependencies contained in others
   * {sqrt(x), sqrt(x), 1/sqrt(x)} -> {1/sqrt(x)} */
  for (int i = 0; i < newDependencies.numberOfChildren(); i++) {
    OExpression depI = newDependencies.childAtIndex(i);
    for (int j = 0; j < newDependencies.numberOfChildren(); j++) {
      if (i == j) {
        continue;
      }
      if (newDependencies.childAtIndex(j).containsSameDependency(
              depI, reductionContext)) {
        newDependencies.removeChildAtIndexInPlace(i);
        i--;
        break;
      }
    }
  }

  OExpression tempList2 = expandedDependency.dependenciesList();
  newDependencies = static_cast<OList &>(tempList2);
  /* Step 3: Remove dependencies already contained in main expression.
   * dep(x^2+1,{x}) -> x^2+1 */
  for (int i = 0; i < newDependencies.numberOfChildren(); i++) {
    OExpression depI = newDependencies.childAtIndex(i);
    if (expandedDependency.mainExpression().containsSameDependency(
            depI, reductionContext)) {
      newDependencies.removeChildAtIndexInPlace(i);
      i--;
    }
  }

  return expandedDependency.shallowReduce(reductionContext);
}

}  // namespace Poincare
