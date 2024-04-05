#include <escher/metric.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

// LayoutNode

size_t CombinedCodePointsLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  size_t numberOfChar =
      SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar, m_combinedCodePoint);
  return numberOfChar;
}

void CombinedCodePointsLayoutNode::render(KDContext *ctx, KDPoint p,
                                          KDGlyph::Style style) {
  // Null-terminating char
  constexpr int bufferSize = 2 * sizeof(CodePoint) / sizeof(char) + 1;
  char buffer[bufferSize];
  serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, 0);
  ctx->drawString(buffer, p, style);
}

bool CombinedCodePointsLayoutNode::protectedIsIdenticalTo(OLayout l) {
  assert(l.otype() == Type::CombinedCodePointsLayout);
  CombinedCodePointsLayout &cpl = static_cast<CombinedCodePointsLayout &>(l);
  return combinedCodePoint() == cpl.combinedCodePoint() &&
         CodePointLayoutNode::protectedIsIdenticalTo(l);
}

CombinedCodePointsLayout CombinedCodePointsLayout::Builder(
    CodePoint mainCodePoint, CodePoint CombinedCodePoints) {
  void *bufferNode =
      Pool::sharedPool->alloc(sizeof(CombinedCodePointsLayoutNode));
  CombinedCodePointsLayoutNode *node = new (bufferNode)
      CombinedCodePointsLayoutNode(mainCodePoint, CombinedCodePoints);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<CombinedCodePointsLayout &>(h);
}

}  // namespace Poincare
