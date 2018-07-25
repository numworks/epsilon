#include <poincare/empty_layout_node.h>
#include <poincare/matrix_layout_node.h>
#include <escher/palette.h>
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

int EmptyLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
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
  LayoutNode::moveCursorVertically(direction, &cursorResult, shouldRecomputeLayout, equivalentPositionVisited);
  if (cursorResult.isDefined()) {
    cursor->setTo(&cursorResult);
    return;
  }
  LayoutCursor::Position newPosition = cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;
  cursor->setPosition(newPosition);
  LayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, false);
}

bool EmptyLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  EmptyLayoutRef thisRef(this);
  LayoutRef siblingRef(sibling); // Create the reference now, as the node might be moved
  if (m_color == Color::Grey) {
    /* The parent is a MatrixLayout, and the current empty row or column is
     * being filled in, so add a new empty row or column. */
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    static_cast<MatrixLayoutNode *>(parentNode)->newRowOrColumnAtIndex(parentNode->indexOfChild(this));
    // WARNING: Do not use "this" afterwards.
  }
  if (sibling->mustHaveLeftSibling()) {
    thisRef.setColor(Color::Yellow);
    return true;
  } else {
    thisRef.replaceWith(siblingRef, cursor);
    // WARNING: do not call "this" afterwards
    return false;
  }
}

void EmptyLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  if (m_isVisible) {
    KDColor fillColor = m_color == Color::Yellow ? Palette::YellowDark : Palette::GreyBright;
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
  }
}

}
