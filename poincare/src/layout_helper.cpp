#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/string_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <assert.h>
#include <utility>

namespace Poincare {

Layout LayoutHelper::Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName, Context * context, OperatorTest forbidOperator) {
  HorizontalLayout result = HorizontalLayout::Builder();
  const size_t operatorLength = strlen(operatorName);
  const int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);
  for (int i = 0; i < numberOfChildren; i++) {
    Layout childLayout = expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits, context, false, true);

    if (i > 0) {
      /* Handle the operator */
      if (operatorLength > 0
       && !(forbidOperator && forbidOperator(expression.childAtIndex(i - 1), expression.childAtIndex(i))))
      {
        Layout operatorLayout = String(operatorName, operatorLength);
        assert(operatorLayout.type() == LayoutNode::Type::CodePointLayout);
        operatorLayout.setMargin(true);
        result.addOrMergeChildAtIndex(operatorLayout, result.numberOfChildren(), true);
      }
      childLayout.setMargin(true);
    }
    result.addOrMergeChildAtIndex(childLayout, result.numberOfChildren(), true);
  }
  return std::move(result);
}

Layout LayoutHelper::Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName, Context * context) {
  HorizontalLayout result = HorizontalLayout::Builder();
  // Add the operator name.
  result.addOrMergeChildAtIndex(String(operatorName, strlen(operatorName)), 0, true);

  // Create the layout of arguments separated by commas.
  HorizontalLayout args = HorizontalLayout::Builder();
  const int numberOfChildren = expression.numberOfChildren();
  for (int i = 0; i < numberOfChildren; i++) {
    if (i > 0) {
      args.addChildAtIndex(CodePointLayout::Builder(','), args.numberOfChildren(), args.numberOfChildren(), nullptr);
    }
    args.addOrMergeChildAtIndex(expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits, context, false, true), args.numberOfChildren(), true);
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(Parentheses(args, false), result.numberOfChildren(), true);
  return std::move(result);
}

Layout LayoutHelper::Parentheses(Layout layout, bool cloneLayout) {
  if (layout.isUninitialized()) {
    return ParenthesisLayout::Builder();
  }
  return ParenthesisLayout::Builder(cloneLayout ? layout.clone() : layout);
}

Layout LayoutHelper::String(const char * buffer, int bufferLen) {
  if (bufferLen < 0) {
    bufferLen = strlen(buffer);
  }
  return UTF8Helper::StringGlyphLength(buffer) <= 1 ? StringToCodePointsLayout(buffer, bufferLen) : StringToStringLayout(buffer, bufferLen);
}

Layout LayoutHelper::StringLayoutOfSerialization(const Expression & expression, char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) {
  int length = expression.serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  assert(length < bufferSize);
  return LayoutHelper::String(buffer, length);
}

Layout LayoutHelper::StringToCodePointsLayout(const char * buffer, int bufferLen) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  UTF8Decoder decoder(buffer);
  const char * currentPointer = buffer;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  assert(!codePoint.isCombining());
  int layoutIndex = 0;
  int bufferIndex = 0;
  while (codePoint != UCodePointNull && bufferIndex < bufferLen) {
    CodePoint nextCodePoint = decoder.nextCodePoint();
    Layout nextChild;
    if (nextCodePoint.isCombining()) {
      nextChild = CombinedCodePointsLayout::Builder(codePoint, nextCodePoint);
      nextPointer = decoder.stringPosition();
      nextCodePoint = decoder.nextCodePoint();
    } else {
      nextChild = CodePointLayout::Builder(codePoint);
    }
    resultLayout.addChildAtIndex(nextChild, layoutIndex, layoutIndex, nullptr);
    layoutIndex++;
    bufferIndex += nextPointer - currentPointer;
    currentPointer = nextPointer;
    codePoint = nextCodePoint;
    nextPointer = decoder.stringPosition();
    assert(!codePoint.isCombining());
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::StringToStringLayout(const char * buffer, int bufferLen) {
  assert(bufferLen > 0);
  /* MiddleDot MUST be handled as code point.
   * If you hit this assertion, use the method 'StringToCodePointsLayout'
   * to turn your string into an haroizontal layout filled with code points.*/
  assert(!UTF8Helper::HasCodePoint(buffer, UCodePointMiddleDot));
  return StringLayout::Builder(buffer, bufferLen + 1);
}

Layout LayoutHelper::CodePointsToLayout(const CodePoint * buffer, int bufferLen) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  for (int i = 0; i < bufferLen; i++) {
    assert(!buffer[i].isCombining());
    // TODO support combining code point?
    resultLayout.addChildAtIndex(CodePointLayout::Builder(buffer[i]), i, i, nullptr);
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::Logarithm(Layout argument, Layout index) {
  Layout logLayout = String("log", 3);
  assert(logLayout.type() != LayoutNode::Type::HorizontalLayout);
  HorizontalLayout resultLayout = HorizontalLayout::Builder(logLayout);
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::Position::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return std::move(resultLayout);
}

HorizontalLayout LayoutHelper::CodePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript) {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(base),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(subscript), VerticalOffsetLayoutNode::Position::Subscript));
}

}
