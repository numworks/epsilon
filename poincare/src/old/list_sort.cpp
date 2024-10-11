#include <poincare/layout.h>
#include <poincare/old/helpers.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/list_sort.h>
#include <poincare/old/point_evaluation.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

int ListSortNode::numberOfChildren() const {
  return ListSort::s_functionHelper.numberOfChildren();
}

size_t ListSortNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListSort::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListSortNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListSort(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListSortNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> child = childAtIndex(0)->approximate(T(), approximationContext);
  if (child.otype() != EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Undefined();
  }
  ListComplex<T> listChild = static_cast<ListComplex<T>&>(child);
  listChild.sort();
  return std::move(listChild);
}

OExpression ListSort::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.otype() != ExpressionNode::Type::OList) {
    if (!child.deepIsList(reductionContext.context())) {
      return replaceWithUndefinedInPlace();
    }
    return *this;
  }

  if (child.numberOfChildren() == 0 ||
      recursivelyMatches(OExpression::IsUndefined, nullptr)) {
    // Cannot sort if a child is the expression Undefined
    replaceWithInPlace(child);
    return child;
  }

  OList list = static_cast<OList&>(child);
  ApproximationContext approximationContext(reductionContext, true);
  Evaluation<float> approximatedList =
      list.approximateToEvaluation<float>(approximationContext);
  if (list.numberOfChildren() != approximatedList.numberOfChildren()) {
    return *this;
  }

  bool listOfDefinedScalars = approximatedList.isListOfDefinedScalars();
  bool listOfDefinedPoints = approximatedList.isListOfDefinedPoints();
  if (!listOfDefinedScalars && !listOfDefinedPoints) {
    return *this;
  }

#if 0
  Helpers::ListSortPack<float> pack{
      &list, static_cast<ListComplex<float>*>(&approximatedList),
      listOfDefinedScalars};
  Helpers::Sort(Helpers::SwapInList<float>, Helpers::CompareInList<float>,
                &pack, child.numberOfChildren());
#endif

  replaceWithInPlace(child);
  return child;
}

template Evaluation<float> ListSortNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListSortNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
