#include <poincare/simplification_helper.h>
#include <poincare/boolean.h>
#include <poincare/dependency.h>
#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/parametered_expression.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/nonreal.h>
#include <assert.h>


namespace Poincare {


void SimplificationHelper::deepBeautifyChildren(Expression e, const ExpressionNode::ReductionContext& reductionContext) {
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

void SimplificationHelper::defaultDeepReduceChildren(Expression e, const ExpressionNode::ReductionContext& reductionContext) {
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    assert(childrenCount == e.numberOfChildren());
    e.childAtIndex(i).deepReduce(reductionContext);
  }
}

Expression SimplificationHelper::defaultShallowReduce(Expression e, ExpressionNode::ReductionContext * reductionContext,  BooleanReduction booleanParameter, UnitReduction unitParameter, MatrixReduction matrixParameter, ListReduction listParameter) {
  // Step1. Shallow reduce undefined
  Expression res = shallowReduceUndefined(e);
  if (!res.isUninitialized()) {
    return res;
  }
  // Step2. Bubble up dependencies
  res = bubbleUpDependencies(e, *reductionContext);
  if (!res.isUninitialized()) {
    return res;
  }
  // Step3. Handle units
  if (unitParameter == UnitReduction::BanUnits) {
    res = shallowReduceBanningUnits(e);
  } else if (unitParameter == UnitReduction::ExtractUnitsOfFirstChild) {
    res = shallowReduceKeepingUnitsFromFirstChild(e, *reductionContext);
  }
  if (!res.isUninitialized()) {
    return res;
  }
  // Step4. Handle matrices
  if (matrixParameter == MatrixReduction::UndefinedOnMatrix) {
    res = undefinedOnMatrix(e, reductionContext);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  // Step5. Handle lists
  if (listParameter == ListReduction::DistributeOverLists) {
    res = distributeReductionOverLists(e, *reductionContext);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  // Step6. Handle booleans
  if (booleanParameter == BooleanReduction::UndefinedOnBooleans) {
    res = undefinedOnBooleans(e);
    if (!res.isUninitialized()) {
      return res;
    }
  }
  return res;
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

Expression SimplificationHelper::shallowReduceKeepingUnitsFromFirstChild(Expression e, const ExpressionNode::ReductionContext& reductionContext) {
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

Expression SimplificationHelper::undefinedOnMatrix(Expression e, ExpressionNode::ReductionContext * reductionContext) {
  if (!reductionContext->shouldCheckMatrices()) {
    return Expression();
  }
  int n = e.numberOfChildren();
  for (int i = 0; i < n ; i ++) {
    if (e.childAtIndex(i).deepIsMatrix(reductionContext->context())) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  reductionContext->setCheckMatrices(false);
  return Expression();
}

Expression SimplificationHelper::undefinedOnBooleans(Expression e) {
  int n = e.numberOfChildren();
  for (int i = 0; i < n ; i ++) {
    if (e.childAtIndex(i).hasBooleanValue()) {
      return e.replaceWithUndefinedInPlace();
    }
  }
  return Expression();
}

Expression SimplificationHelper::distributeReductionOverLists(Expression e, const ExpressionNode::ReductionContext& reductionContext) {
  int listLength = e.lengthOfListChildren();
  if (listLength == Expression::k_noList) {
    /* No list in children, shallow reduce as usual. */
    return Expression();
  } else if (listLength == Expression::k_mismatchedLists) {
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
    if (e.childAtIndex(i).deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
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
  return  result.shallowReduce(reductionContext);
}


Expression SimplificationHelper::bubbleUpDependencies(Expression e, const ExpressionNode::ReductionContext& reductionContext) {
  assert(e.type() != ExpressionNode::Type::Store);
  if (e.type() == ExpressionNode::Type::Comparison) {
    return Expression();
  }
  List dependencies = List::Builder();
  int nChildren = e.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    if (e.isParameteredExpression() && (i == ParameteredExpression::ParameteredChildIndex())) {
      /* A parametered expression can have dependencies on its parameter, which
       * we don't want to factor, as the parameter does not have meaning
       * outside of the parametered expression.
       * The parametered expression will have to handle dependencies manually
       * in its shallowReduce. */
      continue;
    }
    Expression child = e.childAtIndex(i);
    if (child.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency &>(child).extractDependencies(dependencies);
    }
  }
  if (dependencies.numberOfChildren() > 0) {
    e = e.shallowReduce(reductionContext);
    Expression d = Dependency::Builder(Undefined::Builder(), dependencies);
    e.replaceWithInPlace(d);
    d.replaceChildAtIndexInPlace(0, e);
    if (e.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency &>(e).extractDependencies(dependencies);
    }
    return d.shallowReduce(reductionContext);
  }
  return Expression();
}

bool SimplificationHelper::extractIntegerChildAtIndex(Expression e, int integerChildIndex, int * integerChildReturnValue, bool * isSymbolReturnValue) {
  assert(e.numberOfChildren() > integerChildIndex);
  Expression child = e.childAtIndex(integerChildIndex);
  *isSymbolReturnValue = false;
  if (child.type() != ExpressionNode::Type::Rational) {
    if (child.type() != ExpressionNode::Type::Symbol) {
      return false;
    }
    *isSymbolReturnValue = true;
    return true;
  }
  Rational rationalChild = static_cast<Rational &>(child);
  Integer integerChild = rationalChild.signedIntegerNumerator();
  if (!rationalChild.isInteger() || !integerChild.isExtractable()) {
    return false;
  }
  *integerChildReturnValue = integerChild.extractedInt();
  return true;
}

}
