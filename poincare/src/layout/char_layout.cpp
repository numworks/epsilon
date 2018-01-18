#include "char_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <ion/charset.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

CharLayout::CharLayout(char c, KDText::FontSize fontSize) :
  StaticLayoutHierarchy<0>(),
  m_char(c),
  m_fontSize(fontSize)
{
}

ExpressionLayout * CharLayout::clone() const {
  CharLayout * layout = new CharLayout(m_char, m_fontSize);
  return layout;
}

bool CharLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool CharLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool CharLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis <= 0
      && (m_char == '+'
        || m_char == '-'
        || m_char == '*'
        || m_char == Ion::Charset::MultiplicationSign
        || m_char == Ion::Charset::MiddleDot))
  {
    return false;
  }
  return true;
}

void CharLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  char string[2] = {m_char, 0};
  ctx->drawString(string, p, m_fontSize, expressionColor, backgroundColor);
}

KDPoint CharLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

KDSize CharLayout::computeSize() {
  return KDText::charSize(m_fontSize);
}

void CharLayout::computeBaseline() {
  // Half height of the font.
  m_baseline = (KDText::charSize(m_fontSize).height()+1)/2;
  m_baselined = true;
}

}
