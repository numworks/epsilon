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


Expression SimplificationHelper::shallowReduceUndefined(Expression e) {
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
  return Expression();
}


Expression SimplificationHelper::shallowReduceBanningUnits(Expression e) {
  // Generically, an Expression does not accept any Unit in its children.
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression unit;
    Expression childI = e.childAtIndex(i).removeUnit(&unit);
    if (childI.isUndefined() || !unit.isUninitialized()) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  return Expression();
}

Expression SimplificationHelper::defaultShallowReduce(Expression e) {
  Expression res = shallowReduceUndefined(e);
  if (res.isUninitialized()) {  // did nothing
    res = shallowReduceBanningUnits(e);
  }
  return res;
}

Expression SimplificationHelper::shallowReduceKeepingUnits(Expression e, ExpressionNode::ReductionContext reductionContext) {
  Expression child = e.childAtIndex(0);
  Expression unit;
  child.removeUnit(&unit);
  if (!unit.isUninitialized()) {

    Multiplication mul = Multiplication::Builder(unit);
    e.replaceWithInPlace(mul);
    Expression value = e.shallowReduce(reductionContext);
    mul.addChildAtIndexInPlace(value, 0, 1);
    mul.mergeSameTypeChildrenInPlace();
    return std::move(mul);
  }
  return Expression();
}

Expression SimplificationHelper::shallowReduceUndefinedAndUnits(Expression e, ExpressionNode::ReductionContext reductionContext) {
  Expression res = shallowReduceUndefined(e);
  if (res.isUninitialized()) {
    res = shallowReduceKeepingUnits(e, reductionContext);
  }
  return res;
}

}