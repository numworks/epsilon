#include <poincare/empty_layout_node.h>
//#include "matrix_layout.h" //TODO ?
#include <escher/palette.h>
#include <poincare/layout_cursor.h>
#include <assert.h>

namespace Poincare {

void EmptyLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  cursor->setPosition(LayoutCursor::Position::Left);
  LayoutNode * p = parent();
  if (p != nullptr) {
    return p->deleteBeforeCursor(cursor);
  }
}

void EmptyLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->layoutNode() == this);
  // Ask the parent.
  LayoutNode * p = parent();
  if (p != nullptr) {
    cursor->setPosition(LayoutCursor::Position::Left);
    p->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void EmptyLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->layoutNode() == this);
  // Ask the parent.
  LayoutNode * p = parent();
  if (p != nullptr) {
    cursor->setPosition(LayoutCursor::Position::Right);
    p->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

int EmptyLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[0] = 0;
  return 0;
}

void EmptyLayoutNode::computeSize() {
  assert(!m_sized);
  KDCoordinate sizeWidth = m_isVisible ? width() + 2*(m_margins ? k_marginWidth : 0) : 0;
  m_frame.setSize(KDSize(sizeWidth, height() + 2*(m_margins ? k_marginHeight : 0)));
  m_sized = true;
}

void EmptyLayoutNode::computeBaseline() {
  assert(!m_baselined);
  m_baseline = (m_margins ? k_marginHeight : 0) + height()/2;
  m_baselined = true;
}

void EmptyLayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  /* The two cursor positions around an EmptyLayoutNode are equivalent, so both
   * should be checked. */
  assert(cursor->layoutReference() == this);
  LayoutCursor cursorResult = cursor->clone();
  moveCursorVertically(direction, &cursorResult, shouldRecomputeLayout, equivalentPositionVisited);
  if (cursorResult.isDefined()) {
    cursor->setTo(&cursorResult);
    return;
  }
  LayoutCursor::Position newPosition = cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;
  cursor->setPosition(newPosition);
  LayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, false);
}

bool EmptyLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  if (sibling->mustHaveLeftSibling()) {
    m_color = Color::Yellow;
  }
  return true;
  /*   Color currentColor = m_color;
  int currentIndexInParent = indexInParent();
  if (currentColor == Color::Grey) {
  // The parent is a MatrixLayout.
  static_cast<MatrixLayoutNode *>(parent())->newRowOrColumnAtIndex(indexInParent);
  }*/
}

void EmptyLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  if (m_isVisible) {
    KDColor fillColor = m_color == Color::Yellow ? Palette::YellowDark : Palette::GreyBright;
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
  }
}

}
