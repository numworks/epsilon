#include "empty_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>

namespace Poincare {

ExpressionLayout * EmptyLayout::clone() const {
  EmptyLayout * layout = new EmptyLayout();
  return layout;
}

void EmptyLayout::addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) {
  replaceWith(brother, true);
}

bool EmptyLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool EmptyLayout::moveRight(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Ask the parent.
  cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

void EmptyLayout::computeBaseline() {
  m_baseline = 0;
  m_baselined = true;
}

}
