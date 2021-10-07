#include <poincare/combined_code_point_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>

namespace Poincare {

// LayoutNode

int CombinedCodePointLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, m_codePoint);
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, m_combinedCodePoint);
  return numberOfChar;
}

void CombinedCodePointLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  assert(m_displayType == DisplayType::None);
  constexpr int bufferSize =  2 * sizeof(CodePoint)/sizeof(char) + 1; // Null-terminating char
  char buffer[bufferSize];
  serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, 0);
  ctx->drawString(buffer, p, m_font, expressionColor, backgroundColor);
}

bool CombinedCodePointLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::CombinedCodePointLayout);
  CombinedCodePointLayout & cpl = static_cast<CombinedCodePointLayout &>(l);
  return combinedCodePoint() == cpl.combinedCodePoint() && CodePointLayoutNode::protectedIsIdenticalTo(l);
}

CombinedCodePointLayout CombinedCodePointLayout::Builder(CodePoint mainCodePoint, CodePoint combinedCodePoint, const KDFont * font) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(CombinedCodePointLayoutNode));
  CombinedCodePointLayoutNode * node = new (bufferNode) CombinedCodePointLayoutNode(mainCodePoint, combinedCodePoint, font);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<CombinedCodePointLayout &>(h);
}

}
