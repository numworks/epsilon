#include <poincare/boolean.h>
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

Layout PiecewiseOperatorNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  PiecewiseOperatorLayout l = PiecewiseOperatorLayout::Builder();
  int n = numberOfChildren();
  int i = 0;
  while (i + 1 < n) {
    Layout leftChildLayout = childAtIndex(i)->createLayout(
        floatDisplayMode, numberOfSignificantDigits, context);
    Layout rightChildLayout = childAtIndex(i + 1)->createLayout(
        floatDisplayMode, numberOfSignificantDigits, context);
    l.addRow(leftChildLayout, rightChildLayout);
    i += 2;
  }
  if (i < n) {
    // Last child has no condition
    assert(n % 2 == 1 && i == n - 1);
    Layout leftChildLayout = childAtIndex(i)->createLayout(
        floatDisplayMode, numberOfSignificantDigits, context);
    l.addRow(leftChildLayout);
  }
  return std::move(l);
}

int PiecewiseOperatorNode::serialize(
    char* buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PiecewiseOperator::s_functionHelper.aliasesList().mainAlias());
}

Expression PiecewiseOperatorNode::shallowReduce(
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

Expression PiecewiseOperator::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  int n = children.numberOfChildren();
  // Check that each condition is boolean
  for (int i = 0; i < n; i++) {
    if (i % 2 == 0) {
      continue;
    }
    Expression condition = children.childAtIndex(i);
    if (!condition.hasBooleanValue() &&
        condition.type() != ExpressionNode::Type::EmptyExpression) {
      return Expression();
    }
  }
  return UntypedBuilderMultipleChildren<PiecewiseOperator>(children);
}

Expression PiecewiseOperator::shallowReduce(ReductionContext reductionContext) {
  {
    // Use custom dependencies bubbling-up for piecwise.
    Expression e = bubbleUpPiecewiseDependencies(reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    /* Do not use defaultShallowReduce since it calls shallowReduceUndfined
     * and a piecewiseOperator can be defined even with an undefined child. */
    e = SimplificationHelper::distributeReductionOverLists(*this,
                                                           reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  int n = numberOfChildren();
  assert(n > 0);
  int i = 0;
  while (i + 1 < n) {
    Expression condition = childAtIndex(i + 1);
    if (condition.hasBooleanValue()) {
      // Skip conditions that are not booleans. They are always false
      if (condition.type() != ExpressionNode::Type::Boolean) {
        // The condition is undeterminated, can't reduce
        return *this;
      }
      if (static_cast<Boolean&>(condition).value()) {
        // Condition is true
        Expression result = childAtIndex(i);
        replaceWithInPlace(result);
        return result;
      }
    }
    i += 2;
  }
  if (i < n) {
    // Last child has no condition and every other condition is false
    assert(n % 2 == 1 && i == n - 1);
    Expression result = childAtIndex(i);
    replaceWithInPlace(result);
    return result;
  }
  // Every condition is false
  return replaceWithUndefinedInPlace();
}

bool PiecewiseOperatorNode::derivate(const ReductionContext& reductionContext,
                                     Symbol symbol, Expression symbolValue) {
  return PiecewiseOperator(this).derivate(reductionContext, symbol,
                                          symbolValue);
}

bool PiecewiseOperator::derivate(const ReductionContext& reductionContext,
                                 Symbol symbol, Expression symbolValue) {
  {
    Expression e = Derivative::DefaultDerivate(*this, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return true;
    }
  }

  // diff(piecewise(exp1,cond1,exp2)) == piecewise(diff(exp1),cond1,diff(exp2)))

  int n = numberOfChildren();
  for (int i = 0; i < n; i += 2) {
    derivateChildAtIndexInPlace(i, reductionContext, symbol, symbolValue);
  }
  return true;
}

template <typename T>
int PiecewiseOperator::indexOfFirstTrueConditionWithValueForSymbol(
    const char* symbol, T x, Context* context,
    Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) const {
  assert(numberOfChildren() > 0);
  VariableContext variableContext = VariableContext(symbol, context);
  variableContext.setApproximationForVariable<T>(x);
  ApproximationContext approximationContext =
      ApproximationContext(&variableContext, complexFormat, angleUnit);
  return static_cast<PiecewiseOperatorNode*>(node())
      ->indexOfFirstTrueCondition<T>(approximationContext);
}

Expression PiecewiseOperator::bubbleUpPiecewiseDependencies(
    const ReductionContext& reductionContext) {
  /* - Do not bubble up dependencies of conditions.
   * - When bubbling up dependencies, keep conditions in dependencies list :
   *
   * Example:
   *
   * piecewise(
   *   dep(3,{1/x,tan(x)} , x>a,
   *   dep(4,{sqrt(x)})   , x>b,
   *   dep(5,{ln(x)})       otherwise)
   *
   * becomes
   *
   * dep(
   * piecewise(
   *   3 , x>a,
   *   4 , x>b,
   *   5 , otherwise),
   * {
   *   piecewise(
   *     1/x , x>a,
   *     0   , x>b,
   *     0     otherwise),
   *   piecewise(
   *     tan(x) , x>a,
   *     0      , x>b,
   *     0        otherwise),
   *   piecewise(
   *     0       , x>a,
   *     sqrt(x) , x>b,
   *     0         otherwise),
   *   piecewise(
   *     0    , x>a,
   *     0    , x>b,
   *     ln(x)  otherwise)
   * })
   *
   * piecwise(tan(x),x>1,0)})*/
  int nChildren = numberOfChildren();
  // Create a piecewise with same conditions but filled with 0.
  Expression genericPiecewiseDependency = clone();
  for (int i = 0; i < nChildren; i += 2) {
    genericPiecewiseDependency.replaceChildAtIndexInPlace(i,
                                                          Rational::Builder(0));
  }

  List dependencies = List::Builder();
  for (int i = 0; i < nChildren; i += 2) {
    Expression child = childAtIndex(i);
    int currentNDependencies = dependencies.numberOfChildren();
    if (child.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency&>(child).extractDependencies(dependencies);
    }
    int newNDependencies = dependencies.numberOfChildren();
    if (newNDependencies == currentNDependencies) {
      continue;
    }
    for (int k = currentNDependencies; k < newNDependencies; k++) {
      /* Clone the piecewise containing 0 and replace the expression at index i
       * with the dependency expression. */
      Expression piecewiseDependency = genericPiecewiseDependency.clone();
      piecewiseDependency.replaceChildAtIndexInPlace(
          i, dependencies.childAtIndex(k));
      dependencies.replaceChildAtIndexInPlace(k, piecewiseDependency);
    }
  }
  if (dependencies.numberOfChildren() > 0) {
    Expression e = shallowReduce(reductionContext);
    Expression d = Dependency::Builder(Undefined::Builder(), dependencies);
    e.replaceWithInPlace(d);
    d.replaceChildAtIndexInPlace(0, e);
    if (e.type() == ExpressionNode::Type::Dependency) {
      static_cast<Dependency&>(e).extractDependencies(dependencies);
    }
    return d.shallowReduce(reductionContext);
  }
  return Expression();
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
    float>(const char* symbol, float x, Context* context,
           Preferences::ComplexFormat complexFormat,
           Preferences::AngleUnit angleUnit) const;
template int PiecewiseOperator::indexOfFirstTrueConditionWithValueForSymbol<
    double>(const char* symbol, double x, Context* context,
            Preferences::ComplexFormat complexFormat,
            Preferences::AngleUnit angleUnit) const;

}  // namespace Poincare
