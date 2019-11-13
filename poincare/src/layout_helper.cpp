#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/left_parenthesis_layout.h>
#include <poincare/right_parenthesis_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <assert.h>
#include <utility>

namespace Poincare {

Layout LayoutHelper::Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
  HorizontalLayout result = HorizontalLayout::Builder();
  const size_t operatorLength = strlen(operatorName);
  const int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);
  for (int i = 0; i < numberOfChildren; i++) {
    if (i > 0 && operatorLength > 0) {
      result.addOrMergeChildAtIndex(String(operatorName, operatorLength), result.numberOfChildren(), true);
    }
    result.addOrMergeChildAtIndex(
        expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits),
        result.numberOfChildren(),
        true);
  }
  return std::move(result);
}

Layout LayoutHelper::Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName) {
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
    args.addOrMergeChildAtIndex(expression.childAtIndex(i).createLayout(floatDisplayMode, numberOfSignificantDigits), args.numberOfChildren(), true);
  }
  // Add the parenthesed arguments.
  result.addOrMergeChildAtIndex(Parentheses(args, false), result.numberOfChildren(), true);
  return std::move(result);
}

Layout LayoutHelper::Parentheses(Layout layout, bool cloneLayout) {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addChildAtIndex(LeftParenthesisLayout::Builder(), 0, 0, nullptr);
  if (!layout.isUninitialized()) {
    result.addOrMergeChildAtIndex(cloneLayout ? layout.clone() : layout, 1, true);
  }
  result.addChildAtIndex(RightParenthesisLayout::Builder(), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  return std::move(result);
}

Layout LayoutHelper::String(const char * buffer, int bufferLen, const KDFont * font) {
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
    resultLayout.addChildAtIndex(CodePointLayout::Builder(codePoint, font), layoutIndex, layoutIndex, nullptr);
    layoutIndex++;
    bufferIndex+= nextPointer - currentPointer;
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
    while (codePoint.isCombining()) {
      bufferIndex+= nextPointer - currentPointer;
      currentPointer = nextPointer;
      codePoint = decoder.nextCodePoint();
      nextPointer = decoder.stringPosition();
    }
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::CodePointString(const CodePoint * buffer, int bufferLen, const KDFont * font) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  for (int i = 0; i < bufferLen; i++) {
    resultLayout.addChildAtIndex(CodePointLayout::Builder(buffer[i], font), i, i, nullptr);
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::Logarithm(Layout argument, Layout index) {
  Layout logLayout = String("log", 3);
  HorizontalLayout resultLayout = static_cast<HorizontalLayout &>(logLayout);
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::Position::Subscript);
  resultLayout.addChildAtIndex(offsetLayout, resultLayout.numberOfChildren(), resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return std::move(resultLayout);
}

}
