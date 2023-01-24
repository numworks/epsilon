#include <poincare/layout_helper.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/logarithm.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/string_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <assert.h>
#include <utility>

namespace Poincare {

Layout LayoutHelper::DefaultCreateOperatorLayoutForInfix(const char * operatorName, Expression left, Expression right, Layout rightLayout) {
  rightLayout.setMargin(true);
  size_t operatorLength = strlen(operatorName);
  if (operatorLength == 0) {
    return Layout();
  }
  Layout result = String(operatorName, operatorLength);
  result.setMargin(true);
  return result;
}

Layout LayoutHelper::Infix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName, Context * context, OperatorLayoutForInfix operatorLayoutBuilder) {
  HorizontalLayout result = HorizontalLayout::Builder();
  const int numberOfChildren = expression.numberOfChildren();
  assert(numberOfChildren > 1);

  Expression leftChild;
  Expression rightChild;

  for (int i = 0; i < numberOfChildren; i++) {
    rightChild = expression.childAtIndex(i);
    Layout childLayout = rightChild.createLayout(floatDisplayMode, numberOfSignificantDigits, context, false, true);

    if (i > 0) {
      /* Handle the operator */
      assert(!leftChild.isUninitialized() && !rightChild.isUninitialized());
      Layout operatorLayout = operatorLayoutBuilder(operatorName, leftChild, rightChild, childLayout);
      if (!operatorLayout.isUninitialized()) {
        result.addOrMergeChildAtIndex(operatorLayout, result.numberOfChildren(), true);
      }
    }

    result.addOrMergeChildAtIndex(childLayout, result.numberOfChildren(), true);
    leftChild = rightChild;
  }
  return std::move(result);
}

Layout LayoutHelper::Prefix(const Expression & expression, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, const char * operatorName, Context * context, bool addParenthesese) {
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
  Layout argsResult = args;
  if (addParenthesese) {
    // Add the parenthesed arguments.
    argsResult = Parentheses(args, false);
  }
  result.addOrMergeChildAtIndex(argsResult, result.numberOfChildren(), true);
  return std::move(result);
}

Layout LayoutHelper::Parentheses(Layout layout, bool cloneLayout) {
  if (layout.isUninitialized() || layout.isEmpty() || (layout.type() == LayoutNode::Type::HorizontalLayout && layout.numberOfChildren() == 0)) {
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
  constexpr const char * k_logName = LogarithmNode::k_functionName;
  constexpr int k_logNameLength = Helpers::StringLength(k_logName);
  Layout logLayout = String(k_logName, k_logNameLength);
  assert(logLayout.type() != LayoutNode::Type::HorizontalLayout);
  HorizontalLayout resultLayout = HorizontalLayout::Builder(logLayout);

  int baseIndex;
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::VerticalPosition::Subscript);
  if (Preferences::sharedPreferences->logarithmBasePosition() == Preferences::LogarithmBasePosition::TopLeft) {
    baseIndex = 0;
    offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::VerticalPosition::Superscript, VerticalOffsetLayoutNode::HorizontalPosition::Prefix);
  } else {
    baseIndex = resultLayout.numberOfChildren();
    offsetLayout = VerticalOffsetLayout::Builder(index, VerticalOffsetLayoutNode::VerticalPosition::Subscript, VerticalOffsetLayoutNode::HorizontalPosition::Suffix);
  }

  resultLayout.addChildAtIndex(offsetLayout, baseIndex, resultLayout.numberOfChildren(), nullptr);
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false), resultLayout.numberOfChildren(), true);
  return std::move(resultLayout);
}

HorizontalLayout LayoutHelper::CodePointSubscriptCodePointLayout(CodePoint base, CodePoint subscript) {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(base),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder(subscript), VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

}
