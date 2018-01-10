#include "empty_visible_layout.h"
#include "matrix_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <escher/palette.h>
#include <assert.h>

namespace Poincare {

EmptyVisibleLayout::EmptyVisibleLayout(Color color) :
  StaticLayoutHierarchy(),
  m_color(color)
{
}

ExpressionLayout * EmptyVisibleLayout::clone() const {
  EmptyVisibleLayout * layout = new EmptyVisibleLayout();
  return layout;
}

void EmptyVisibleLayout::addBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother) {
  Color currentColor = m_color;
  int indexInParent = m_parent->indexOfChild(this);
  ExpressionLayout * parent = m_parent;
  replaceWith(brother, true);
  if (currentColor == Color::Grey) {
    // The parent is a MatrixLayout.
    static_cast<MatrixLayout *>(parent)->newRowOrColumnAtIndex(indexInParent);
  }
}

void EmptyVisibleLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->backspaceAtCursor(cursor);
  }
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

int EmptyVisibleLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[0] = 0;
  return 0;
}

void EmptyVisibleLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDColor fillColor = m_color == Color::Yellow ? Palette::YellowDark : Palette::GreyBright;
  ctx->fillRect(KDRect(p.x()+k_marginWidth, p.y()+k_marginHeight, k_width, k_height), fillColor);
  ctx->fillRect(KDRect(p.x()+k_marginWidth, p.y()+k_marginHeight, k_width, k_height), fillColor);
}

KDSize EmptyVisibleLayout::computeSize() {
  return KDSize(k_width + 2*k_marginWidth, k_height + 2*k_marginHeight);
}

void EmptyVisibleLayout::computeBaseline() {
  m_baseline = k_marginHeight + k_height/2;
  m_baselined = true;
}

}
