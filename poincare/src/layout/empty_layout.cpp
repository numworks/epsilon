#include "empty_layout.h"
#include "matrix_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <escher/palette.h>
#include <assert.h>

namespace Poincare {

EmptyLayout::EmptyLayout(Color color, bool visible, KDText::FontSize size, bool margins) :
  StaticLayoutHierarchy(),
  m_isVisible(visible),
  m_color(color),
  m_size(size),
  m_margins(margins)
{
}

ExpressionLayout * EmptyLayout::clone() const {
  EmptyLayout * layout = new EmptyLayout(m_color, m_isVisible, m_size, m_margins);
  return layout;
}

void EmptyLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->deleteBeforeCursor(cursor);
  }
}

ExpressionLayoutCursor EmptyLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  assert(cursor.pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor.setPosition(ExpressionLayoutCursor::Position::Left);
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor EmptyLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  assert(cursor.pointedExpressionLayout() == this);
  // Ask the parent.
  if (m_parent) {
    cursor.setPosition(ExpressionLayoutCursor::Position::Right);
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

int EmptyLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[0] = 0;
  return 0;
}

void EmptyLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  if (m_isVisible) {
    KDColor fillColor = m_color == Color::Yellow ? Palette::YellowDark : Palette::GreyBright;
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
  }
}

KDSize EmptyLayout::computeSize() {
  KDCoordinate sizeWidth = m_isVisible ? width() + 2*(m_margins ? k_marginWidth : 0) : 0;
  return KDSize(sizeWidth, height() + 2*(m_margins ? k_marginHeight : 0));
}

void EmptyLayout::computeBaseline() {
  m_baseline = (m_margins ? k_marginHeight : 0) + height()/2;
  m_baselined = true;
}

void EmptyLayout::privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) {
  Color currentColor = m_color;
  int indexInParent = m_parent->indexOfChild(this);
  ExpressionLayout * parent = m_parent;
  if (sibling->mustHaveLeftSibling()) {
    m_color = Color::Yellow;
    ExpressionLayout::privateAddSibling(cursor, sibling, moveCursor);
  } else {
    if (moveCursor) {
      replaceWithAndMoveCursor(sibling, true, cursor);
    } else {
      replaceWith(sibling, true);
    }
  }
  if (currentColor == Color::Grey) {
    // The parent is a MatrixLayout.
    static_cast<MatrixLayout *>(parent)->newRowOrColumnAtIndex(indexInParent);
  }
}

ExpressionLayoutCursor EmptyLayout::cursorVerticalOf(VerticalDirection direction, ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  /* The two cursor positions around an EmptyLayout are equivalent, so both
   * should be checked. */
  assert(cursor.pointedExpressionLayout() == this);
  ExpressionLayoutCursor cursorResult = ExpressionLayout::cursorVerticalOf(direction, cursor, shouldRecomputeLayout, equivalentPositionVisited);
  if (cursorResult.isDefined()) {
    return cursorResult;
  }
  ExpressionLayoutCursor::Position newPosition = cursor.position() == ExpressionLayoutCursor::Position::Left ? ExpressionLayoutCursor::Position::Right : ExpressionLayoutCursor::Position::Left;
  cursor.setPosition(newPosition);
  return ExpressionLayout::cursorVerticalOf(direction, cursor, shouldRecomputeLayout, false);
}

}
