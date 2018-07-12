#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>
#include <poincare/allocation_failed_layout_node.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>

namespace Poincare {

template<>
TreeNode * LayoutRef::FailedAllocationStaticNode() {
  static AllocationFailedLayoutNode FailureNode;
  if (FailureNode.identifier() >= -1) {
    int newIdentifier = TreePool::sharedPool()->registerStaticNode(&FailureNode);
    FailureNode.rename(newIdentifier);
  }
  return &FailureNode;
}

// Cursor
template <typename T>
LayoutCursor LayoutReference<T>::cursor() const {
  return LayoutCursor(this->typedNode());
}

template<>
LayoutCursor LayoutRef::equivalentCursor(LayoutCursor * cursor) {
  return this->typedNode()->equivalentCursor(cursor);
}

// Tree modification

template<>
void LayoutRef::replaceChild(LayoutRef oldChild, LayoutRef newChild, LayoutCursor * cursor) {
  if (!typedNode()->willReplaceChild(oldChild.typedNode(), newChild.typedNode(), cursor)) {
    return;
  }
  replaceTreeChild(oldChild, newChild);
  if (cursor != nullptr) {
    if (isAllocationFailure()) {
      cursor->setLayoutReference(*this);
    } else {
      cursor->setLayoutReference(newChild);
    }
  }
}

template<>
void LayoutRef::replaceWithJuxtapositionOf(LayoutRef leftChild, LayoutRef rightChild, LayoutCursor * cursor) {
  LayoutReference<LayoutNode> p = parent();
  assert(p.isDefined());
  assert(!p.isHorizontal());
  /* One of the children to juxtapose might be "this", so we cannot just call
   * replaceWith. */
  LayoutReference<LayoutNode> horizontalLayoutR = HorizontalLayoutRef();
  int index = indexInParent();
  horizontalLayoutR.addChildTreeAtIndex(leftChild, 0);
  horizontalLayoutR.addChildTreeAtIndex(rightChild, 1);
  p.addChildTreeAtIndex(horizontalLayoutR, index);

  if (cursor != nullptr) {
    if (horizontalLayoutR.isAllocationFailure()) {
      cursor->setLayoutReference(*this);
    } else {
      cursor->setLayoutReference(horizontalLayoutR);
      cursor->setPosition(LayoutCursor::Position::Right);
    }
  }
}

template <typename T>
void LayoutReference<T>::addChildAtIndex(LayoutRef l, int index, LayoutCursor * cursor) {
  int newIndex = index;
  if (!this->typedNode()->willAddChildAtIndex(l.typedNode(), &newIndex, cursor)) {
    return;
  }
  LayoutRef nextPointedLayout(nullptr);
  LayoutCursor::Position nextPosition = LayoutCursor::Position::Left;
  if (newIndex < this->numberOfChildren()) {
    nextPointedLayout = this->childAtIndex(newIndex);
    nextPosition = LayoutCursor::Position::Left;
  } else {
    nextPointedLayout = *this;
    nextPosition = LayoutCursor::Position::Right;
  }

  this->addChildTreeAtIndex(l, newIndex);

  if (cursor != nullptr) {
    if (this->isAllocationFailure()) {
      cursor->setLayoutReference(*this);
    } else {
      cursor->setLayoutReference(nextPointedLayout);
      cursor->setPosition(nextPosition);
    }
  }
}

template<>
void LayoutRef::addSibling(LayoutCursor * cursor, LayoutReference<LayoutNode> sibling, bool moveCursor) {
  if (!typedNode()->willAddSibling(cursor, sibling.typedNode(), moveCursor)) { //TODO
    return;
  }
  /* The layout must have a parent, because HorizontalLayoutRef's
   * preprocessAddSibling returns false only an HorizontalLayout can be the
   * root layout. */
  LayoutRef p = parent();
  assert(p.isDefined());
  if (p.isHorizontal()) {
    int indexInParent = p.indexOfChild(*this);
    int siblingIndex = cursor->position() == LayoutCursor::Position::Left ? indexInParent : indexInParent + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. Do not enter the special case if
     * "this" is a VerticalOffsetLayout, to avoid an infinite loop. */
    if (!isVerticalOffset()) {
      LayoutRef neighbour(nullptr);
      if (cursor->position() == LayoutCursor::Position::Left && indexInParent > 0) {
        neighbour = p.childAtIndex(indexInParent - 1);
      } else if (cursor->position() ==LayoutCursor::Position::Right && indexInParent < p.numberOfChildren() - 1) {
        neighbour = p.childAtIndex(indexInParent + 1);
      }
      if (neighbour.isDefined() && neighbour.isVerticalOffset()) {
        cursor->setLayoutReference(neighbour);
        cursor->setPosition(cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left);
        neighbour.addSibling(cursor, sibling, moveCursor);
        return;
      }
    }

    // Else, let the parent add the sibling.
    HorizontalLayoutRef(p.typedNode()).addOrMergeChildAtIndex(sibling, siblingIndex, true, cursor);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(sibling, *this, cursor);
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    replaceWithJuxtapositionOf(*this, sibling, cursor);
  }
}

template <typename T>
void LayoutReference<T>::removeChild(LayoutRef l, LayoutCursor * cursor, bool force) {
  if (!this->typedNode()->willRemoveChild(l.typedNode(), cursor)) {
    return;
  }
  assert(this->hasChild(l));
  int index = this->indexOfChild(l);
  this->removeTreeChild(l);
  if (index < this->numberOfChildren()) {
    cursor->setLayoutReference(this->childAtIndex(index));
    cursor->setPosition(LayoutCursor::Position::Left);
  } else {
    int newPointedLayoutIndex = index - 1;
    assert(newPointedLayoutIndex >= 0);
    if (newPointedLayoutIndex < this->numberOfChildren()) {
      cursor->setLayoutReference(this->childAtIndex(newPointedLayoutIndex));
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayoutReference(*this);
      cursor->setPosition(LayoutCursor::Position::Right);

    }
  }
  this->typedNode()->didRemoveChildAtIndex(index, cursor, force);
}

template <>
void LayoutRef::collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex) {
  LayoutRef p = parent();
  if (!p.isDefined() || !p.isHorizontal()) {
    return;
  }
  int idxInParent = p.indexOfChild(*this);
  int numberOfSiblings = p.numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  LayoutRef absorbingChild = childAtIndex(absorbingChildIndex);
  if (!absorbingChild.isDefined() || !absorbingChild.isHorizontal()) {
    return;
  }
  HorizontalLayoutRef horizontalAbsorbingChild = HorizontalLayoutRef(absorbingChild.node());
  if (direction == HorizontalDirection::Right && idxInParent < numberOfSiblings - 1) {
    canCollapse = !(p.childAtIndex(idxInParent+1).mustHaveLeftSibling());
  }
  LayoutRef sibling(nullptr);
  bool forceCollapse = false;
  while (canCollapse) {
    if (direction == HorizontalDirection::Right && idxInParent == numberOfSiblings - 1) {
      break;
    }
    if (direction == HorizontalDirection::Left && idxInParent == 0) {
      break;
    }
    int siblingIndex = direction == HorizontalDirection::Right ? idxInParent+1 : idxInParent-1;
    sibling = p.childAtIndex(siblingIndex);
    /* Even if forceCollapse is true, isCollapsable should be called to update
     * the number of open parentheses. */
    bool shouldCollapse = sibling.isCollapsable(&numberOfOpenParenthesis, direction == HorizontalDirection::Left);
    if (shouldCollapse || forceCollapse) {
      /* If the collapse direction is Left and the next sibling to be collapsed
       * must have a left sibling, force the collapsing of this needed left
       * sibling. */
      forceCollapse = direction == HorizontalDirection::Left && sibling.mustHaveLeftSibling();
      p.removeChildAtIndex(siblingIndex, nullptr);
      int newIndex = direction == HorizontalDirection::Right ? absorbingChild.numberOfChildren() : 0;
      horizontalAbsorbingChild.addOrMergeChildAtIndex(sibling, newIndex, true);
      numberOfSiblings--;
      if (direction == HorizontalDirection::Left) {
        idxInParent--;
      }
    } else {
      break;
    }
  }
}

template LayoutCursor LayoutReference<LayoutNode>::cursor() const;
template LayoutCursor LayoutReference<CharLayoutNode>::cursor() const;
template void LayoutReference<LayoutNode>::removeChild(LayoutRef l, LayoutCursor * cursor, bool force);
template void LayoutReference<HorizontalLayoutNode>::removeChild(LayoutRef l, LayoutCursor * cursor, bool force);
template void LayoutReference<LayoutNode>::addChildAtIndex(LayoutRef l, int index, LayoutCursor * cursor);
template void LayoutReference<HorizontalLayoutNode>::addChildAtIndex(LayoutRef l, int index, LayoutCursor * cursor);
}
