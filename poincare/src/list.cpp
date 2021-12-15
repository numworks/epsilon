#include <poincare/list.h>
#include <poincare/code_point_layout.h>
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
    result = HorizontalLayout::Builder(CodePointLayout::Builder('{'), elementsLayout, CodePointLayout::Builder('}'));
    result.mergeChildrenAtIndexInPlace(elementsLayout, 1);
  } else if (m_numberOfChildren == 1) {
    result = HorizontalLayout::Builder(CodePointLayout::Builder('{'), childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), CodePointLayout::Builder('}'));
  } else {
    assert(m_numberOfChildren == 0);
    result = HorizontalLayout::Builder(CodePointLayout::Builder('{'), CodePointLayout::Builder('}'));
  }
  return result;
}

Expression ListNode::shallowReduce(ReductionContext reductionContext) {
  /* We bypass the reduction to undef in case of undef children, as {undef} and
   * undef are different objects. */
  return List(this);
}

template<typename T> Evaluation<T> ListNode::templatedApproximate(ApproximationContext approximationContext) const {
  /* TODO */
  return Complex<T>::Undefined();
}

// List

template Evaluation<float> ListNode::templatedApproximate(ApproximationContext approximationContext) const;
template Evaluation<double> ListNode::templatedApproximate(ApproximationContext approximationContext) const;
}
