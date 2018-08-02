#include <poincare/char_layout_node.h>
#include <poincare/layout_engine.h>
#include <ion/charset.h>

namespace Poincare {

// LayoutNode
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

int CharLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, m_char);
}

bool CharLayoutNode::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis <= 0
      && (m_char == '+'
        || m_char == '-'
        || m_char == '*'
        || m_char == Ion::Charset::MultiplicationSign
        || m_char == Ion::Charset::MiddleDot
        || m_char == Ion::Charset::Sto
        || m_char == '='
        || m_char == ','))
  {
    return false;
  }
  return true;
}

// Sizing and positioning
KDSize CharLayoutNode::computeSize() {
  return KDText::charSize(m_fontSize);
}

KDCoordinate CharLayoutNode::computeBaseline() {
  return (KDText::charSize(m_fontSize).height()+1)/2; //TODO +1 ?
}

void CharLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  char string[2] = {m_char, 0};
  ctx->drawString(string, p, m_fontSize, expressionColor, backgroundColor);
}

}
