#include <poincare/char_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

// LayoutNode
int CharLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, m_char);
}

void CharLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Right) {
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void CharLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Left) {
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

// Sizing and positioning
void CharLayoutNode::computeSize() {
  assert(!m_sized);
  m_frame.setSize(KDText::charSize(m_fontSize));
  m_sized = true;
}

void CharLayoutNode::computeBaseline() {
  assert(!m_baselined);
  m_baseline = (KDText::charSize(m_fontSize).height()+1)/2; //TODO +1 ?
  m_baselined = true;
}

void CharLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  char string[2] = {m_char, 0};
  ctx->drawString(string, p, m_fontSize, expressionColor, backgroundColor);
}

}
