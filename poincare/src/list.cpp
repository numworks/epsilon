#include <poincare/addition.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/dependency.h>
#include <poincare/helpers.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/list_complex.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>

#include "poincare/symbol_abstract.h"

namespace Poincare {

// ListNode

size_t ListNode::serialize(char* buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  size_t writtenChars = SerializationHelper::CodePoint(buffer, bufferSize, '{');
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  if (m_numberOfChildren > 0) {
    writtenChars += SerializationHelper::Infix(
        this, buffer + writtenChars, bufferSize - writtenChars,
        floatDisplayMode, numberOfSignificantDigits, ",");
  }
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  writtenChars += SerializationHelper::CodePoint(
      buffer + writtenChars, bufferSize - writtenChars, '}');
  return writtenChars;
}

OLayout ListNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits,
                               Context* context) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  if (m_numberOfChildren > 1) {
    Layout elementsLayout = LayoutHelper::Infix(
        List(this), floatDisplayMode, numberOfSignificantDigits, ",", context);
    result.addOrMergeChildAtIndex(elementsLayout, 0);
  } else if (m_numberOfChildren == 1) {
    result.addOrMergeChildAtIndex(
        childAtIndex(0)->createLayout(floatDisplayMode,
                                      numberOfSignificantDigits, context),
        0);
  } else if (m_numberOfChildren == 0) {
    return CurlyBraceLayout::Builder();
  }
  return CurlyBraceLayout::Builder(result);
}

// Helper functions
int ListNode::extremumIndex(const ApproximationContext& approximationContext,
                            bool minimum) {
  int numberOfElements = numberOfChildren();
  float currentExtremumValue = NAN;
  int returnIndex = -1;
  for (int i = 0; i < numberOfElements; i++) {
    ExpressionNode* child = childAtIndex(i);
    float newValue =
        child->approximate(static_cast<float>(0), approximationContext)
            .toScalar();
    if (std::isnan(newValue)) {
      return -1;
    }
    assert(!std::isnan(currentExtremumValue) || returnIndex < 0);
    if (returnIndex < 0 || (minimum && newValue < currentExtremumValue) ||
        (!minimum && newValue > currentExtremumValue)) {
      returnIndex = i;
      currentExtremumValue = newValue;
    }
  }
  return returnIndex;
}

template <typename T>
Evaluation<T> ListNode::extremumApproximation(
    const ApproximationContext& approximationContext, bool minimum) {
  int index = extremumIndex(approximationContext, minimum);
  if (index < 0) {
    return Complex<T>::Undefined();
  }
  return childAtIndex(index)->approximate(static_cast<T>(0),
                                          approximationContext);
}

template <typename T>
Evaluation<T> ListNode::sumOfElements(
    const ApproximationContext& approximationContext) {
  if (numberOfChildren() == 0) {
    return Complex<T>::Builder(0.0);
  }
  return ApproximationHelper::MapReduce<T>(
      this, approximationContext,
      [](Evaluation<T> eval1, Evaluation<T> eval2,
         Preferences::ComplexFormat complexFormat) {
        return ApproximationHelper::Reduce<T>(
            eval1, eval2, complexFormat, AdditionNode::computeOnComplex<T>,
            ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
            ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
            ApproximationHelper::UndefinedOnMatrixAndMatrix<T>);
      });
}

template <typename T>
Evaluation<T> ListNode::productOfElements(
    const ApproximationContext& approximationContext) {
  if (numberOfChildren() == 0) {
    return Complex<T>::Builder(1.0);
  }
  return ApproximationHelper::MapReduce<T>(
      this, approximationContext,
      [](Evaluation<T> eval1, Evaluation<T> eval2,
         Preferences::ComplexFormat complexFormat) {
        return ApproximationHelper::Reduce<T>(
            eval1, eval2, complexFormat,
            MultiplicationNode::computeOnComplex<T>,
            ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
            ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
            ApproximationHelper::UndefinedOnMatrixAndMatrix<T>);
      });
}

Expression ListNode::shallowReduce(const ReductionContext& reductionContext) {
  return List(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListNode::templatedApproximate(
    const ApproximationContext& approximationContext, bool keepUndef) const {
  ListComplex<T> list = ListComplex<T>::Builder();
  for (ExpressionNode* c : children()) {
    Evaluation<T> eval = c->approximate(T(), approximationContext);
    if (keepUndef || !eval.isUndefined()) {
      int n = list.numberOfChildren();
      list.addChildAtIndexInPlace(eval, n, n);
    }
  }
  return std::move(list);
}

// List

Expression List::Ones(int length) {
  List result = List::Builder();
  for (int i = 0; i < length; i++) {
    result.addChildAtIndexInPlace(Rational::Builder(1), i, i);
  }
  return std::move(result);
}

Expression List::shallowReduce(ReductionContext reductionContext) {
  Expression myParent = parent();
  bool isDependenciesList =
      !myParent.isUninitialized() &&
      myParent.type() == ExpressionNode::Type::Dependency &&
      myParent.indexOfChild(*this) == Dependency::k_indexOfDependenciesList;

  // A list can't contain a matrix or a list
  if (!isDependenciesList &&
      node()->hasMatrixOrListChild(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }

  /* We do not reduce a list to undef when a child is undef because undef and
   * {undef} are different objects. */
  SimplificationHelper::UndefReduction undefReduction =
      isDependenciesList
          ? SimplificationHelper::UndefReduction::BubbleUpUndef
          : SimplificationHelper::UndefReduction::DoNotBubbleUpUndef;

  Expression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::DefinedOnBooleans,
      SimplificationHelper::UnitReduction::KeepUnits,
      SimplificationHelper::MatrixReduction::DefinedOnMatrix,
      SimplificationHelper::ListReduction::DoNotDistributeOverLists,
      SimplificationHelper::PointReduction::DefinedOnPoint, undefReduction);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

Expression List::extremum(const ReductionContext& reductionContext,
                          bool minimum) {
  const ApproximationContext approximationContext(reductionContext, true);
  int extremumIndex = node()->extremumIndex(approximationContext, minimum);
  if (extremumIndex < 0) {
    return Undefined::Builder();
  }
  return childAtIndex(extremumIndex);
}

bool List::isListOfPoints(Context* context) const {
  int n = numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression e = childAtIndex(i);
    if (IsSymbolic(e)) {
      if (!context) {
        return false;
      }
      Expression ve = context->expressionForSymbolAbstract(
          static_cast<SymbolAbstract&>(e), false);
      if (!(ve.isUninitialized() || IsPoint(ve))) {
        return false;
      }
    } else if (!IsPoint(e)) {
      return false;
    }
  }
  return true;
}

template <typename T>
Expression List::approximateAndRemoveUndefAndSort(
    const ApproximationContext& approximationContext) const {
  if (isUninitialized()) {
    return Undefined::Builder();
  }
  Evaluation<T> eval =
      node()->templatedApproximate<T>(approximationContext, false);
  if (eval.type() != EvaluationNode<T>::Type::ListComplex) {
    return Undefined::Builder();
  }
  static_cast<ListComplex<T>&>(eval).sort();
  return eval.complexToExpression(approximationContext.complexFormat());
}

template Evaluation<float> ListNode::templatedApproximate(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListNode::templatedApproximate(
    const ApproximationContext& approximationContext) const;

template Evaluation<float> ListNode::extremumApproximation<float>(
    const ApproximationContext& approximationContext, bool minimum);
template Evaluation<double> ListNode::extremumApproximation<double>(
    const ApproximationContext& approximationContext, bool minimum);

template Evaluation<float> ListNode::sumOfElements<float>(
    const ApproximationContext& approximationContext);
template Evaluation<double> ListNode::sumOfElements<double>(
    const ApproximationContext& approximationContext);

template Evaluation<float> ListNode::productOfElements<float>(
    const ApproximationContext& approximationContext);
template Evaluation<double> ListNode::productOfElements<double>(
    const ApproximationContext& approximationContext);

template Expression List::approximateAndRemoveUndefAndSort<float>(
    const ApproximationContext& approximationContext) const;
template Expression List::approximateAndRemoveUndefAndSort<double>(
    const ApproximationContext& approximationContext) const;
}  // namespace Poincare
