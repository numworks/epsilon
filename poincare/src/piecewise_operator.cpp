#include <poincare/addition.h>
#include <poincare/boolean.h>
#include <poincare/comparison.h>
#include <poincare/dependency.h>
#include <poincare/derivative.h>
#include <poincare/horizontal_layout.h>
#include <poincare/piecewise_operator.h>
#include <poincare/piecewise_operator_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>

namespace Poincare {

size_t PiecewiseOperatorNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PiecewiseOperator::s_functionHelper.aliasesList().mainAlias());
}

OExpression PiecewiseOperatorNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return PiecewiseOperator(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> PiecewiseOperatorNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  // TODO: Distribute on lists
  int i = indexOfFirstTrueCondition<T>(approximationContext);
  if (i < 0) {
    return Complex<T>::Undefined();
  }
  return childAtIndex(i)->approximate(T(), approximationContext);
}

template <typename T>
int PiecewiseOperatorNode::indexOfFirstTrueCondition(
    const ApproximationContext& approximationContext) const {
  int n = numberOfChildren();
  assert(n > 0);
  int i = 0;
  while (i + 1 < n) {
    Evaluation<T> condition =
        childAtIndex(i + 1)->approximate(T(), approximationContext);
    /* If condition is not boolean (if it's undef for example), it is considered
     * false */
    if (condition.type() == EvaluationNode<T>::Type::BooleanEvaluation &&
        static_cast<BooleanEvaluation<T>&>(condition).value()) {
      return i;
    }
    i += 2;
  }
  if (i < n) {
    // Last child has no condition and every other condition is false
    assert(n % 2 == 1 && i == n - 1);
    return i;
  }
  // Every condition is false.
  return -1;
}

OExpression PiecewiseOperator::UntypedBuilder(OExpression children) {
  assert(children.type() == ExpressionNode::Type::OList);
  int n = children.numberOfChildren();
  // Check that each condition is boolean
  for (int i = 0; i < n; i++) {
    if (i % 2 == 0) {
      continue;
    }
    OExpression condition = children.childAtIndex(i);
    if (!condition.hasBooleanValue() &&
        condition.type() != ExpressionNode::Type::EmptyExpression) {
      return OExpression();
    }
  }
  return UntypedBuilderMultipleChildren<PiecewiseOperator>(children);
}

OExpression PiecewiseOperator::shallowReduce(
    ReductionContext reductionContext) {
  /* TODO: add a check that all children expressions (not the conditions) have
   * the same dimension, otherwise reduce to undef */
  {
    // Use custom dependencies bubbling-up for piecewise.
    OExpression e = bubbleUpPiecewiseDependencies(reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    /* Do not use defaultShallowReduce since it calls shallowReduceUndefined
     * and a piecewiseOperator can be defined even with an undefined child. */
    e = SimplificationHelper::distributeReductionOverLists(*this,
                                                           reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  int n = numberOfChildren();
  assert(n > 0);

  // Bubble-up undef from conditions
  for (int i = 1; i < n; i += 2) {
    if (childAtIndex(i).isUndefined()) {
      return replaceWithUndefinedInPlace();
    }
  }

  int i = 0;
  while (i + 1 < n) {
    OExpression condition = childAtIndex(i + 1);
    if (condition.hasBooleanValue()) {
      // Skip conditions that are not booleans. They are always false
      if (condition.type() != ExpressionNode::Type::OBoolean) {
        // The condition is undetermined, can't reduce
        return *this;
      }
      if (static_cast<OBoolean&>(condition).value()) {
        // Condition is true
        OExpression result = childAtIndex(i);
        replaceWithInPlace(result);
        return result;
      }
    }
    i += 2;
  }
  if (i < n) {
    // Last child has no condition and every other condition is false
    assert(n % 2 == 1 && i == n - 1);
    OExpression result = childAtIndex(i);
    replaceWithInPlace(result);
    return result;
  }
  // Every condition is false
  return replaceWithUndefinedInPlace();
}

bool PiecewiseOperatorNode::derivate(const ReductionContext& reductionContext,
                                     Symbol symbol, OExpression symbolValue) {
  return PiecewiseOperator(this).derivate(reductionContext, symbol,
                                          symbolValue);
}

bool PiecewiseOperator::derivate(const ReductionContext& reductionContext,
                                 Symbol symbol, OExpression symbolValue) {
  {
    OExpression e =
        Derivative::DefaultDerivate(*this, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return true;
    }
  }

  /* To derivate piecewise, we just need to derivate the expressions and keep
   * the same conditions except that the inferior equal and superior equal
   * should turn into strict inferior and superior.
   * Indeed the derivative of piecewise(x,x>=0,-x) in x=0 is undef.
   *
   * To do so, each condition is duplicated.
   * The first one is transformed to change inferior equal into inferior (same
   * for superior), and is matched with the derivative.
   * The second one is transformed to change inferior into inferior equal (same
   * for superior), and is matched with undef.
   * Since only the first true condition will be applied, if x is in the open
   * interval, the derivative is properly computed, if it's at bounds, it's
   * undef.
   *
   * Example:
   * piecewise(
   *   x , 0 < cos(x) <= 0.5,
   *   0
   * )
   * differentiates into
   * piecewise(
   *   1     , 0 < cos(x) < 0.5,
   *   undef , 0 <= cos(x) <= 0.5,
   *   0
   * )*/

  int i = 0;
  while (i < numberOfChildren()) {
    // Derivate the expression
    derivateChildAtIndexInPlace(i, reductionContext, symbol, symbolValue);
    if (i == numberOfChildren() - 1) {
      // No condition for last expression.
      break;
    }
    OExpression originalCondition = childAtIndex(i + 1);
    if (originalCondition.type() != ExpressionNode::Type::Comparison) {
      i += 2;
      continue;
    }
    // Turn >= into > and <= into <
    OExpression strictCondition =
        static_cast<Comparison&>(originalCondition).cloneWithStrictOperators();
    OExpression lenientCondition =
        static_cast<Comparison&>(originalCondition).cloneWithLenientOperators();
    // Add the undef expression with the condition containing <=, >=, ==
    assert(!lenientCondition.isUndefined());
    addChildAtIndexInPlace(Undefined::Builder(), i + 2, numberOfChildren());
    addChildAtIndexInPlace(lenientCondition, i + 3, numberOfChildren());
    if (strictCondition.isUndefined()) {
      /* It's undef if it contained a ==. In this case, the derivative is just
       * undef when the condition is true. */
      removeChildAtIndexInPlace(i);
      removeChildAtIndexInPlace(i);
      i -= 2;
    } else {
      // Replace the condition containing >=, <= with the one with > and <
      replaceChildAtIndexInPlace(i + 1, strictCondition);
    }
    i += 4;
  }
  return true;
}

template <typename T>
int PiecewiseOperator::indexOfFirstTrueConditionWithValueForSymbol(
    const char* symbol, T x,
    const ApproximationContext& approximationContext) const {
  assert(numberOfChildren() > 0);
  VariableContext variableContext =
      VariableContext(symbol, approximationContext.context());
  variableContext.setApproximationForVariable<T>(x);
  ApproximationContext newContext = ApproximationContext(
      &variableContext, approximationContext.complexFormat(),
      approximationContext.angleUnit());
  return static_cast<PiecewiseOperatorNode*>(node())
      ->indexOfFirstTrueCondition<T>(newContext);
}

OExpression PiecewiseOperator::bubbleUpPiecewiseDependencies(
    const ReductionContext& reductionContext) {
  /* - Do not bubble up dependencies of conditions.
   * - When bubbling up dependencies, keep conditions in dependencies list :
   *
   * Example:
   *
   * piecewise(
   *   2                  , x>a,
   *   dep(3,{1/x,tan(x)} , x>b,
   *   dep(4,{sqrt(x)})   , x>c,
   *   dep(5,{ln(x)})       otherwise)
   *
   * becomes
   *
   * dep(
   * piecewise(
   *   2 , x>a,
   *   3 , x>b,
   *   4 , x>c,
   *   5 , otherwise),
   * {
   *   piecewise(
   *     0            , x>a,
   *     1/x + tan(x) , x>b,
   *     sqrt(x)      , x>c,
   *     ln(x)          otherwise)
   * })
   *
   * TODO:
   * Adding 1/x and tan(x) here is a hack to make sure the expression is undef
   * if one of these is undef. The best would be to turn it into:
   * dep(
   * piecewise(
   *   2 , x>a,
   *   3 , x>b,
   *   4 , x>c,
   *   5 , otherwise),
   * {
   *   piecewise(
   *     [ ]           , x>a,
   *     [1/x, tan(x)] , x>b,
   *     [sqrt(x)]     , x>c,
   *     [ln(x)]         otherwise)
   * })
   * but for now a matrix is NOT undef if one of its children is undef. This
   * should be changed soon though.
   * */
  int nChildren = numberOfChildren();
  // Create a piecewise with same conditions but filled with 0.
  OExpression piecewiseDependency = clone();
  for (int i = 0; i < nChildren; i += 2) {
    piecewiseDependency.replaceChildAtIndexInPlace(i, Rational::Builder(0));
  }

  OList dependencies = OList::Builder();
  for (int i = 0; i < nChildren; i += 2) {
    OExpression child = childAtIndex(i);
    int currentNDependencies = dependencies.numberOfChildren();
    if (child.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency&>(child).extractDependencies(dependencies);
    }
    int newNDependencies = dependencies.numberOfChildren();
    if (newNDependencies == currentNDependencies) {
      continue;
    }
    OExpression dependencyExpression;
    if (newNDependencies == currentNDependencies + 1) {
      dependencyExpression =
          dependencies.childAtIndex(newNDependencies - 1).clone();
    } else {
      Addition a = Addition::Builder();
      for (int k = currentNDependencies; k < newNDependencies; k++) {
        a.addChildAtIndexInPlace(dependencies.childAtIndex(k).clone(),
                                 a.numberOfChildren(), a.numberOfChildren());
      }
      dependencyExpression = a;
    }
    piecewiseDependency.replaceChildAtIndexInPlace(i, dependencyExpression);
  }
  if (dependencies.numberOfChildren() == 0) {
    return OExpression();
  }
  dependencies = OList::Builder();
  dependencies.addChildAtIndexInPlace(piecewiseDependency, 0, 0);
  return SimplificationHelper::reduceAfterBubblingUpDependencies(
      *this, dependencies, reductionContext);
}

template Evaluation<float> PiecewiseOperatorNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> PiecewiseOperatorNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

template int PiecewiseOperatorNode::indexOfFirstTrueCondition<float>(
    const ApproximationContext& approximationContext) const;
template int PiecewiseOperatorNode::indexOfFirstTrueCondition<double>(
    const ApproximationContext& approximationContext) const;

template int PiecewiseOperator::indexOfFirstTrueConditionWithValueForSymbol<
    float>(const char* symbol, float x,
           const ApproximationContext& approximationContext) const;
template int PiecewiseOperator::indexOfFirstTrueConditionWithValueForSymbol<
    double>(const char* symbol, double x,
            const ApproximationContext& approximationContext) const;

}  // namespace Poincare
