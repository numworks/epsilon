#include <poincare/empty_layout.h>
#include <poincare/matrix_layout.h>
#include <escher/palette.h>
#include <assert.h>

namespace Poincare {

void EmptyLayoutNode::setVisible(bool visible) {
  if (m_visibility == Visibility::Never) {
    return;
  }
  m_visibility = visible ? Visibility::On : Visibility::Off;
}

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
  cursor->setPosition(LayoutCursor::Position::Left);
  LayoutNode * p = parent();
  assert(p != nullptr);
  p->moveCursorLeft(cursor, shouldRecomputeLayout);
}

void EmptyLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() == this);
  // Ask the parent.
  cursor->setPosition(LayoutCursor::Position::Right);
  askParentToMoveCursorRight(cursor, shouldRecomputeLayout);
}

int EmptyLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize-1;
  }
  buffer[0] = 0;
  return 0;
}

KDSize EmptyLayoutNode::computeSize(KDFont::Size font) {
  KDCoordinate sizeWidth = isVisible() ? width(font) + 2*(m_margins ? k_marginWidth : 0) : 0;
  return KDSize(sizeWidth, height(font) + 2*(m_margins ? k_marginHeight : 0));
}

KDCoordinate EmptyLayoutNode::computeBaseline(KDFont::Size font) {
  return (m_margins ? k_marginHeight : 0) + height(font)/2;
}

void EmptyLayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  /* The two cursor positions around an EmptyLayoutNode are equivalent, so both
   * should be checked. */
  assert(cursor->layoutNode() == this);
  LayoutCursor cursorResult = *cursor;
  LayoutNode::moveCursorVertically(direction, &cursorResult, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  if (cursorResult.isDefined()) {
    cursor->setTo(&cursorResult);
    return;
  }
  LayoutCursor::Position newPosition = cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;
  cursor->setPosition(newPosition);
  LayoutNode::moveCursorVertically(direction, cursor, shouldRecomputeLayout, false, forSelection);
}

bool EmptyLayoutNode::willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) {
  EmptyLayout thisRef(this);
  if (m_color == Color::Gray) {
    /* The parent is a MatrixLayout, and the current empty row or column is
     * being filled in, so add a new empty row or column. */
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    if (GridLayoutNode::IsGridLayoutType(parentNode->type())) {
      static_cast<GridLayoutNode *>(parentNode)->willAddSiblingToEmptyChildAtIndex(parentNode->indexOfChild(this));
    }
    // WARNING: Do not use previous node pointers afterwards.
  }
  if (sibling->mustHaveLeftSibling()) {
    thisRef.setColor(Color::Yellow);
    return true;
  } else {
    thisRef.replaceWith(*sibling, cursor);
    // WARNING: do not call "this" afterwards
    return false;
  }
}

void EmptyLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  if (isVisible()) {
    KDColor fillColor = m_color == Color::Yellow ? Escher::Palette::YellowDark : Escher::Palette::GrayBright;
    KDCoordinate marginWidth = m_margins ? k_marginWidth : 0;
    KDCoordinate marginHeight = m_margins ? k_marginHeight : 0;
    ctx->fillRect(KDRect(p.x() + marginWidth, p.y() + marginHeight, width(font), height(font)), fillColor);
  }
}

bool EmptyLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::EmptyLayout);
  EmptyLayoutNode * n = static_cast<EmptyLayoutNode *>(l.node());
  return color() == n->color();
}

EmptyLayout::EmptyLayout(const EmptyLayoutNode * n) : Layout(n) {}

EmptyLayout EmptyLayout::Builder(EmptyLayoutNode::Color color, EmptyLayoutNode::Visibility visible, bool margins) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(EmptyLayoutNode));
  EmptyLayoutNode * node = new (bufferNode) EmptyLayoutNode(color, visible, margins);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<EmptyLayout &>(h);
}

}
