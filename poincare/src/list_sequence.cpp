#include <poincare/based_integer.h>
#include <poincare/integer.h>
#include <poincare/list.h>
#include <poincare/list_sequence.h>
#include <poincare/list_sequence_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>

namespace Poincare {

constexpr OExpression::FunctionHelper ListSequence::s_functionHelper;

int ListSequenceNode::numberOfChildren() const {
  return ListSequence::s_functionHelper.numberOfChildren();
}

OExpression ListSequenceNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListSequence(this).shallowReduce(reductionContext);
}

size_t ListSequenceNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListSequence::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> ListSequenceNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ListComplex<T> list = ListComplex<T>::Builder();
  T upperBound =
      childAtIndex(2)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(upperBound) || upperBound < 1) {
    return Complex<T>::Undefined();
  }
  for (int i = 1; i <= static_cast<int>(upperBound); i++) {
    list.addChildAtIndexInPlace(approximateFirstChildWithArgument(
                                    static_cast<T>(i), approximationContext),
                                list.numberOfChildren(),
                                list.numberOfChildren());
  }
  return std::move(list);
}

OExpression ListSequence::UntypedBuilder(OExpression children) {
  assert(children.type() == ExpressionNode::Type::OList);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return OExpression();
  }
  return Builder(children.childAtIndex(0),
                 children.childAtIndex(1).convert<Symbol>(),
                 children.childAtIndex(2));
}

OExpression ListSequence::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::DefinedOnMatrix,
        SimplificationHelper::ListReduction::DoNotDistributeOverLists,
        SimplificationHelper::PointReduction::DefinedOnPoint);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  assert(childAtIndex(1).type() == ExpressionNode::Type::Symbol);
  OExpression function = childAtIndex(0);
  OExpression variableExpression = childAtIndex(1);
  Symbol variable = static_cast<Symbol&>(variableExpression);

  int upperBound;
  int upperBoundIndex = 2;
  bool indexIsSymbol;
  bool indexIsInteger = SimplificationHelper::extractIntegerChildAtIndex(
      *this, upperBoundIndex, &upperBound, &indexIsSymbol);
  if (!indexIsInteger) {
    return replaceWithUndefinedInPlace();
  }
  if (indexIsSymbol) {
    return *this;
  }

  OList finalList = OList::Builder();
  for (int i = 1; i <= upperBound; i++) {
    OExpression newListElement = function.clone().replaceSymbolWithExpression(
        variable, BasedInteger::Builder(Integer(i)));
    finalList.addChildAtIndexInPlace(newListElement, i - 1, i - 1);
    newListElement.deepReduce(reductionContext);
  }

  replaceWithInPlace(finalList);
  return finalList.shallowReduce(reductionContext);
}

}  // namespace Poincare
