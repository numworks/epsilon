#include "editable_string_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

bool EditableStringLayout::moveLeft(ExpressionLayoutCursor * cursor) {
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

bool EditableStringLayout::moveRight(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go after the first char.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    size_t stringLength = strlen(m_string);
    if (stringLength > 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Inside);
      cursor->setPositionInside(1);
      return true;
    }
    if (stringLength == 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    assert(stringLength == 0);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->moveRight(cursor);
    }
    return false;
  }
  // Case: Inside.
  // Go one char right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Inside) {
    int cursorIndex = cursor->positionInside();
    assert(cursorIndex > 0 && cursorIndex < strlen(m_string));
    if (cursorIndex == strlen(m_string)-1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return true;
    }
    cursor->setPositionInside(cursorIndex + 1);
    return true;
  }
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

}
