#include <poincare/simplification_helper.h>
#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/parenthesis.h>
#include <poincare/undefined.h>
#include <poincare/nonreal.h>
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

Expression SimplificationHelper::shallowReduceKeepingUnitsFromFirstChild(Expression e, ExpressionNode::ReductionContext reductionContext) {
  Expression child = e.childAtIndex(0);
  Expression unit;
  child.removeUnit(&unit);
  if (!unit.isUninitialized()) {
    Multiplication mul = Multiplication::Builder(unit);
    e.replaceWithInPlace(mul);
    Expression value = e.shallowReduce(reductionContext);
    if (value.type() == ExpressionNode::Type::Nonreal || value.type() == ExpressionNode::Type::Undefined) {
      // Undefined * _unit is Undefined. Same with Nonreal.
      mul.replaceWithInPlace(value);
      return value;
    }
    mul.addChildAtIndexInPlace(value, 0, 1);
    // In case `unit` was a multiplication of units, flatten
    mul.mergeSameTypeChildrenInPlace();
    return std::move(mul);
  }
  return Expression();
}

Expression SimplificationHelper::shallowReduceUndefinedKeepingUnitsFromFirstChild(Expression e, ExpressionNode::ReductionContext reductionContext) {
  Expression res = shallowReduceUndefined(e);
  if (res.isUninitialized()) {
    res = shallowReduceKeepingUnitsFromFirstChild(e, reductionContext);
  }
  return res;
}

Expression SimplificationHelper::undefinedOnMatrix(Expression e, ExpressionNode::ReductionContext reductionContext) {
  int n = e.numberOfChildren();
  for (int i = 0; i < n ; i ++) {
    if (e.childAtIndex(i).deepIsMatrix(reductionContext.context())) {
      return Undefined::Builder();
    }
  }
  return Expression();
}

Expression SimplificationHelper::distributeReductionOverLists(Expression e, ExpressionNode::ReductionContext reductionContext) {
  int listLength = e.lengthOfListChildren();
  if (listLength == Expression::k_noList) {
    /* No list in children, shallow reduce as usual. */
    return Expression();
  } else if (listLength == Expression::k_mismatchedLists) {
    /* Operators only apply to lists of the same length. */
    return e.replaceWithUndefinedInPlace();
  }
  /* We want to transform f({a,b},c) into {f(a,c),f(b,c)} */
  int n = e.numberOfChildren();
  List children = List::Builder();
  for (int i = 0; i < n; i++) {
    children.addChildAtIndexInPlace(e.childAtIndex(i), i, i);
  }
  assert(children.numberOfChildren() == n);
  /* We moved all of our children into another expression. Now, by cloning
   * 'this', we get an empty expression with the right type, to be inserted
   * into the result list. */
  List result = List::Builder();
  for (int listIndex = 0; listIndex < listLength; listIndex++) {
    Expression element = e.clone();
    for (int childIndex = 0; childIndex < n; childIndex++) {
      Expression child = children.childAtIndex(childIndex);
      if (child.type() == ExpressionNode::Type::List) {
        assert(child.numberOfChildren() == listLength);
        element.replaceChildAtIndexInPlace(childIndex, child.childAtIndex(listIndex));
      } else {
        element.replaceChildAtIndexInPlace(childIndex, child.clone());
      }
    }
    result.addChildAtIndexInPlace(element, listIndex, listIndex);
    element.shallowReduce(reductionContext);
  }
  e.replaceWithInPlace(result);
  return std::move(result);
}

}
