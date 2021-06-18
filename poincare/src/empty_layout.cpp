#include <poincare/empty_layout.h>
#include <poincare/matrix_layout.h>
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

void EmptyLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() == this);
  // Ask the parent.
  LayoutNode * p = parent();
  if (p != nullptr) {
    cursor->setPosition(LayoutCursor::Position::Left);
    p->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void EmptyLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() == this);
  // Ask the parent.
  LayoutNode * p = parent();
  if (p != nullptr) {
    cursor->setPosition(LayoutCursor::Position::Right);
    p->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

int EmptyLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[0] = 0;
  return 0;
}

KDSize EmptyLayoutNode::computeSize() {
  KDCoordinate sizeWidth = m_isVisible ? width() + 2*(m_margins ? k_marginWidth : 0) : 0;
  return KDSize(sizeWidth, height() + 2*(m_margins ? k_marginHeight : 0));
}

KDCoordinate EmptyLayoutNode::computeBaseline() {
  return (m_margins ? k_marginHeight : 0) + height()/2;
}

void EmptyLayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  /* The two cursor positions around an EmptyLayoutNode are equivalent, so both
   * should be checked. */
  assert(cursor->layoutNode() == this);
  LayoutCursor cursorResult = cursor->clone();
  LayoutNode::moveCursorVertically(direction, &cursorResult, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  if (cursorResult.isDefined()) {
    cursor->setTo(&cursorResult);
    return;
  }
  LayoutCursor::Position newPosition = cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;
  cursor->setPosition(newPosition);
  LayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, false, forSelection);
}

bool EmptyLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  EmptyLayout thisRef(this);
  Layout siblingRef(sibling); // Create the reference now, as the node might be moved
  if (m_color == Color::Gray) {
    /* The parent is a MatrixLayout, and the current empty row or column is
     * being filled in, so add a new empty row or column. */
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    parentNode->willAddSiblingToEmptyChildAtIndex(parentNode->indexOfChild(this));
    // WARNING: Do not use previous node pointers afterwards.
  }
  if (siblingRef.mustHaveLeftSibling()) {
    thisRef.setColor(Color::Yellow);
    return true;
  } else {
    thisRef.replaceWith(siblingRef, cursor);
    // WARNING: do not call "this" afterwards
    return false;
  }
}

void EmptyLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  if (m_isVisible) {
    KDColor fillColor = m_color == Color::Yellow ? Palette::CalculationEmptyBoxNeeded : Palette::CalculationEmptyBox;
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
    ctx->fillRect(KDRect(p.x()+(m_margins ? k_marginWidth : 0), p.y()+(m_margins ? k_marginHeight : 0), width(), height()), fillColor);
  }
}

bool EmptyLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::EmptyLayout);
  EmptyLayoutNode * n = static_cast<EmptyLayoutNode *>(l.node());
  return color() == n->color();
}

EmptyLayout::EmptyLayout(const EmptyLayoutNode * n) : Layout(n) {}

EmptyLayout EmptyLayout::Builder(EmptyLayoutNode::Color color, bool visible, const KDFont * font, bool margins) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(EmptyLayoutNode));
  EmptyLayoutNode * node = new (bufferNode) EmptyLayoutNode(color, visible, font, margins);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<EmptyLayout &>(h);
}

}
