#include <poincare/layout_reference.h>
#include <poincare/bracket_pair_layout_node.h>
#include <poincare/char_layout_node.h>
#include <poincare/empty_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

// Cursor
LayoutCursor LayoutReference::cursor() const {
  assert(!isUninitialized());
  return LayoutCursor(const_cast<LayoutReference *>(this)->node());
}

LayoutCursor LayoutReference::equivalentCursor(LayoutCursor * cursor) {
  assert(!isUninitialized());
  return node()->equivalentCursor(cursor);
}

LayoutReference LayoutReference::root() const {
  assert(!isUninitialized());
  LayoutNode * r = node()->root();
  return r == nullptr ? LayoutReference() : LayoutReference(r);
}

// Tree modification

void LayoutReference::replaceChild(LayoutRef oldChild, LayoutRef newChild, LayoutCursor * cursor, bool force) {
  int childIndex = indexOfChild(oldChild);
  assert(childIndex >= 0);
  if (!node()->willReplaceChild(oldChild.node(), newChild.node(), cursor, force)) {
    return;
  }
  replaceChildInPlace(oldChild, newChild);
  if (cursor != nullptr) {
    cursor->setLayoutReference(newChild);
  }
  node()->didReplaceChildAtIndex(childIndex, cursor, force);
}

void LayoutReference::replaceChildWithEmpty(LayoutRef oldChild, LayoutCursor * cursor) {
  replaceChild(oldChild, EmptyLayoutRef(), cursor);
}

void LayoutReference::replaceWithJuxtapositionOf(LayoutRef leftChild, LayoutRef rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle) {
  LayoutReference p = parent();
  assert(!p.isUninitialized());
  if (!p.isHorizontal()) {
    /* One of the children to juxtapose might be "this", so we cannot just call
     * replaceWith. */
    HorizontalLayoutRef horizontalLayoutR;
    p.replaceChild(*this, horizontalLayoutR, cursor);
    horizontalLayoutR.addOrMergeChildAtIndex(leftChild, 0, false);
    if (putCursorInTheMiddle) {
      if (!horizontalLayoutR.isEmpty()) {
        cursor->setLayoutReference(horizontalLayoutR.childAtIndex(horizontalLayoutR.numberOfChildren()-1));
        cursor->setPosition(LayoutCursor::Position::Right);
      } else {
        cursor->setLayoutReference(horizontalLayoutR);
        cursor->setPosition(LayoutCursor::Position::Left);
      }
    }
    horizontalLayoutR.addOrMergeChildAtIndex(rightChild, 1, false);
    return;
  }
  /* The parent is an Horizontal layout, so directly add the two juxtaposition
   * children to the parent. */
  int idxInParent = p.indexOfChild(*this);
  HorizontalLayoutRef castedParent = HorizontalLayoutRef(static_cast<HorizontalLayoutNode *>(p.node()));
  if (putCursorInTheMiddle) {
    if (idxInParent > 0) {
      cursor->setLayoutReference(castedParent.childAtIndex(idxInParent-1));
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayoutReference(castedParent);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
  }
  castedParent.addOrMergeChildAtIndex(rightChild, idxInParent, true);
  castedParent.addOrMergeChildAtIndex(leftChild, idxInParent, true, putCursorInTheMiddle ? cursor : nullptr);
  p.removeChild(*this, cursor->layoutReference() == *this ? cursor : nullptr);
}

void LayoutReference::addChildAtIndex(LayoutRef l, int index, int currentNumberOfChildren, LayoutCursor * cursor) {
  int newIndex = index;
  int newCurrentNumberOfChildren = currentNumberOfChildren;
  if (!node()->willAddChildAtIndex(l.node(), &newIndex, &newCurrentNumberOfChildren, cursor)) {
    return;
  }
  LayoutRef nextPointedLayout;
  LayoutCursor::Position nextPosition = LayoutCursor::Position::Left;
  if (cursor != nullptr) {
    if (newIndex < this->numberOfChildren()) {
      nextPointedLayout = childAtIndex(newIndex);
      nextPosition = LayoutCursor::Position::Left;
    } else {
      nextPointedLayout = *this;
      nextPosition = LayoutCursor::Position::Right;
    }
  }

  addChildAtIndexInPlace(l, newIndex, newCurrentNumberOfChildren);

  if (cursor != nullptr) {
    cursor->setLayoutReference(nextPointedLayout);
    cursor->setPosition(nextPosition);
  }
}

void LayoutReference::addSibling(LayoutCursor * cursor, LayoutReference sibling, bool moveCursor) {
  if (!node()->willAddSibling(cursor, sibling.node(), moveCursor)) {
    return;
  }
  /* The layout must have a parent, because HorizontalLayoutRef's
   * preprocessAddSibling returns false only an HorizontalLayout can be the
   * root layout. */
  LayoutRef rootLayout = root();
  LayoutRef p = parent();
  assert(!p.isUninitialized());
  if (p.isHorizontal()) {
    int indexInParent = p.indexOfChild(*this);
    int siblingIndex = cursor->position() == LayoutCursor::Position::Left ? indexInParent : indexInParent + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. Do not enter the special case if
     * "this" is a VerticalOffsetLayout, to avoid an infinite loop. */
    if (!isVerticalOffset()) {
      LayoutRef neighbour;
      if (cursor->position() == LayoutCursor::Position::Left && indexInParent > 0) {
        neighbour = p.childAtIndex(indexInParent - 1);
      } else if (cursor->position() == LayoutCursor::Position::Right && indexInParent < p.numberOfChildren() - 1) {
        neighbour = p.childAtIndex(indexInParent + 1);
      }
      if (!neighbour.isUninitialized() && neighbour.isVerticalOffset()) {
        if (moveCursor) {
          cursor->setLayoutReference(neighbour);
          cursor->setPosition(cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left);
        }
        neighbour.addSibling(cursor, sibling, moveCursor);
        return;
      }
    }

    // Else, let the parent add the sibling.
    HorizontalLayoutRef(static_cast<HorizontalLayoutNode *>(p.node())).addOrMergeChildAtIndex(sibling, siblingIndex, true, moveCursor ? cursor : nullptr);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(sibling, *this, cursor);
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    replaceWithJuxtapositionOf(*this, sibling, cursor);
  }
}

void LayoutReference::removeChild(LayoutRef l, LayoutCursor * cursor, bool force) {
  if (!node()->willRemoveChild(l.node(), cursor, force)) {
    return;
  }
  assert(hasChild(l));
  int index = indexOfChild(l);
  removeChildInPlace(l, l.numberOfChildren());
  if (cursor) {
    if (index < numberOfChildren()) {
      LayoutRef newCursorRef = childAtIndex(index);
      cursor->setLayoutReference(newCursorRef);
      cursor->setPosition(LayoutCursor::Position::Left);
    } else {
      int newPointedLayoutIndex = index - 1;
      if (newPointedLayoutIndex >= 0 && newPointedLayoutIndex < numberOfChildren()) {
        cursor->setLayoutReference(childAtIndex(newPointedLayoutIndex));
        cursor->setPosition(LayoutCursor::Position::Right);
      } else {
        cursor->setLayoutReference(*this);
        cursor->setPosition(LayoutCursor::Position::Right);
      }
    }
  }
  node()->didRemoveChildAtIndex(index, cursor, force);
}

void LayoutReference::collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex) {
  LayoutRef p = parent();
  if (p.isUninitialized() || !p.isHorizontal()) {
    return;
  }
  int idxInParent = p.indexOfChild(*this);
  int numberOfSiblings = p.numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  LayoutRef absorbingChild = childAtIndex(absorbingChildIndex);
  if (absorbingChild.isUninitialized() || !absorbingChild.isHorizontal()) {
    return;
  }
  HorizontalLayoutRef horizontalAbsorbingChild = HorizontalLayoutRef(static_cast<HorizontalLayoutNode *>(absorbingChild.node()));
  if (direction == HorizontalDirection::Right && idxInParent < numberOfSiblings - 1) {
    canCollapse = !(p.childAtIndex(idxInParent+1).mustHaveLeftSibling());
  }
  LayoutRef sibling;
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

void LayoutReference::collapseSiblings(LayoutCursor * cursor) {
  LayoutRef rootLayout = root();
  if (node()->shouldCollapseSiblingsOnRight()) {
    LayoutReference absorbingChild = childAtIndex(rightCollapsingAbsorbingChildIndex());
    if (!absorbingChild.isHorizontal()) {
      LayoutRef horRef = HorizontalLayoutRef(absorbingChild.clone());
      replaceChild(absorbingChild, horRef, cursor, true);
    }
    collapseOnDirection(HorizontalDirection::Right, rightCollapsingAbsorbingChildIndex());
  }
  if (node()->shouldCollapseSiblingsOnLeft()) {
    LayoutReference absorbingChild = childAtIndex(leftCollapsingAbsorbingChildIndex());
    if (!absorbingChild.isHorizontal()) {
      replaceChild(absorbingChild, HorizontalLayoutRef(absorbingChild.clone()), cursor, true);
    }
    collapseOnDirection(HorizontalDirection::Left, leftCollapsingAbsorbingChildIndex());
  }
  node()->didCollapseSiblings(cursor);
}

}
