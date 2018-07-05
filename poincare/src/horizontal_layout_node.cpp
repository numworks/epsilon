#include <poincare/horizontal_layout_node.h>
#include <poincare/layout_engine.h>

namespace Poincare {

static inline KDCoordinate maxCoordinate(KDCoordinate c1, KDCoordinate c2) { return c1 > c2 ? c1 : c2; }


// Tree navigation

LayoutCursor HorizontalLayoutNode::equivalentCursor(LayoutCursor * cursor) {
  if (cursor->layoutReference().node() == this) {
    // First or last child, if any
    int childrenCount = numberOfChildren();
    if (childrenCount == 0) {
      return LayoutCursor();
    }
    int index = cursor->position() == LayoutCursor::Position::Left ? 0 : childrenCount - 1;
    return LayoutCursor(childAtIndex(index), cursor->position());
  }
  // Left or right of a child: return right or left of its sibling, or of this
  int indexOfPointedLayout = indexOfChild(cursor->layoutNode());
  if (indexOfPointedLayout < 0) {
    return LayoutCursor();
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    if (indexOfPointedLayout == 0) {
      return LayoutCursor(this, LayoutCursor::Position::Left);
    }
    return LayoutCursor(childAtIndex(indexOfPointedLayout - 1), LayoutCursor::Position::Right);
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  if (indexOfPointedLayout == numberOfChildren() - 1) {
    return LayoutCursor(this, LayoutCursor::Position::Right);
  }
  return LayoutCursor(childAtIndex(indexOfPointedLayout + 1), LayoutCursor::Position::Left);
}

// Tree modification

void HorizontalLayoutNode::addOrMergeChildAtIndex(LayoutNode * l, int index, bool removeEmptyChildren) {
  if (l->isHorizontal()) {
    mergeChildrenAtIndex(static_cast<HorizontalLayoutNode *>(l), index, removeEmptyChildren);
  } else {
    addChildAtIndex(l, index);
  }
}

void HorizontalLayoutNode::mergeChildrenAtIndex(HorizontalLayoutNode * h, int index, bool removeEmptyChildren) {
  /* Remove any empty child that would be next to the inserted layout.
   * If the layout to insert starts with a vertical offset layout, any empty
   * layout child directly on the left of the inserted layout (if there is one)
   * should not be removed: it will be the base for the VerticalOffsetLayout. */
  bool shouldRemoveOnLeft = h->numberOfChildren() == 0 ? true : !(h->childAtIndex(0)->mustHaveLeftSibling());
  int newIndex = removeEmptyChildBeforeInsertionAtIndex(index, shouldRemoveOnLeft);

  // Merge the horizontal layout
  LayoutRef(this).mergeChildrenAtIndex(LayoutRef(h), newIndex);
}

void HorizontalLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  LayoutNode * p = parent();
  if (p == nullptr
      && cursor->layoutReference().node() == this
      && (cursor->position() == LayoutCursor::Position::Left
        || numberOfChildren() == 0))
  {
    /* Case: Left and this is the main layout or Right and this is the main
     * layout with no children. Return. */
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    int indexOfPointedLayout = indexOfChild(cursor->layoutReference().typedNode());
    if (indexOfPointedLayout >= 0) {
      /* Case: Left of a child.
       * Point Right of the previous child. If there is no previous child, point
       * Left of this. Perform another backspace. */
      if (indexOfPointedLayout == 0) {
        cursor->setLayoutNode(this);
      } else {
        assert(indexOfPointedLayout > 0);
        cursor->setLayoutNode(childAtIndex(indexOfPointedLayout - 1));
        cursor->setPosition(LayoutCursor::Position::Right);
      }
      cursor->performBackspace();
      return;
    }
  }
  assert(cursor->layoutReference().node() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Point to the last child and perform backspace.
    cursor->setLayoutNode(childAtIndex(numberOfChildren() - 1));
    cursor->performBackspace();
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

void HorizontalLayoutNode::removeChildAndMoveCursor(LayoutNode * l, LayoutCursor * cursor) {
  if (numberOfChildren() == 1) {
    assert(childAtIndex(0) == l);
    LayoutNode * p = parent();
    if (p != nullptr) {
      p->removeChildAndMoveCursor(this, cursor);
    } else {
      removeChild(l);
      cursor->setLayoutNode(this);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
    return;
  }
  LayoutNode::removeChildAndMoveCursor(l, cursor);
}

// LayoutNode

int HorizontalLayoutNode::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (numberOfChildren() == 0) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[0] = 0;
    return 0;
  }
  return LayoutEngine::writeInfixTreeRefTextInBuffer(TreeRef(const_cast<HorizontalLayoutNode *>(this)), buffer, bufferSize, numberOfSignificantDigits, "");
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
  if (this == cursor->layoutReference().node()) {
    if (cursor->position() == LayoutCursor::Position::Right) {
      // Case: Right. Ask the parent.
      LayoutNode * parentNode = parent();
      if (parentNode) {
        parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
      }
      return;
    }
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left
    int childrenCount = numberOfChildren();
    LayoutNode * parentNode = parent();
    if (childrenCount == 0) {
      // If there are no children, go Right and ask the parent
      cursor->setPosition(LayoutCursor::Position::Right);
      if (parentNode != nullptr) {
        parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
      }
      return;
    }
    /* If there is at least one child, set the cursor to the first child and
     * move it Right */
    LayoutNode * firstChild = childAtIndex(0);
    assert(firstChild != nullptr);
    cursor->setLayoutNode(firstChild);
    return firstChild->moveCursorRight(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Right of a child.
  assert(cursor->position() == LayoutCursor::Position::Right);
  int childIndex = indexOfChild(cursor->layoutReference().node());
  assert(childIndex >= 0);
  if (childIndex == numberOfChildren() - 1) {
    // Case: the child is the rightmost. Ask the parent.
    LayoutNode * parentNode = parent();
    if (parentNode) {
      cursor->setLayoutNode(this);
      parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
    }
    return;
  }
  /* Case: the child is not the rightmost. Go to its right sibling and move
   * Right. */
  cursor->setLayoutNode(childAtIndex(childIndex+1));
  cursor->setPosition(LayoutCursor::Position::Left);
  return childAtIndex(childIndex+1)->moveCursorRight(cursor, shouldRecomputeLayout);
}

void HorizontalLayoutNode::replaceChild(LayoutNode * oldChild, LayoutNode * newChild) {
  privateReplaceChild(oldChild, newChild, nullptr);
}

void HorizontalLayoutNode::replaceChildAndMoveCursor(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor) {
  privateReplaceChild(oldChild, newChild, cursor);
}


// Protected

void HorizontalLayoutNode::computeSize() {
  assert(!m_sized);
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (LayoutNode * l : children()) {
    KDSize childSize = l->layoutSize();
    totalWidth += childSize.width();
    maxUnderBaseline = maxCoordinate(maxUnderBaseline, childSize.height() - l->baseline());
    maxAboveBaseline = maxCoordinate(maxAboveBaseline, l->baseline());
  }
  m_frame.setSize(KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline));
  m_sized = true;
}

void HorizontalLayoutNode::computeBaseline() {
  assert(!m_baselined);
  m_baseline = 0;
  for (LayoutNode * l : children()) {
    m_baseline = maxCoordinate(m_baseline, l->baseline());
  }
  m_baselined = true;
}

KDPoint HorizontalLayoutNode::positionOfChild(LayoutNode * l) {
  assert(hasChild(l));
  KDCoordinate x = 0;
  int index = indexOfChild(l);
  assert(index > -1);
  if (index > 0) {
    LayoutNode * previousChild = childAtIndex(index-1);
    x = previousChild->origin().x() + previousChild->layoutSize().width();
  }
  KDCoordinate y = baseline() - l->baseline();
  return KDPoint(x, y);
}

// Private

void HorizontalLayoutNode::privateAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  int childrenCount = numberOfChildren();
  // Add the "sibling" as a child.
  if (cursor->position() == LayoutCursor::Position::Left) {
    int indexForInsertion = 0;
    /* If the first child is empty, remove it before adding the layout, unless
     * the new sibling needs the empty layout as a base. */
    if (childrenCount > 0 && childAtIndex(0)->isEmpty()) {
      if (sibling->mustHaveLeftSibling()) {
        indexForInsertion = 1;
      } else {
        /* We force the removal of the child even followed by a neighbourg
         * requiring a left sibling as we are about to add a sibling in first
         * position anyway. */
        privateRemoveChildAtIndex(0, true);
      }
    }
    if (moveCursor) {
      if (childrenCount > indexForInsertion) {
        cursor->setLayoutNode(childAtIndex(indexForInsertion));
      } else {
        cursor->setLayoutNode(this);
        cursor->setPosition(LayoutCursor::Position::Right);
      }
    }
    addOrMergeChildAtIndex(sibling, indexForInsertion, false);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // If the last child is empty, remove it before adding the layout.
  if (childrenCount > 0 && childAtIndex(childrenCount - 1)->isEmpty() && !sibling->mustHaveLeftSibling()) {
    /* Force remove the last child. */
    privateRemoveChildAtIndex(childrenCount - 1, true);
    childrenCount--;
  }
  addOrMergeChildAtIndex(sibling, childrenCount, false);
  if (moveCursor) {
    cursor->setLayoutNode(this);
  }
}

void HorizontalLayoutNode::privateReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor) {
  if (oldChild == newChild) {
    return;
  }
  int oldChildIndex = indexOfChild(oldChild);
  if (newChild->isEmpty()) {
    if (numberOfChildren() > 1) {
      /* If the new layout is empty and the horizontal layout has other
       * children, just remove the old child. */
      removeChild(oldChild);
      if (cursor != nullptr) {
        if (oldChildIndex == 0) {
          cursor->setLayoutNode(this);
          cursor->setPosition(LayoutCursor::Position::Left);
        } else {
          cursor->setLayoutNode(childAtIndex(oldChildIndex -1));
          cursor->setPosition(LayoutCursor::Position::Right);
        }
      }
      return;
    }
    /* The old layout was the only horizontal layout child, so if this has a
     * a parent, replace this with the new empty layout. */
    LayoutNode * p = parent();
    if (p != nullptr) {
      if (cursor) {
        replaceWithAndMoveCursor(newChild, cursor);
      } else {
        replaceWith(newChild);
      }
      return;
    }
    /* This is the main horizontal layout, the old child is its only child and
     * the new child is Empty: remove the old child and delete the new child. */
    assert(p == nullptr);
    removeChild(oldChild);
    if (cursor != nullptr) {
      cursor->setLayoutNode(this);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
    return;
  }
  /* If the new child is also an horizontal layout, steal the children of the
   * new layout then destroy it. */
  bool oldWasAncestorOfNewLayout = newChild->hasAncestor(oldChild, false);
  if (newChild->isHorizontal()) {
    int indexForInsertion = indexOfChild(oldChild);
    if (cursor != nullptr) {
      /* If the old layout is not an ancestor of the new layout, or if the
       * cursor was on the right of the new layout, place the cursor on the
       * right of the new layout, which is left of the next sibling or right of
       * the parent. */
      if (!oldWasAncestorOfNewLayout || cursor->position() == LayoutCursor::Position::Right) {
        if (oldChildIndex == numberOfChildren() - 1) {
          cursor->setLayoutNode(this);
          cursor->setPosition(LayoutCursor::Position::Right);
        } else {
          cursor->setLayoutNode(childAtIndex(oldChildIndex + 1));
          cursor->setPosition(LayoutCursor::Position::Left);
        }
      } else {
        /* Else place the cursor on the left of the new layout, which is right
         * of the previous sibling or left of the parent. */
        if (oldChildIndex == 0) {
          cursor->setLayoutNode(this);
          cursor->setPosition(LayoutCursor::Position::Left);
        } else {
          cursor->setLayoutNode(childAtIndex(oldChildIndex - 1));
          cursor->setPosition(LayoutCursor::Position::Right);
        }
      }
    }
    bool oldChildRemovedAtMerge = oldChild->isEmpty();
    mergeChildrenAtIndex(static_cast<HorizontalLayoutNode *>(newChild), indexForInsertion + 1, true);
    if (!oldChildRemovedAtMerge) {
      removeChildAtIndex(indexForInsertion);
    }
    return;
  }
  // Else, just replace the child.
  if (cursor != nullptr && !oldWasAncestorOfNewLayout) {
    cursor->setPosition(LayoutCursor::Position::Right);
  }
  LayoutNode::replaceChild(oldChild, newChild);
  if (cursor != nullptr) {
    cursor->setLayoutNode(newChild);
  }
}

void HorizontalLayoutNode::privateRemoveChildAtIndex(int index, bool forceRemove) {
  /* Remove the child before potentially adding an EmptyLayout. Indeed, adding
   * a new child would remove any EmptyLayout surrounding the new child and in
   * the case the child to be removed was an Empty layout, it would result in
   * removing it twice if we remove it afterwards. */
  removeChild(childAtIndex(index));
  /* If the child to remove is at index 0 and its right sibling must have a left
   * sibling (e.g. it is a VerticalOffsetLayout), replace the child with an
   * EmptyLayout instead of removing it. */

  /*if (!forceRemove && index == 0 && numberOfChildren() > 0 && child(0)->mustHaveLeftSibling()) {
    addChildAtIndex(new EmptyLayout(), 0);
  }*/ //TODO
}

int HorizontalLayoutNode::removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft) {
  int currentNumberOfChildren = numberOfChildren();
  assert(index >= 0 && index <= currentNumberOfChildren);
  int newIndex = index;
  /* If empty, remove the child that would be on the right of the inserted
   * layout. */
  if (newIndex < currentNumberOfChildren) {
    LayoutNode * c = childAtIndex(newIndex);
    if (c->isEmpty()) {
      removeChild(c);
      currentNumberOfChildren--;
    }
  }
  /* If empty, remove the child that would be on the left of the inserted
   * layout. */
  if (shouldRemoveOnLeft && newIndex - 1 >= 0 && newIndex - 1 <= currentNumberOfChildren -1) {
    LayoutNode * c = childAtIndex(newIndex - 1);
    if (c->isEmpty()) {
      removeChild(c);
      newIndex = index - 1;
    }
  }
  return newIndex;
}

}
