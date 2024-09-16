#include <assert.h>
#include <omg/utf8_decoder.h>
#include <omg/utf8_helper.h>
#include <poincare/layout.h>
#include <poincare/old/logarithm.h>

#include <utility>

namespace Poincare {

#define Layout OLayout

Layout LayoutHelper::DefaultCreateOperatorLayoutForInfix(
    const char* operatorName, OExpression left, OExpression right,
    Layout rightLayout) {
  size_t operatorLength = strlen(operatorName);
  if (operatorLength == 0) {
    return Layout();
  }
  Layout result = String(operatorName, operatorLength);
  return result;
}

Layout LayoutHelper::Parentheses(Layout layout, bool cloneLayout) {
  if (layout.isUninitialized()) {
    return ParenthesisLayout::Builder();
  }
  return ParenthesisLayout::Builder(cloneLayout ? layout.clone() : layout);
}

Layout LayoutHelper::String(const char* buffer, int bufferLen) {
  if (bufferLen < 0) {
    bufferLen = strlen(buffer);
  }
  return UTF8Helper::StringGlyphLength(buffer) <= 1
             ? StringToCodePointsLayout(buffer, bufferLen)
             : StringToStringLayout(buffer, bufferLen);
}

Layout LayoutHelper::StringLayoutOfSerialization(
    const OExpression& expression, char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) {
  size_t length = expression.serialize(buffer, bufferSize, floatDisplayMode,
                                       numberOfSignificantDigits);
  assert(length < bufferSize);
  return LayoutHelper::String(buffer, length);
}

Layout LayoutHelper::StringToCodePointsLayout(const char* buffer,
                                              int bufferLen) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  UTF8Decoder decoder(buffer);
  const char* currentPointer = buffer;
  CodePoint codePoint = decoder.nextCodePoint();
  const char* nextPointer = decoder.stringPosition();
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
    resultLayout.addChildAtIndexInPlace(nextChild, layoutIndex, layoutIndex);
    layoutIndex++;
    bufferIndex += nextPointer - currentPointer;
    currentPointer = nextPointer;
    codePoint = nextCodePoint;
    nextPointer = decoder.stringPosition();
    assert(!codePoint.isCombining());
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::StringToStringLayout(const char* buffer, int bufferLen) {
  assert(bufferLen > 0);
  /* MiddleDot MUST be handled as code point.
   * If you hit this assertion, use the method 'StringToCodePointsLayout'
   * to turn your string into an haroizontal layout filled with code points.*/
  assert(!UTF8Helper::HasCodePoint(buffer, UCodePointMiddleDot));
  return StringLayout::Builder(buffer, bufferLen + 1);
}

Layout LayoutHelper::CodePointsToLayout(const CodePoint* buffer,
                                        int bufferLen) {
  assert(bufferLen > 0);
  HorizontalLayout resultLayout = HorizontalLayout::Builder();
  for (int i = 0; i < bufferLen; i++) {
    assert(!buffer[i].isCombining());
    // TODO support combining code point?
    resultLayout.addChildAtIndexInPlace(CodePointLayout::Builder(buffer[i]), i,
                                        i);
  }
  return resultLayout.squashUnaryHierarchyInPlace();
}

Layout LayoutHelper::Logarithm(Layout argument, Layout index) {
  constexpr const char* k_logName = LogarithmNode::k_functionName;
  constexpr int k_logNameLength = Helpers::StringLength(k_logName);
  Layout logLayout = String(k_logName, k_logNameLength);
  assert(!logLayout.isHorizontal());
  HorizontalLayout resultLayout = HorizontalLayout::Builder(logLayout);

  int baseIndex;
  VerticalOffsetLayout offsetLayout = VerticalOffsetLayout::Builder(
      index, VerticalOffsetLayoutNode::VerticalPosition::Subscript);
  if (Preferences::SharedPreferences()->logarithmBasePosition() ==
      Preferences::LogarithmBasePosition::TopLeft) {
    baseIndex = 0;
    offsetLayout = VerticalOffsetLayout::Builder(
        index, VerticalOffsetLayoutNode::VerticalPosition::Superscript,
        VerticalOffsetLayoutNode::HorizontalPosition::Prefix);
  } else {
    baseIndex = resultLayout.numberOfChildren();
    offsetLayout = VerticalOffsetLayout::Builder(
        index, VerticalOffsetLayoutNode::VerticalPosition::Subscript,
        VerticalOffsetLayoutNode::HorizontalPosition::Suffix);
  }

  resultLayout.addChildAtIndexInPlace(offsetLayout, baseIndex,
                                      resultLayout.numberOfChildren());
  resultLayout.addOrMergeChildAtIndex(Parentheses(argument, false),
                                      resultLayout.numberOfChildren());
  return std::move(resultLayout);
}

HorizontalLayout LayoutHelper::CodePointSubscriptCodePointLayout(
    CodePoint base, CodePoint subscript) {
  return HorizontalLayout::Builder(
      CodePointLayout::Builder(base),
      VerticalOffsetLayout::Builder(
          CodePointLayout::Builder(subscript),
          VerticalOffsetLayoutNode::VerticalPosition::Subscript));
}

}  // namespace Poincare
