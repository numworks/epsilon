#include <poincare/list.h>
#include <poincare/addition.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/dependency.h>
#include <poincare/helpers.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/list_complex.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

// ListNode

int ListNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int writtenChars = SerializationHelper::CodePoint(buffer, bufferSize, '{');
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  if (m_numberOfChildren > 0) {
    writtenChars += SerializationHelper::Infix(this, buffer + writtenChars, bufferSize - writtenChars, floatDisplayMode, numberOfSignificantDigits, ",");
  }
  if (writtenChars >= bufferSize - 1) {
    return writtenChars;
  }
  writtenChars += SerializationHelper::CodePoint(buffer + writtenChars, bufferSize - writtenChars, '}');
  return writtenChars;
}

Layout ListNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  HorizontalLayout result = HorizontalLayout::Builder(LeftCurlyBraceLayout::Builder(), RightCurlyBraceLayout::Builder());
  if (m_numberOfChildren > 1) {
    Layout elementsLayout = LayoutHelper::Infix(List(this), floatDisplayMode, numberOfSignificantDigits, ",", context);
    result.addOrMergeChildAtIndex(elementsLayout, 1, true);
  } else if (m_numberOfChildren == 1) {
    result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), 1, true);
  }
  return std::move(result);
}

// Helper functions
int ListNode::extremumIndex(const ApproximationContext& approximationContext, bool minimum) {
  void * comparisonContext[] = {this, const_cast<ApproximationContext*>(&approximationContext), &minimum};
  return Poincare::Helpers::ExtremumIndex(
      Helpers::ListEvaluationComparisonAtIndex,
      comparisonContext,
      numberOfChildren(),
      minimum);
}

template <typename T> Evaluation<T> ListNode::extremumApproximation(const ApproximationContext& approximationContext, bool minimum) {
  if (numberOfChildren() == 0) {
    return Complex<T>::Undefined();
  }
  return childAtIndex(extremumIndex(approximationContext, minimum))->approximate(static_cast<T>(0), approximationContext);
}

template<typename T> Evaluation<T> ListNode::sumOfElements(const ApproximationContext& approximationContext) {
  if (numberOfChildren() == 0) {
    return Complex<T>::Builder(0.0);
  }
  return ApproximationHelper::MapReduce<T>(
      this,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          complexFormat,
          AdditionNode::computeOnComplex<T>,
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
}

template<typename T> Evaluation<T> ListNode::productOfElements(const ApproximationContext& approximationContext) {
  if (numberOfChildren() == 0) {
    return Complex<T>::Builder(1.0);
  }
  return ApproximationHelper::MapReduce<T>(
      this,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          complexFormat,
          MultiplicationNode::computeOnComplex<T>,
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
}

Expression ListNode::shallowReduce(const ReductionContext& reductionContext) {
  return List(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> list = ListComplex<T>::Builder();
  for (ExpressionNode * c : children()) {
    list.addChildAtIndexInPlace(c->approximate(T(), approximationContext), list.numberOfChildren(), list.numberOfChildren());
  }
  return std::move(list);
}

// List

Expression List::Ones(int length) {
  List result = List::Builder();
  for (int i = 0; i < length; i++) {
    result.addChildAtIndexInPlace(Rational::Builder(1), i, i);
  }
  return result;
}


Expression List::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  // A list can't contain a matrix or a list
  if (node()->hasMatrixOrListChild(reductionContext.context())) {
    Expression myParent = parent();
    bool isDependenciesList = !myParent.isUninitialized() && myParent.type() == ExpressionNode::Type::Dependency && myParent.indexOfChild(*this) == Dependency::k_indexOfDependenciesList;
    // If it's a list of dependencies, it can contain matrices and lists.
    if (!isDependenciesList) {
      return replaceWithUndefinedInPlace();
    }
  }
  /* We bypass the reduction to undef in case of undef children, as {undef} and
   * undef are different objects. */
  return *this;
}

Expression List::extremum(const ExpressionNode::ReductionContext& reductionContext, bool minimum) {
  if (numberOfChildren() == 0) {
    return Undefined::Builder();
  }
  const ExpressionNode::ApproximationContext approximationContext(reductionContext, true);
  return childAtIndex(node()->extremumIndex(approximationContext, minimum));
}

template Evaluation<float> ListNode::templatedApproximate(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListNode::templatedApproximate(const ApproximationContext& approximationContext) const;

template Evaluation<float> ListNode::extremumApproximation<float>(const ApproximationContext& approximationContext, bool minimum);
template Evaluation<double> ListNode::extremumApproximation<double>(const ApproximationContext& approximationContext, bool minimum);

template Evaluation<float> ListNode::sumOfElements<float>(const ApproximationContext& approximationContext);
template Evaluation<double> ListNode::sumOfElements<double>(const ApproximationContext& approximationContext);

template Evaluation<float> ListNode::productOfElements<float>(const ApproximationContext& approximationContext);
template Evaluation<double> ListNode::productOfElements<double>(const ApproximationContext& approximationContext);
}
