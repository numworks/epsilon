#include <poincare/layout.h>
#include <poincare/old/layout_cursor.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

LayoutNode::DeletionMethod
BracketPairLayoutNode::deletionMethodForCursorLeftOfChild(
    int childIndex) const {
  return StandardDeletionMethodForLayoutContainingArgument(childIndex, 0);
}

KDSize BracketPairLayoutNode::computeSize(KDFont::Size font) {
  KDSize childSize = childLayout()->layoutSize(font);
  KDCoordinate width = 2 * bracketWidth() + childSize.width();
  KDCoordinate height = Height(childSize.height(), minVerticalMargin());
  return KDSize(width, height);
}

KDCoordinate BracketPairLayoutNode::computeBaseline(KDFont::Size font) {
  KDCoordinate childHeight = childLayout()->layoutSize(font).height();
  KDCoordinate childBaseLine = childLayout()->baseline(font);
  return Baseline(childHeight, childBaseLine, minVerticalMargin());
}

KDPoint BracketPairLayoutNode::positionOfChild(LayoutNode* child,
                                               KDFont::Size font) {
  assert(childLayout() == child);
  KDCoordinate childHeight = childLayout()->layoutSize(font).height();

  return ChildOffset(minVerticalMargin(), bracketWidth(), childHeight);
}

void BracketPairLayoutNode::render(KDContext* ctx, KDPoint p,
                                   KDGlyph::Style style) {
  renderOneBracket(true, ctx, p, style);
  KDCoordinate rightBracketOffset =
      bracketWidth() + childLayout()->layoutSize(style.font).width();
  renderOneBracket(false, ctx, p.translatedBy(KDPoint(rightBracketOffset, 0)),
                   style);
}

size_t BracketPairLayoutNode::serializeWithSymbol(
    char symbolOpen, char symbolClose, char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  size_t length =
      SerializationHelper::CodePoint(buffer, bufferSize, symbolOpen);
  if (length >= bufferSize - 1) {
    return bufferSize - 1;
  }
  length +=
      childLayout()->serialize(buffer + length, bufferSize - length,
                               floatDisplayMode, numberOfSignificantDigits);
  if (length >= bufferSize) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize;
  }
  length += SerializationHelper::CodePoint(buffer + length, bufferSize - length,
                                           symbolClose);
  return std::min(length, bufferSize - 1);
}

}  // namespace Poincare
