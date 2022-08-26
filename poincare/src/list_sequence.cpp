#include <poincare/list_sequence.h>
#include <poincare/based_integer.h>
#include <poincare/integer.h>
#include <poincare/list.h>
#include <poincare/list_sequence_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>

namespace Poincare {

constexpr Expression::FunctionHelper ListSequence::s_functionHelper;

int ListSequenceNode::numberOfChildren() const {
  return ListSequence::s_functionHelper.numberOfChildren();
}

Expression ListSequenceNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListSequence(this).shallowReduce(reductionContext);
}

Layout ListSequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return ListSequenceLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int ListSequenceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSequence::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Evaluation<T> ListSequenceNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> list = ListComplex<T>::Builder();
  T upperBound = childAtIndex(2)->approximate(T(), approximationContext).toScalar();
    if (std::isnan(upperBound) || upperBound < 1) {
    return Complex<T>::Undefined();
  }
  for (int i = 1; i <= static_cast<int>(upperBound); i++) {
     list.addChildAtIndexInPlace(approximateFirstChildWithArgument(static_cast<T>(i), approximationContext), list.numberOfChildren(), list.numberOfChildren());
  }
  return std::move(list);
}

Expression ListSequence::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
}

Expression ListSequence::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  assert(childAtIndex(1).type() == ExpressionNode::Type::Symbol);
  Expression function = childAtIndex(0);
  Expression variableExpression = childAtIndex(1);
  Symbol variable = static_cast<Symbol &>(variableExpression);

  int upperBound;
  int upperBoundIndex = 2;
  bool indexIsSymbol;
  bool indexIsInteger = SimplificationHelper::extractIntegerChildAtIndex(*this, upperBoundIndex, &upperBound, &indexIsSymbol);
  if (!indexIsInteger) {
    return replaceWithUndefinedInPlace();
  }
  if (indexIsSymbol) {
    return *this;
  }

  List finalList = List::Builder();
  for (int i = 1; i <= upperBound; i++) {
    Expression newListElement = function.clone().replaceSymbolWithExpression(variable, BasedInteger::Builder(Integer(i)));
    finalList.addChildAtIndexInPlace(newListElement, i - 1, i - 1);
    newListElement.deepReduce(reductionContext);
  }

  replaceWithInPlace(finalList);
  return finalList.shallowReduce(reductionContext);
}

}
