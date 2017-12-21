#include "uneditable_parenthesis_right_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * UneditableParenthesisRightLayout::clone() const {
  return new UneditableParenthesisRightLayout();
}

bool UneditableParenthesisRightLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool UneditableParenthesisRightLayout::moveRight(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return m_parent->moveRight(cursor);
  }
  return false;
}

}
