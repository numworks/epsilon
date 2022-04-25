#include <poincare/list.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

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

Layout ListNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  Layout result;
  if (m_numberOfChildren > 1) {
    Layout elementsLayout = LayoutHelper::Infix(List(this), floatDisplayMode, numberOfSignificantDigits, ",");
    result = HorizontalLayout::Builder(LeftCurlyBraceLayout::Builder(), elementsLayout, RightCurlyBraceLayout::Builder());
    result.mergeChildrenAtIndexInPlace(elementsLayout, 1);
  } else if (m_numberOfChildren == 1) {
    result = HorizontalLayout::Builder(LeftCurlyBraceLayout::Builder(), childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), RightCurlyBraceLayout::Builder());
  } else {
    assert(m_numberOfChildren == 0);
    result = HorizontalLayout::Builder(LeftCurlyBraceLayout::Builder(), RightCurlyBraceLayout::Builder());
  }
  return result;
}

Expression ListNode::shallowReduce(ReductionContext reductionContext) {
  return List(this).shallowReduce(reductionContext.context());
}

template<typename T> Evaluation<T> ListNode::templatedApproximate(ApproximationContext approximationContext) const {
  return Complex<T>::Undefined();
}

// List

Expression List::shallowReduce(Context * context) {
  // A list can't contain a matrix or a list
  if (node()->hasMatrixChild(context) || node()->hasListChild(context)) {
    return replaceWithUndefinedInPlace();
  }
  /* We bypass the reduction to undef in case of undef children, as {undef} and
   * undef are different objects. */
  return *this;
}

template Evaluation<float> ListNode::templatedApproximate(ApproximationContext approximationContext) const;
template Evaluation<double> ListNode::templatedApproximate(ApproximationContext approximationContext) const;
}
