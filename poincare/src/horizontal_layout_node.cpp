#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate c1, KDCoordinate c2) { return c1 > c2 ? c1 : c2; }

int HorizontalLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (numberOfChildren() == 0) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[0] = 0;
    return 0;
  }
  return LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "");
}

void HorizontalLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (this == cursor->layoutReference().node()) {
    if (cursor->position() == LayoutCursor::Position::Left) {
      // Case: Left. Ask the parent.
      LayoutNode * parentNode = parent();
      if (parentNode != nullptr) {
        parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
      }
      return;
    }
    assert(cursor->position() == LayoutCursor::Position::Right);
    /* Case: Right. Go to the last child if there is one, and move Left. Else
     * go Left and ask the parent. */
    int childrenCount = numberOfChildren();
    if (childrenCount >= 1) {
      cursor->setLayoutNode(static_cast<LayoutNode *>(childTreeAtIndex(childrenCount-1)));
    } else {
      cursor->setPosition(LayoutCursor::Position::Left);
    }
    return cursor->moveLeft(shouldRecomputeLayout);
  }

  // Case: The cursor is Left of a child.
  assert(cursor->position() == LayoutCursor::Position::Left);
  int childIndex = indexOfChildByIdentifier(cursor->layoutIdentifier());
  assert(childIndex >= 0);
  if (childIndex == 0) {
    // Case: the child is the leftmost. Ask the parent.
    if (parent()) {
      cursor->setLayoutNode(this);
      return cursor->moveLeft(shouldRecomputeLayout);
    }
    return;
  }
  // Case: the child is not the leftmost. Go to its left sibling and move Left.
  cursor->setLayoutNode(static_cast<LayoutNode *>(childTreeAtIndex(childIndex-1)));
  cursor->setPosition(LayoutCursor::Position::Right);
  cursor->moveLeft(shouldRecomputeLayout);
}

void HorizontalLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  //TODO
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

KDSize HorizontalLayoutNode::computeSize() {
  assert(!m_sized);
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (LayoutNode * l : directChildren()) {
    KDSize childSize = l->size();
    totalWidth += childSize.width();
    maxUnderBaseline = max(maxUnderBaseline, childSize.height() - l->baseline());
    maxAboveBaseline = max(maxAboveBaseline, l->baseline());
  }
  m_frame.setSize(KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline));
  m_sized = true;
}

void HorizontalLayoutNode::computeBaseline() {
  assert(!m_baselined);
  m_baseline = 0;
  for (LayoutNode * l : directChildren()) {
    m_baseline = max(m_baseline, l->baseline());
  }
  m_baselined = true;
}

KDPoint HorizontalLayoutNode::positionOfChild(LayoutNode * l) {
  assert(hasChild(l));
  KDCoordinate x = 0;
  int index = indexOfChild(l);
  assert(index > -1);
  if (index > 0) {
    LayoutNode * previousChild = child(index-1);
    x = previousChild->origin().x() + previousChild->size().width();
  }
  KDCoordinate y = baseline() - l->baseline();
  return KDPoint(x, y);
}

}
