#include <assert.h>
#include <poincare/boolean.h>
#include <poincare/dependency.h>
#include <poincare/expression.h>
#include <poincare/expression_node.h>
#include <poincare/integer.h>
#include <poincare/list.h>
#include <poincare/multiplication.h>
#include <poincare/nonreal.h>
#include <poincare/parametered_expression.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

namespace Poincare {

void SimplificationHelper::deepBeautifyChildren(
    OExpression e, const ReductionContext& reductionContext) {
  const int nbChildren = e.numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    OExpression child = e.childAtIndex(i);
    child = child.deepBeautify(reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (e.node()->childAtIndexNeedsUserParentheses(child, i)) {
      e.replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

void SimplificationHelper::defaultDeepReduceChildren(
    OExpression e, const ReductionContext& reductionContext) {
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    assert(childrenCount == e.numberOfChildren());
    e.childAtIndex(i).deepReduce(reductionContext);
  }
}

OExpression SimplificationHelper::defaultShallowReduce(
    OExpression e, ReductionContext* reductionContext,
    BooleanReduction booleanParameter, UnitReduction unitParameter,
    MatrixReduction matrixParameter, ListReduction listParameter,
    PointReduction pointParameter, UndefReduction undefParameter,
    DependencyReduction dependencyParameter) {
  OExpression res;
  // Step 1: Shallow reduce undefined
  if (undefParameter == UndefReduction::BubbleUpUndef) {
    res = shallowReduceUndefined(e);
  }
  if (!res.isUninitialized()) {
    return res;
  }
  // Step 2: Bubble up dependencies
  if (dependencyParameter == DependencyReduction::BubbleUp) {
    res = bubbleUpDependencies(e, *reductionContext);
  }
  if (!res.isUninitialized()) {
    return res;
  }
  // Step 3: Handle units
  if (unitParameter == UnitReduction::BanUnits) {
    res = shallowReduceBanningUnits(e);
  } else if (unitParameter == UnitReduction::ExtractUnitsOfFirstChild) {
    res = shallowReduceKeepingUnitsFromFirstChild(e, *reductionContext);
  }
  if (!res.isUninitialized()) {
    return res;
  }
  // Step 4: Handle matrices
  if (matrixParameter == MatrixReduction::UndefinedOnMatrix) {
    res = undefinedOnMatrix(e, reductionContext);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  // Step 5: Handle lists
  if (listParameter == ListReduction::DistributeOverLists) {
    res = distributeReductionOverLists(e, *reductionContext);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  // Step 6: Handle booleans
  if (booleanParameter == BooleanReduction::UndefinedOnBooleans) {
    res = undefinedOnBooleans(e);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  // Step 7: Handle points
  if (pointParameter == PointReduction::UndefinedOnPoint) {
    res = undefinedOnPoint(e);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  return res;
}

OExpression SimplificationHelper::shallowReduceUndefined(OExpression e) {
  OExpression result;
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    /* The reduction is shortcut if one child is nonreal or undefined:
     * - the result is nonreal if at least one child is nonreal
     * - the result is undefined if at least one child is undefined but no child
     *   is nonreal */
    ExpressionNode::Type childIType = e.childAtIndex(i).type();
    if (childIType == ExpressionNode::Type::Nonreal) {
      result = Nonreal::Builder();
      break;
    } else if (childIType == ExpressionNode::Type::Undefined) {
      result = Undefined::Builder();
    }
  }
  if (!result.isUninitialized()) {
    e.replaceWithInPlace(result);
    return result;
  }
  return OExpression();
}

OExpression SimplificationHelper::shallowReduceBanningUnits(OExpression e) {
  // Generically, an OExpression does not accept any Unit in its children.
  if (e.hasUnit()) {
    return e.replaceWithUndefinedInPlace();
  }
  return OExpression();
}

OExpression SimplificationHelper::shallowReduceKeepingUnitsFromFirstChild(
    OExpression e, const ReductionContext& reductionContext) {
  OExpression child = e.childAtIndex(0);
  OExpression unit;
  child.removeUnit(&unit);
  if (!unit.isUninitialized()) {
    Multiplication mul = Multiplication::Builder(unit);
    e.replaceWithInPlace(mul);
    OExpression value = e.shallowReduce(reductionContext);
    if (value.isUndefined()) {
      // Undefined * _unit is Undefined. Same with Nonreal.
      mul.replaceWithInPlace(value);
      return value;
    }
    mul.addChildAtIndexInPlace(value, 0, 1);
    // In case `unit` was a multiplication of units, flatten
    mul.mergeSameTypeChildrenInPlace();
    return std::move(mul);
  }
  return OExpression();
}

OExpression SimplificationHelper::undefinedOnMatrix(
    OExpression e, ReductionContext* reductionContext) {
  if (!reductionContext->shouldCheckMatrices()) {
    return OExpression();
  }
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (e.childAtIndex(i).deepIsMatrix(reductionContext->context())) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  reductionContext->setCheckMatrices(false);
  return OExpression();
}

OExpression SimplificationHelper::undefinedOnBooleans(OExpression e) {
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (e.childAtIndex(i).hasBooleanValue()) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  return OExpression();
}

OExpression SimplificationHelper::undefinedOnPoint(OExpression e) {
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (e.childAtIndex(i).type() == ExpressionNode::Type::Point) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  return OExpression();
}

OExpression SimplificationHelper::distributeReductionOverLists(
    OExpression e, const ReductionContext& reductionContext) {
  int listLength = e.lengthOfListChildren();
  if (listLength == OExpression::k_noList) {
    /* No list in children, shallow reduce as usual. */
    return OExpression();
  } else if (listLength == OExpression::k_mismatchedLists) {
    /* Operators only apply to lists of the same length. */
    return e.replaceWithUndefinedInPlace();
  }
  /* We want to transform f({a,b},c) into {f(a,c),f(b,c)}.
   * Step 1 : Move all of 'this' children into another expression, so that
   * 'this' contains only ghosts children.
   * This is to ensure that no list will be duplicated in the pool when we'll
   * clone 'this' into the result list.
   * */
  int n = e.numberOfChildren();
  List children = List::Builder();
  for (int i = 0; i < n; i++) {
    // You can't mix lists and matrices
    if (e.childAtIndex(i).deepIsMatrix(
            reductionContext.context(),
            reductionContext.shouldCheckMatrices())) {
      return e.replaceWithUndefinedInPlace();
    }
    children.addChildAtIndexInPlace(e.childAtIndex(i), i, i);
  }
  assert(children.numberOfChildren() == n);
  /* Step 2 : Build the result list by cloning 'this' and readding
   * its children at the right place. If the child is a list, just add
   * the k-th element of the list. */
  List result = List::Builder();
  for (int listIndex = 0; listIndex < listLength; listIndex++) {
    OExpression element = e.clone();
    for (int childIndex = 0; childIndex < n; childIndex++) {
      OExpression child = children.childAtIndex(childIndex);
      if (child.type() == ExpressionNode::Type::List) {
        assert(child.numberOfChildren() == listLength);
        element.replaceChildAtIndexInPlace(childIndex,
                                           child.childAtIndex(listIndex));
      } else {
        element.replaceChildAtIndexInPlace(childIndex, child.clone());
      }
    }
    result.addChildAtIndexInPlace(element, listIndex, listIndex);
    element.shallowReduce(reductionContext);
  }
  e.replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

OExpression SimplificationHelper::bubbleUpDependencies(
    OExpression e, const ReductionContext& reductionContext) {
  assert(e.type() != ExpressionNode::Type::Store);
  if (e.type() == ExpressionNode::Type::Comparison) {
    return OExpression();
  }
  List dependencies = List::Builder();
  int nChildren = e.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    if (e.isParameteredExpression() &&
        (i == ParameteredExpression::ParameteredChildIndex())) {
      /* A parametered expression can have dependencies on its parameter, which
       * we don't want to factor, as the parameter does not have meaning
       * outside of the parametered expression.
       * The parametered expression will have to handle dependencies manually
       * in its shallowReduce. */
      continue;
    }
    OExpression child = e.childAtIndex(i);
    if (child.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency&>(child).extractDependencies(dependencies);
    }
  }
  if (dependencies.numberOfChildren() == 0) {
    return OExpression();
  }
  return reduceAfterBubblingUpDependencies(e, dependencies, reductionContext);
}

OExpression SimplificationHelper::reduceAfterBubblingUpDependencies(
    OExpression e, List dependencies,
    const ReductionContext& reductionContext) {
  assert(dependencies.numberOfChildren() > 0);
  e = e.shallowReduce(reductionContext);
  OExpression d = Dependency::Builder(Undefined::Builder(), dependencies);
  e.replaceWithInPlace(d);
  d.replaceChildAtIndexInPlace(0, e);
  if (e.type() == ExpressionNode::Type::Dependency) {
    static_cast<Dependency&>(e).extractDependencies(dependencies);
  }
  return d.shallowReduce(reductionContext);
}

bool SimplificationHelper::extractInteger(OExpression e,
                                          int* integerReturnValue,
                                          bool* isSymbolReturnValue) {
  *isSymbolReturnValue = false;
  int coef;
  OExpression numberExpression;
  if (e.isNumber()) {
    coef = 1;
    numberExpression = e;
  } else if (e.type() == ExpressionNode::Type::Opposite &&
             e.childAtIndex(0).isNumber()) {
    coef = -1;
    numberExpression = e.childAtIndex(0);
  } else {
    if (e.type() != ExpressionNode::Type::Symbol) {
      return false;
    }
    *isSymbolReturnValue = true;
    return true;
  }
  Number number = static_cast<Number&>(numberExpression);
  if (!number.isInteger()) {
    return false;
  }
  Integer integer = number.integerValue();
  if (!integer.isExtractable()) {
    return false;
  }
  *integerReturnValue = coef * integer.extractedInt();
  return true;
}

bool SimplificationHelper::extractIntegerChildAtIndex(
    OExpression e, int integerChildIndex, int* integerChildReturnValue,
    bool* isSymbolReturnValue) {
  assert(e.numberOfChildren() > integerChildIndex);
  OExpression child = e.childAtIndex(integerChildIndex);
  return extractInteger(child, integerChildReturnValue, isSymbolReturnValue);
}

}  // namespace Poincare
