#include <poincare/comparison_operator.h>
#include <poincare/code_point_layout.h>
#include <poincare/evaluation.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/subtraction.h>

namespace Poincare {

Layout ComparisonOperatorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), 0, false);
  Layout operatorLayout = CodePointLayout::Builder(comparisonCodePoint());
  operatorLayout.setMargin(true);
  result.addChildAtIndex(operatorLayout, result.numberOfChildren(), result.numberOfChildren(), nullptr);
  Layout secondChildLayout = childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  secondChildLayout.setMargin(true);
  result.addOrMergeChildAtIndex(secondChildLayout, result.numberOfChildren(), false);
  return std::move(result);
}

int ComparisonOperatorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, comparisonString());
}

Evaluation<float> ComparisonOperatorNode::approximate(SinglePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<float>(approximationContext);
}

Evaluation<double> ComparisonOperatorNode::approximate(DoublePrecision p, const ApproximationContext& approximationContext) const {
  return templatedApproximate<double>(approximationContext);
}

template<typename T>
Evaluation<T> ComparisonOperatorNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> firstChildApprox = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> secondChildApprox = childAtIndex(1)->approximate(T(), approximationContext);
  if (firstChildApprox.type() != EvaluationNode<T>::Type::Complex || secondChildApprox.type() != EvaluationNode<T>::Type::Complex) {
    return Complex<T>::Undefined();
  }
  std::complex<T> difference = firstChildApprox.complexAtIndex(0) - secondChildApprox.complexAtIndex(0);
  T scalarDifference = ComplexNode<T>::ToScalar(difference);
  ComparisonOperator::TrinaryBoolean chidlrenAreEqual;
  ComparisonOperator::TrinaryBoolean leftChildIsGreater;
  if (std::isnan(scalarDifference)) {
    leftChildIsGreater = ComparisonOperator::TrinaryBoolean::Unknown;
    chidlrenAreEqual = (firstChildApprox.isUndefined() || secondChildApprox.isUndefined()) ? ComparisonOperator::TrinaryBoolean::Unknown : ComparisonOperator::TrinaryBoolean::False;
  } else {
    leftChildIsGreater = scalarDifference >= 0.0 ? ComparisonOperator::TrinaryBoolean::True : ComparisonOperator::TrinaryBoolean::False;
    chidlrenAreEqual = (std::fabs(scalarDifference) < Float<T>::EpsilonLax()) ? ComparisonOperator::TrinaryBoolean::True : ComparisonOperator::TrinaryBoolean::False;
  }
  ComparisonOperator::TrinaryBoolean truthValue = ComparisonOperator::TrinaryTruthValue(type(), chidlrenAreEqual, leftChildIsGreater);
  switch (truthValue) {
  case ComparisonOperator::TrinaryBoolean::False:
    return Complex<T>::Builder(0.0);
  case ComparisonOperator::TrinaryBoolean::True:
    return Complex<T>::Builder(1.0);
  default:
    assert(truthValue == ComparisonOperator::TrinaryBoolean::Unknown);
    return Complex<T>::Undefined();
  }
}

Expression ComparisonOperatorNode::shallowReduce(const ReductionContext& reductionContext) {
  return ComparisonOperator(this).shallowReduce(reductionContext);
}

bool ComparisonOperator::IsComparisonOperatorType(ExpressionNode::Type type) {
  switch (type) {
  case ExpressionNode::Type::Equal:
  case ExpressionNode::Type::Superior:
  case ExpressionNode::Type::SuperiorEqual:
  case ExpressionNode::Type::Inferior:
  case ExpressionNode::Type::InferiorEqual:
    return true;
  default:
    return false;
  }
}

ExpressionNode::Type ComparisonOperator::Opposite(ExpressionNode::Type type) {
  switch (type) {
  case ExpressionNode::Type::Superior:
    return ExpressionNode::Type::Inferior;
  case ExpressionNode::Type::SuperiorEqual:
    return ExpressionNode::Type::InferiorEqual;
  case ExpressionNode::Type::Inferior:
    return ExpressionNode::Type::Superior;
  default:
    assert(type == ExpressionNode::Type::InferiorEqual);
    return ExpressionNode::Type::SuperiorEqual;
  }
}

ComparisonOperator::TrinaryBoolean ComparisonOperator::TrinaryTruthValue(ExpressionNode::Type type, TrinaryBoolean chidlrenAreEqual, TrinaryBoolean leftChildIsGreater) {
  assert(IsComparisonOperatorType(type));
  if (chidlrenAreEqual == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  if (chidlrenAreEqual == TrinaryBoolean::True) {
    bool result = (type == ExpressionNode::Type::SuperiorEqual || type == ExpressionNode::Type::InferiorEqual || type == ExpressionNode::Type::Equal);
    return result ? TrinaryBoolean::True : TrinaryBoolean::False;
  }
  assert(chidlrenAreEqual == TrinaryBoolean::False);
  if (type == ExpressionNode::Type::Equal) {
    return TrinaryBoolean::False;
  }
  if (leftChildIsGreater == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  bool leftIsGreater = leftChildIsGreater == TrinaryBoolean::True;
  bool operatorIsSuperior = (type == ExpressionNode::Type::SuperiorEqual || type == ExpressionNode::Type::Superior);
  return (operatorIsSuperior == leftIsGreater) ? TrinaryBoolean::True : TrinaryBoolean::False;
}


Expression ComparisonOperator::standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const {
  Expression sub = Subtraction::Builder(childAtIndex(0).clone(), childAtIndex(1).clone());
  return sub.cloneAndReduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, reductionTarget));
}

Expression ComparisonOperator::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        reductionContext
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression difference = Subtraction::Builder(childAtIndex(0), childAtIndex(1));
  difference = difference.shallowReduce(reductionContext);
  ExpressionNode::NullStatus differenceNullStatus = difference.nullStatus(reductionContext.context());
  ExpressionNode::Sign differenceSign = difference.sign(reductionContext.context());

  TrinaryBoolean childrenAreEqual =
    differenceNullStatus == ExpressionNode::NullStatus::Unknown ?
      TrinaryBoolean::Unknown :
      (differenceNullStatus == ExpressionNode::NullStatus::Null ?
        TrinaryBoolean::True :
        TrinaryBoolean::False);
  TrinaryBoolean leftIsGreater =
    differenceSign == ExpressionNode::Sign::Unknown ?
      TrinaryBoolean::Unknown :
      (differenceSign == ExpressionNode::Sign::Positive ?
        TrinaryBoolean::True :
        TrinaryBoolean::False);
  TrinaryBoolean comparison = TrinaryTruthValue(type(), childrenAreEqual, leftIsGreater);
  if (comparison == TrinaryBoolean::Unknown) {
    replaceChildAtIndexInPlace(0, difference);
    replaceChildAtIndexInPlace(1, Rational::Builder(0));
    return *this; // Let approximation decide
  }
  Expression result;
  if (comparison == TrinaryBoolean::True) {
    result = Rational::Builder(1);
  } else {
    assert(comparison == TrinaryBoolean::False);
    result = Rational::Builder(0);
  }
  replaceWithInPlace(result);
  return result;
}

}
