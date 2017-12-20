#include "string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

StringLayout::StringLayout(const char * string, size_t length, KDText::FontSize fontSize) :
  StaticLayoutHierarchy<0>(),
  m_fontSize(fontSize)
{
  m_string = new char[length+1];
  memcpy(m_string, string, length);
  m_string[length] = 0;
}

StringLayout::~StringLayout() {
  delete[] m_string;
}

ExpressionLayout * StringLayout::clone() const {
  StringLayout * layout = new StringLayout(m_string, strlen(m_string), m_fontSize);
  return layout;
}

bool StringLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // A StringLayout is not editable, and the cursor cannot go inside it.
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
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool StringLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // A StringLayout is not editable, and the cursor cannot go inside it.
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
    return m_parent->moveRight(cursor);
  }
  return false;
}

void StringLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->drawString(m_string, p, m_fontSize, expressionColor, backgroundColor);
}

KDPoint StringLayout::positionOfChild(ExpressionLayout * child) {
  assert(false); // We should never be here
  return KDPointZero;
}

KDSize StringLayout::computeSize() {
  return KDText::stringSize(m_string, m_fontSize);
}

void StringLayout::computeBaseline() {
  // Half height of the font.
  m_baseline = (KDText::charSize(m_fontSize).height()+1)/2;
  m_baselined = true;
}

}
