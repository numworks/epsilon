#include "string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

StringLayout::StringLayout(const char * string, size_t length, KDText::FontSize fontSize) :
  ExpressionLayout(),
  m_fontSize(fontSize)
{
  m_string = new char[length+1];
  memcpy(m_string, string, length);
  m_string[length] = 0;
  // Half height of the font.
  m_baseline = (KDText::charSize(m_fontSize).height()+1)/2;
}

StringLayout::~StringLayout() {
  delete[] m_string;
}

char * StringLayout::text() {
  return m_string;
}

bool StringLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go before the last char.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    size_t stringLength = strlen(m_string);
    if (stringLength > 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Inside);
      cursor->setPositionInside(stringLength - 1);
      return true;
    }
    if (stringLength == 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
    assert(stringLength == 0);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    if (m_parent) {
      return m_parent->moveLeft(cursor);
    }
    return false;
  }
  // Case: Inside.
  // Go one char left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Inside) {
    int cursorIndex = cursor->positionInside();
    assert(cursorIndex > 0 && cursorIndex < strlen(m_string));
    if (cursorIndex == 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      return true;
    }
    cursor->setPositionInside(cursorIndex - 1);
    return true;
  }
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

ExpressionLayout * StringLayout::child(uint16_t index) {
  return nullptr;
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

}
