#include <poincare/layout_node.h>
#include <poincare/allocation_failed_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_reference.h>

namespace Poincare {

// Rendering

void LayoutNode::draw(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  for (LayoutNode * l : children()) {
    l->draw(ctx, p, expressionColor, backgroundColor);
  }
  render(ctx, absoluteOrigin().translatedBy(p), expressionColor, backgroundColor);
}

KDPoint LayoutNode::origin() {
  LayoutNode * p = parent();
  if (p != nullptr) {
    return absoluteOrigin();
  } else {
    return KDPoint(absoluteOrigin().x() - p->absoluteOrigin().x(),
        absoluteOrigin().y() - p->absoluteOrigin().y());
  }
}

KDPoint LayoutNode::absoluteOrigin() {
  LayoutNode * p = parent();
  if (!m_positioned) {
    if (p != nullptr) {
      m_frame.setOrigin(p->absoluteOrigin().translatedBy(p->positionOfChild(this)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_positioned = true;
  }
  return m_frame.origin();
}

KDSize LayoutNode::layoutSize() {
  if (!m_sized) {
    computeSize();
  }
  return m_frame.size();
}

KDCoordinate LayoutNode::baseline() {
  if (!m_baselined) {
    computeBaseline();
  }
  return m_baseline;
}

void LayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_sized = false;
  m_positioned = false;
  m_baselined = false;
  for (LayoutNode * l : children()) {
    l->invalidAllSizesPositionsAndBaselines();
  }
}

// TreeNode

TreeNode * LayoutNode::FailedAllocationStaticNode() {
  return LayoutRef::FailedAllocationStaticNode();
}

// Tree navigation
LayoutCursor LayoutNode::equivalentCursor(LayoutCursor * cursor) {
  return LayoutCursor(cursor->layoutReference());
}

// Tree modification
void LayoutNode::addSibling(LayoutCursor * cursor, LayoutNode * sibling) {
  privateAddSibling(cursor, sibling, false);
}

void LayoutNode::addSiblingAndMoveCursor(LayoutCursor * cursor, LayoutNode * sibling) {
  privateAddSibling(cursor, sibling, true);
}

// Private

void LayoutNode::privateAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  /* The layout must have a parent, because HorizontalLayout overrides
   * privateAddSibling and only an HorizontalLayout can be the root layout. */
  LayoutNode * p = parent();
  assert(p != nullptr);
  if (p->isHorizontal()) {
    int indexInParent = p->indexOfChild(this);
    int siblingIndex = cursor->position() == LayoutCursor::Position::Left ? indexInParent : indexInParent + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. Do not enter the special case if
     * "this" is a VerticalOffsetLayout, to avoid an infinite loop. */
    if (!isVerticalOffset()) {
      LayoutNode * neighbour = nullptr;
      if (cursor->position() == LayoutCursor::Position::Left && indexInParent > 0) {
        neighbour = p->childAtIndex(indexInParent - 1);
      } else if (cursor->position() ==LayoutCursor::Position::Right && indexInParent < p->numberOfChildren() - 1) {
        neighbour = p->childAtIndex(indexInParent + 1);
      }
      if (neighbour != nullptr && neighbour->isVerticalOffset()) {
        cursor->setLayoutNode(neighbour);
        cursor->setPosition(cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left);
        if (moveCursor) {
          neighbour->addSiblingAndMoveCursor(cursor, sibling);
        } else {
          neighbour->addSibling(cursor, sibling);
        }
        return;
      }
    }

    // Else, let the parent add the sibling.
    if (moveCursor) {
      if (siblingIndex < p->numberOfChildren()) {
        cursor->setLayoutNode(p->childAtIndex(siblingIndex));
        cursor->setPosition(LayoutCursor::Position::Left);
      } else {
        cursor->setLayoutNode(p);
        cursor->setPosition(LayoutCursor::Position::Right);
      }
    }
    static_cast<HorizontalLayoutNode *>(p)->addOrMergeChildAtIndex(sibling, siblingIndex, true); //TODO
    return;
  }
  LayoutNode * juxtapositionLayout = nullptr;
  if (cursor->position() == LayoutCursor::Position::Left) {
    juxtapositionLayout = replaceWithJuxtapositionOf(sibling, this); //TODO
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    juxtapositionLayout = replaceWithJuxtapositionOf(this, sibling);
  }
  if (moveCursor) {
    cursor->setLayoutNode(juxtapositionLayout);
    cursor->setPosition(LayoutCursor::Position::Right);
  }
}

}
