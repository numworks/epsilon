#include <poincare/list_access.h>
#include <poincare/code_point_layout.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/list_complex.h>
#include <poincare/rational.h>

namespace Poincare {

template<>
ExpressionNode::Type ListAccessNode<1>::type() const {
  return Type::ListElement;
}

template<>
ExpressionNode::Type ListAccessNode<2>::type() const {
  return Type::ListSlice;
}

template<>
Layout ListAccessNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false), 0, true);
  result.addOrMergeChildAtIndex(childAtIndex(k_listChildIndex)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
  return std::move(result);
}

template<>
Layout ListAccessNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout parameters = HorizontalLayout::Builder();
  parameters.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
  parameters.addOrMergeChildAtIndex(CodePointLayout::Builder(','), 0, true);
  parameters.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);

  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(LayoutHelper::Parentheses(parameters, false), 0, true);
  result.addOrMergeChildAtIndex(childAtIndex(k_listChildIndex)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, true);
  return std::move(result);
}

template<>
Expression ListAccessNode<1>::shallowReduce(const ReductionContext& reductionContext) {
  return ListElement(this).shallowReduce(reductionContext);
}

template<>
Expression ListAccessNode<2>::shallowReduce(const ReductionContext& reductionContext) {
  return ListSlice(this).shallowReduce(reductionContext);
}

template<>
template<typename T> Evaluation<T> ListAccessNode<1>::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> child = childAtIndex(ListAccessNode<1>::k_listChildIndex)->approximate(T(), approximationContext);
  if (child.type() != EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Undefined();
  }
  ListComplex<T> listChild = static_cast<ListComplex<T>&>(child);

  T indexChild = childAtIndex(0)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(indexChild) || static_cast<int>(indexChild) != indexChild) {
    return Complex<T>::Undefined();
  }
  int indexInt = static_cast<int>(indexChild);
  if (indexInt < 1 || indexInt > listChild.numberOfChildren()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(listChild.complexAtIndex(indexInt - 1));
}

template<>
template<typename T> Evaluation<T> ListAccessNode<2>::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> child = childAtIndex(ListAccessNode<2>::k_listChildIndex)->approximate(T(), approximationContext);
  if (child.type() != EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Undefined();
  }
  ListComplex<T> listChild = static_cast<ListComplex<T>&>(child);

  T startIndex = childAtIndex(0)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(startIndex) || static_cast<int>(startIndex) != startIndex) {
    return Complex<T>::Undefined();
  }
  T endIndex = childAtIndex(1)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(endIndex) || static_cast<int>(endIndex) != endIndex) {
    return Complex<T>::Undefined();
  }
  int startInt = static_cast<int>(startIndex);
  int endInt = static_cast<int>(endIndex);
  ListComplex<T> returnList = ListComplex<T>::Builder();
  for (int i = startInt - 1; i < endInt; i++) {
    if (i >= listChild.numberOfChildren()) {
      break;
    }
    if (i < 0) {
      continue;
    }
    returnList.addChildAtIndexInPlace(Complex<T>::Builder(listChild.complexAtIndex(i)), returnList.numberOfChildren(), returnList.numberOfChildren());
  }
  return std::move(returnList);
}

Expression ListElement::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression indexChild = childAtIndex(0);
  Expression listChild = childAtIndex(ListAccessNode<1>::k_listChildIndex);

  if (listChild.type() != ExpressionNode::Type::List || indexChild.type() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }

  Rational rationalIndex = static_cast<Rational &>(indexChild);
  Integer integerIndex = rationalIndex.signedIntegerNumerator();
  if (!rationalIndex.isInteger() || !integerIndex.isExtractable()) {
    return replaceWithUndefinedInPlace();
  }

  int index = integerIndex.extractedInt() - 1; // List index starts at 1
  if (index < 0 || index >= listChild.numberOfChildren()) {
    return replaceWithUndefinedInPlace();
  }

  Expression element = listChild.childAtIndex(index);
  replaceWithInPlace(element);
  return element;
}

Expression ListSlice::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression firstIndexChild = childAtIndex(0);
  Expression lastIndexChild = childAtIndex(1);
  Expression listChild = childAtIndex(ListAccessNode<2>::k_listChildIndex);
  int listNumberOfChildren = listChild.numberOfChildren();

  if (listChild.type() != ExpressionNode::Type::List || firstIndexChild.type() != ExpressionNode::Type::Rational || lastIndexChild.type() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }

  Rational firstRationalIndex = static_cast<Rational &>(firstIndexChild);
  Rational lastRationalIndex = static_cast<Rational &>(lastIndexChild);
  Integer firstIntegerIndex = firstRationalIndex.signedIntegerNumerator();
  Integer lastIntegerIndex = lastRationalIndex.signedIntegerNumerator();
  if (!firstRationalIndex.isInteger() || !lastRationalIndex.isInteger() || !firstIntegerIndex.isExtractable() || !lastIntegerIndex.isExtractable()) {
    return replaceWithUndefinedInPlace();
  }

  int firstIndex = firstIntegerIndex.extractedInt() - 1;
  int lastIndex = lastIntegerIndex.extractedInt() - 1;
  List typedList = static_cast<List &>(listChild);

  if (lastIndex < -1) {
    lastIndex = -1;
  }
  for (int i = listNumberOfChildren - 1; i > lastIndex; i--) {
    typedList.removeChildAtIndexInPlace(i);
  }

  listNumberOfChildren = typedList.numberOfChildren();
  if (firstIndex > listNumberOfChildren) {
    firstIndex = listNumberOfChildren;
  }
  for (int i = firstIndex - 1; i >= 0; i--) {
    typedList.removeChildAtIndexInPlace(i);
  }

  replaceChildAtIndexWithGhostInPlace(ListAccessNode<2>::k_listChildIndex);
  replaceWithInPlace(typedList);
  return std::move(typedList);
}

template Evaluation<float> ListAccessNode<1>::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<float> ListAccessNode<2>::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListAccessNode<1>::templatedApproximate<double>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListAccessNode<2>::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
