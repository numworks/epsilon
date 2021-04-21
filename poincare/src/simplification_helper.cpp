#include <poincare/simplification_helper.h>

#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/parenthesis.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <assert.h>


namespace Poincare {


void SimplificationHelper::defaultDeepBeautifyChildren(Expression e, ExpressionNode::ReductionContext reductionContext) {
  const int nbChildren = e.numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    Expression child = e.childAtIndex(i);
    child = child.deepBeautify(reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (e.node()->childAtIndexNeedsUserParentheses(child, i)) {
      e.replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}


void SimplificationHelper::defaultDeepReduceChildren(Expression e, ExpressionNode::ReductionContext reductionContext) {
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    assert(childrenCount == e.numberOfChildren());
    e.childAtIndex(i).deepReduce(reductionContext);
  }
}


Expression SimplificationHelper::defaultShallowReduce(Expression e) {
  Expression result;
  if (Expression::SimplificationHasBeenInterrupted()) {
    result = Undefined::Builder();
  } else {
    const int childrenCount = e.numberOfChildren();
    for (int i = 0; i < childrenCount; i++) {
      /* The reduction is shortcut if one child is unreal or undefined:
       * - the result is unreal if at least one child is unreal
       * - the result is undefined if at least one child is undefined but no child
       *   is unreal */
      ExpressionNode::Type childIType = e.childAtIndex(i).type();
      if (childIType == ExpressionNode::Type::Unreal) {
        result = Unreal::Builder();
        break;
      } else if (childIType == ExpressionNode::Type::Undefined) {
        result = Undefined::Builder();
      }
    }
  }
  if (!result.isUninitialized()) {
    e.replaceWithInPlace(result);
    return result;
  }
  return e;
}


Expression SimplificationHelper::defaultHandleUnitsInChildren(Expression e) {
  // Generically, an Expression does not accept any Unit in its children.
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression unit;
    Expression childI = e.childAtIndex(i).removeUnit(&unit);
    if (childI.isUndefined() || !unit.isUninitialized()) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  return e;
}


Expression SimplificationHelper::shallowReducePotentialUnit(Expression e, ExpressionNode::ReductionContext reductionContext, bool * handledUnits) {
  Expression exp = defaultShallowReduce(e);
  if (exp.isUndefined()) {
    *handledUnits = true;
    return exp;
  }
  Expression child = e.childAtIndex(0);
  Expression unit;
  child.removeUnit(&unit);
  if (!unit.isUninitialized()) {
    *handledUnits = true;
    // We cannot create the multiplication directly from the value + unit,
    // because we would lose all ref to value.parent()
    // Step 1: create the mul node half empty, and register it to value's parent
    Multiplication mul = Multiplication::Builder(unit);
    e.replaceWithInPlace(mul);
    Expression value = e.shallowReduce(reductionContext);
    // Step 2: Then add addition as mul's child
    mul.addChildAtIndexInPlace(value, 0, 1);
    mul.mergeSameTypeChildrenInPlace();
    return std::move(mul);
  }
  *handledUnits = false;
  return e;
}

}