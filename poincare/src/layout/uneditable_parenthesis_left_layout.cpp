#include "uneditable_parenthesis_left_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * UneditableParenthesisLeftLayout::clone() const {
  return new UneditableParenthesisLeftLayout();
}

bool UneditableParenthesisLeftLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool UneditableParenthesisLeftLayout::moveRight(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return m_parent->moveRight(cursor);
  }
  return false;
}

}
