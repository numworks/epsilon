#include "empty_visible_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <assert.h>

namespace Poincare {

EmptyVisibleLayout::EmptyVisibleLayout() :
  EmptyLayout(),
  m_fillRectColor(KDColor::RGB24(0xffd370)) //TODO make static or in Palette?
{
}

ExpressionLayout * EmptyVisibleLayout::clone() const {
  EmptyVisibleLayout * layout = new EmptyVisibleLayout();
  return layout;
}

bool EmptyVisibleLayout::moveLeft(ExpressionLayoutCursor * cursor) {
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

bool EmptyVisibleLayout::moveRight(ExpressionLayoutCursor * cursor) {
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

void EmptyVisibleLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_marginWidth, p.y()+k_marginHeight, k_width, k_height), m_fillRectColor);
  ctx->fillRect(KDRect(p.x()+k_marginWidth, p.y()+k_marginHeight, k_width, k_height), m_fillRectColor);
}

KDSize EmptyVisibleLayout::computeSize() {
  return KDSize(k_width + 2*k_marginWidth, k_height + 2*k_marginHeight);
}

void EmptyVisibleLayout::computeBaseline() {
  m_baseline = k_marginHeight + k_height/2;
  m_baselined = true;
}

}
