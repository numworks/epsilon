#include <poincare/layout_node.h>
#include <poincare/allocation_failed_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_reference.h>
#include <ion/display.h>

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
void LayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  moveCursorVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void LayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  moveCursorVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void LayoutNode::moveCursorUpInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveCursorInDescendantsVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout);
}

void LayoutNode::moveCursorDownInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  return moveCursorInDescendantsVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout);
}

LayoutCursor LayoutNode::equivalentCursor(LayoutCursor * cursor) {
  // Only HorizontalLayout may have no parent, and it overloads this method
  assert(parent());
  return (cursor->layoutReference().node() == this) ? parent()->equivalentCursor(cursor) : LayoutCursor();
}

// Tree modification
void LayoutNode::addSibling(LayoutCursor * cursor, LayoutNode * sibling) {
  privateAddSibling(cursor, sibling, false);
}

void LayoutNode::addSiblingAndMoveCursor(LayoutCursor * cursor, LayoutNode * sibling) {
  privateAddSibling(cursor, sibling, true);
}

void LayoutNode::removeChildAndMoveCursor(LayoutNode * l, LayoutCursor * cursor) {
  assert(hasChild(l));
  int index = indexOfChild(l);
  removeChild(l);
  if (index < numberOfChildren()) {
    cursor->setLayoutNode(childAtIndex(index));
    cursor->setPosition(LayoutCursor::Position::Left);
  } else {
    int newPointedLayoutIndex = index - 1;
    assert(newPointedLayoutIndex >= 0);
    assert(newPointedLayoutIndex < numberOfChildren());
    cursor->setLayoutNode(childAtIndex(newPointedLayoutIndex));
    cursor->setPosition(LayoutCursor::Position::Right);
  }
}

void LayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  int indexOfPointedLayout = indexOfChild(cursor->layoutReference().typedNode());
  if (indexOfPointedLayout >= 0) {
    // Case: The pointed layout is a child. Move Left.
    assert(cursor->position() == LayoutCursor::Position::Left);
    bool shouldRecomputeLayout = false;
    cursor->moveLeft(&shouldRecomputeLayout);
    return;
  }
  assert(cursor->layoutReference().node() == this);
  LayoutNode * p = parent();
  // Case: this is the pointed layout.
  if (p == nullptr) {
    // Case: No parent. Return.
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Ask the parent.
    p->deleteBeforeCursor(cursor);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Delete the layout.
  p->removeChildAndMoveCursor(this, cursor);
}

LayoutNode * LayoutNode::replaceWith(LayoutNode * newChild) {
  LayoutRef newRef(newChild);
  LayoutRef(this).replaceWith(newRef);
  return newRef.typedNode();
}

void LayoutNode::replaceChild(LayoutNode * oldChild, LayoutNode * newChild) {
  LayoutRef(this).replaceChild(LayoutRef(oldChild), LayoutRef(newChild));
}

LayoutNode * LayoutNode::replaceWithAndMoveCursor(LayoutNode * newChild, LayoutCursor * cursor) {
  return (LayoutRef(this).replaceWithAndMoveCursor(LayoutRef(newChild), cursor)).typedNode();
}

LayoutNode * LayoutNode::replaceWithJuxtapositionOf(LayoutNode * leftChild, LayoutNode * rightChild) {
  return (LayoutRef(this).replaceWithJuxtapositionOf(LayoutRef(leftChild), LayoutRef(rightChild))).typedNode();
}

void LayoutNode::replaceChildAndMoveCursor(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor) {
  assert(hasChild(oldChild));
  if (!newChild->hasAncestor(oldChild, false)) {
    cursor->setPosition(LayoutCursor::Position::Right);
  }
  replaceChild(oldChild, newChild);
  cursor->setLayoutNode(newChild);
}

// Other
bool LayoutNode::hasText() const {
  /* A layout has text if it is not empty or an allocation failure and it is
   * not an horizontal layout with no child or with one child with no text. */
  if (isEmpty() || isAllocationFailure()){
    return false;
  }
  int numChildren = numberOfChildren();
  return !(isHorizontal() && (numChildren == 0 || (numChildren == 1 && !(const_cast<LayoutNode *>(this)->childAtIndex(0)->hasText()))));
}

bool LayoutNode::canBeOmittedMultiplicationLeftFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true too. If isCollapsable changes, it might not be the
   * case anymore so make sure to modify this function if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, true);
}

bool LayoutNode::canBeOmittedMultiplicationRightFactor() const {
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true and isVerticalOffset is false. If one of these
   * functions changes, it might not be the case anymore so make sure to modify
   * canBeOmittedMultiplicationRightFactor if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, false) && !isVerticalOffset();
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
    static_cast<HorizontalLayoutNode *>(p)->addOrMergeChildAtIndex(sibling, siblingIndex, true);
    return;
  }
  LayoutNode * juxtapositionLayout = nullptr;
  if (cursor->position() == LayoutCursor::Position::Left) {
    juxtapositionLayout = replaceWithJuxtapositionOf(sibling, this);
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    juxtapositionLayout = replaceWithJuxtapositionOf(this, sibling);
  }
  if (moveCursor) {
    cursor->setLayoutNode(juxtapositionLayout);
    cursor->setPosition(LayoutCursor::Position::Right);
  }
}

void LayoutNode::collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex) {
  LayoutNode * p = parent();
  if (p == nullptr || !p->isHorizontal()) {
    return;
  }
  int indexInParent = p->indexOfChild(this);
  int numberOfSiblings = p->numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  LayoutNode * absorbingChild = childAtIndex(absorbingChildIndex);
  if (!absorbingChild || !absorbingChild->isHorizontal()) {
    return;
  }
  HorizontalLayoutNode * horizontalAbsorbingChild = static_cast<HorizontalLayoutNode *>(absorbingChild);
  if (direction == HorizontalDirection::Right && indexInParent < numberOfSiblings - 1) {
    canCollapse = !(p->childAtIndex(indexInParent+1)->mustHaveLeftSibling());
  }
  LayoutNode * sibling = nullptr;
  bool forceCollapse = false;
  while (canCollapse) {
    if (direction == HorizontalDirection::Right && indexInParent == numberOfSiblings - 1) {
      break;
    }
    if (direction == HorizontalDirection::Left && indexInParent == 0) {
      break;
    }
    int siblingIndex = direction == HorizontalDirection::Right ? indexInParent+1 : indexInParent-1;
    sibling = p->childAtIndex(siblingIndex);
    /* Even if forceCollapse is true, isCollapsable should be called to update
     * the number of open parentheses. */
    bool shouldCollapse = sibling->isCollapsable(&numberOfOpenParenthesis, direction == HorizontalDirection::Left);
    if (shouldCollapse || forceCollapse) {
      /* If the collapse direction is Left and the next sibling to be collapsed
       * must have a left sibling, force the collapsing of this needed left
       * sibling. */
      forceCollapse = direction == HorizontalDirection::Left && sibling->mustHaveLeftSibling();
      p->removeChildAtIndex(siblingIndex);
      int newIndex = direction == HorizontalDirection::Right ? absorbingChild->numberOfChildren() : 0;
      horizontalAbsorbingChild->addOrMergeChildAtIndex(sibling, newIndex, true);
      numberOfSiblings--;
      if (direction == HorizontalDirection::Left) {
        indexInParent--;
      }
    } else {
      break;
    }
  }
}

// Private

void LayoutNode::moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (!equivalentPositionVisited) {
    LayoutCursor cursorEquivalent = equivalentCursor(cursor);
    if (cursorEquivalent.isDefined()) {
      cursor->setLayoutReference(cursorEquivalent.layoutReference());
      cursor->setPosition(cursorEquivalent.position());
      if (direction == VerticalDirection::Up) {
        cursor->layoutNode()->moveCursorUp(cursor, shouldRecomputeLayout, true);
      } else {
        cursor->layoutNode()->moveCursorDown(cursor, shouldRecomputeLayout, true);
      }
      return;
    }
  }
  LayoutNode * p = parent();
  if (p != nullptr) {
    if (direction == VerticalDirection::Up) {
      p->moveCursorUp(cursor, shouldRecomputeLayout, true);
    } else {
      p->moveCursorDown(cursor, shouldRecomputeLayout, true);
    }
    return;
  }
  cursor->setLayoutNode(nullptr);
}

void LayoutNode::moveCursorInDescendantsVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  LayoutNode *  chilResult = nullptr;
  LayoutNode ** childResultPtr = &chilResult;
  LayoutCursor::Position resultPosition = LayoutCursor::Position::Left;
  /* The distance between the cursor and its next position cannot be greater
   * than this initial value of score. */
  int resultScore = Ion::Display::Width*Ion::Display::Width + Ion::Display::Height*Ion::Display::Height;

  scoreCursorInDescendantsVertically(direction, cursor, shouldRecomputeLayout, childResultPtr, &resultPosition, &resultScore);

  // If there is a valid result
  if (*childResultPtr != nullptr) {
    *shouldRecomputeLayout = chilResult->addGreySquaresToAllMatrixAncestors();
  }
  cursor->setLayoutNode(chilResult);
  cursor->setPosition(resultPosition);
}

void LayoutNode::scoreCursorInDescendantsVertically (
    VerticalDirection direction,
    LayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    LayoutNode ** childResult,
    void * resultPosition,
    int * resultScore)
{
  LayoutCursor::Position * castedResultPosition = static_cast<LayoutCursor::Position *>(resultPosition);
  KDPoint cursorMiddleLeft = cursor->middleLeftPoint();
  bool layoutIsUnderOrAbove = direction == VerticalDirection::Up ? m_frame.isAbove(cursorMiddleLeft) : m_frame.isUnder(cursorMiddleLeft);
  bool layoutContains = m_frame.contains(cursorMiddleLeft);

  if (layoutIsUnderOrAbove) {
    // Check the distance to a Left cursor.
    int currentDistance = LayoutCursor(this, LayoutCursor::Position::Left).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance <= *resultScore ){
      *childResult = this;
      *castedResultPosition = LayoutCursor::Position::Left;
      *resultScore = currentDistance;
    }

    // Check the distance to a Right cursor.
    currentDistance = LayoutCursor(this, LayoutCursor::Position::Right).middleLeftPoint().squareDistanceTo(cursorMiddleLeft);
    if (currentDistance < *resultScore) {
      *childResult = this;
      *castedResultPosition = LayoutCursor::Position::Right;
      *resultScore = currentDistance;
    }
  }
  if (layoutIsUnderOrAbove || layoutContains) {
    int childIndex = 0;
    while (childAtIndex(childIndex++)) {
      childAtIndex(childIndex-1)->scoreCursorInDescendantsVertically(direction, cursor, shouldRecomputeLayout, childResult, castedResultPosition, resultScore);
    }
  }
}

bool LayoutNode::changeGreySquaresOfAllMatrixAncestors(bool add) {
  bool changedSquares = false;
  LayoutNode * currentAncestor = parent();
  while (currentAncestor != nullptr) {
    /*if (currentAncestor->isMatrix()) {
      if (add) {
        static_cast<MatrixLayout *>(currentAncestor)->addGreySquares();
      } else {
        static_cast<MatrixLayout *>(currentAncestor)->removeGreySquares();
      }
      changedSquares = true;
    }*/ //TODO
    currentAncestor = currentAncestor->parent();
  }
  return changedSquares;
}

}
