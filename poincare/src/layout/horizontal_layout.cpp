#include "horizontal_layout.h"
#include "empty_layout.h"
#include <kandinsky.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * HorizontalLayout::clone() const {
  HorizontalLayout * layout = new HorizontalLayout(const_cast<ExpressionLayout **>(children()), numberOfChildren(), true);
  return layout;
}

void HorizontalLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Left
      && m_parent == nullptr)
  {
    // Case: Left and this is the main layout. Return.
    return;
  }
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right
      && m_parent == nullptr
      && numberOfChildren() == 0)
  {
    // Case: Right and this is the main layout with no children. Return.
    return;
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    int indexOfPointedExpression = indexOfChild(cursor->pointedExpressionLayout());
    if (indexOfPointedExpression >= 0) {
      /* Case: Left of a child.
       * Point Right of the previous child. If there is no previous child, point
       * Left of this. Perform another backspace. */
      if (indexOfPointedExpression == 0) {
        cursor->setPointedExpressionLayout(this);
      } else if (indexOfPointedExpression > 0) {
        cursor->setPointedExpressionLayout(editableChild(indexOfPointedExpression - 1));
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      }
      cursor->performBackspace();
      return;
    }
  }
  assert(cursor->pointedExpressionLayout() == this);
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    // Case: Right. Point to the last child and perform backspace.
    cursor->setPointedExpressionLayout(editableChild(numberOfChildren() - 1));
    cursor->performBackspace();
    return;
  }
  ExpressionLayout::deleteBeforeCursor(cursor);
}

void HorizontalLayout::replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) {
  privateReplaceChild(oldChild, newChild, deleteOldChild, nullptr);
}

void HorizontalLayout::replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  privateReplaceChild(oldChild, newChild, deleteOldChild, cursor);
}

void HorizontalLayout::privateReplaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) {
  bool oldWasAncestorOfNewLayout = false;
  if (newChild->hasAncestor(this)) {
    newChild->editableParent()->detachChild(newChild);
    oldWasAncestorOfNewLayout = true;
  }
  int oldChildIndex = indexOfChild(oldChild);
  if (newChild->isEmpty()) {
    if (numberOfChildren() > 1) {
      /* If the new layout is empty and the horizontal layout has other
       * children, just delete the old child. */
      if (!newChild->hasAncestor(oldChild)) {
        delete newChild;
      }
      removeChildAtIndex(oldChildIndex, deleteOldChild);
      if (cursor == nullptr) {
        return;
      }
      if (oldChildIndex == 0) {
        cursor->setPointedExpressionLayout(this);
        cursor->setPosition(ExpressionLayoutCursor::Position::Left);
        return;
      }
      cursor->setPointedExpressionLayout(editableChild(oldChildIndex -1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return;
    }
    /* If the new layout is empty and it was the only horizontal layout child,
     * replace the horizontal layout with this empty layout (only if this is not
     * the main layout, so only if the layout has a parent). */
    if (m_parent) {
      if (!deleteOldChild) {
        removeChildAtIndex(indexOfChild(oldChild), false);
      }
      if (cursor) {
        replaceWithAndMoveCursor(newChild, true, cursor);
        return;
      }
      replaceWith(newChild, deleteOldChild);
      return;
    }
    /* If this is the main horizontal layout, the old child its only child and
     * the new child is Empty, remove the old child and delete the new child. */
    assert(m_parent == nullptr);
    removeChildAtIndex(0, deleteOldChild);
    delete newChild;
    if (cursor == nullptr) {
      return;
    }
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return;
  }
  /* If the new child is also an horizontal layout, steal the children of the
   * new layout then destroy it. */
  if (newChild->isHorizontal()) {
    int indexForInsertion = indexOfChild(oldChild);
    if (cursor != nullptr) {
      /* If the old layout is not an ancestor of the new layout, or if the
       * cursor was on the right of the new layout, place the cursor on the
       * right of the new layout, which is left of the next sibling or right of
       * the parent. */
      if (!oldWasAncestorOfNewLayout || cursor->position() == ExpressionLayoutCursor::Position::Right) {
        if (oldChildIndex == numberOfChildren() - 1) {
          cursor->setPointedExpressionLayout(this);
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        } else {
          cursor->setPointedExpressionLayout(editableChild(oldChildIndex + 1));
          cursor->setPosition(ExpressionLayoutCursor::Position::Left);
        }
      } else {
        /* Else place the cursor on the left of the new layout, which is right
         * of the previous sibling or left of the parent. */
        if (oldChildIndex == 0) {
          cursor->setPointedExpressionLayout(this);
          cursor->setPosition(ExpressionLayoutCursor::Position::Left);
        } else {
          cursor->setPointedExpressionLayout(editableChild(oldChildIndex - 1));
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        }
      }
    }
    bool oldChildRemovedAtMerge = oldChild->isEmpty();
    mergeChildrenAtIndex(static_cast<HorizontalLayout *>(newChild), indexForInsertion + 1, true);
    if (!oldChildRemovedAtMerge) {
      removeChildAtIndex(indexForInsertion, deleteOldChild);
    }
    return;
  }
  // Else, just replace the child.
  if (cursor != nullptr && !oldWasAncestorOfNewLayout) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  }
  ExpressionLayout::replaceChild(oldChild, newChild, deleteOldChild);
  if (cursor == nullptr) {
    return;
  }
  cursor->setPointedExpressionLayout(newChild);
}

void HorizontalLayout::addOrMergeChildAtIndex(ExpressionLayout * eL, int index, bool removeEmptyChildren) {
  if (eL->isHorizontal()) {
    mergeChildrenAtIndex(static_cast<HorizontalLayout *>(eL), index, removeEmptyChildren);
  } else {
    addChildAtIndex(eL, index);
  }
}

ExpressionLayoutCursor HorizontalLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left. Ask the parent.
  if (cursor.pointedExpressionLayout() == this) {
    if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
      if (m_parent) {
        return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
      }
      return ExpressionLayoutCursor();
    }
    assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
    /* Case: Right.
     * Go to the last child if there is one, and move Left.
     * Else go Left and ask the parent. */
    if (numberOfChildren() < 1) {
      cursor.setPosition(ExpressionLayoutCursor::Position::Left);
      if (m_parent) {
        return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
      }
      return ExpressionLayoutCursor();
    }
    ExpressionLayout * lastChild = editableChild(numberOfChildren()-1);
    assert(lastChild != nullptr);
    cursor.setPointedExpressionLayout(lastChild);
    return lastChild->cursorLeftOf(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Left of a child.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  int childIndex = indexOfChild(cursor.pointedExpressionLayout());
  assert(childIndex >= 0);
  if (childIndex == 0) {
    // Case: the child is the leftmost. Ask the parent.
    if (m_parent) {
      cursor.setPointedExpressionLayout(this);
      return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
    }
    return ExpressionLayoutCursor();
  }
  // Case: the child is not the leftmost. Go to its left sibling and move Left.
  cursor.setPointedExpressionLayout(editableChild(childIndex-1));
  cursor.setPosition(ExpressionLayoutCursor::Position::Right);
  return editableChild(childIndex-1)->cursorLeftOf(cursor, shouldRecomputeLayout);
}

ExpressionLayoutCursor HorizontalLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right. Ask the parent.
  if (cursor.pointedExpressionLayout() == this) {
    if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
      if (m_parent) {
        return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
      }
      return ExpressionLayoutCursor();
    }
    assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
    /* Case: Left.
     * Go to the first child if there is one, and move Right.
     * Else go Right and ask the parent. */
    if (numberOfChildren() < 1) {
      cursor.setPosition(ExpressionLayoutCursor::Position::Right);
      if (m_parent) {
        return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
      }
      return ExpressionLayoutCursor();
    }
    ExpressionLayout * firstChild = editableChild(0);
    assert(firstChild != nullptr);
    cursor.setPointedExpressionLayout(firstChild);
    return firstChild->cursorRightOf(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Right of a child.
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  int childIndex = indexOfChild(cursor.pointedExpressionLayout());
  assert(childIndex >= 0);
  if (childIndex == numberOfChildren() - 1) {
    // Case: the child is the rightmost. Ask the parent.
    if (m_parent) {
      cursor.setPointedExpressionLayout(this);
      return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
    }
    return ExpressionLayoutCursor();
  }
  /* Case: the child is not the rightmost. Go to its right sibling and move
   * Right. */
  cursor.setPointedExpressionLayout(editableChild(childIndex+1));
  cursor.setPosition(ExpressionLayoutCursor::Position::Left);
  return editableChild(childIndex+1)->cursorRightOf(cursor, shouldRecomputeLayout);
}

void HorizontalLayout::addChildrenAtIndex(const ExpressionLayout * const * operands, int numberOfOperands, int indexForInsertion, bool removeEmptyChildren) {
  int newIndex = removeEmptyChildBeforeInsertionAtIndex(indexForInsertion, !operands[0]->mustHaveLeftSibling());
  DynamicLayoutHierarchy::addChildrenAtIndex(operands, numberOfOperands, newIndex, removeEmptyChildren);
}

bool HorizontalLayout::addChildAtIndex(ExpressionLayout * operand, int index) {
  int newIndex = removeEmptyChildBeforeInsertionAtIndex(index, !operand->mustHaveLeftSibling());
  return DynamicLayoutHierarchy::addChildAtIndex(operand, newIndex);
}

void HorizontalLayout::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  privateRemoveChildAtIndex(index, deleteAfterRemoval, false);
}

void HorizontalLayout::mergeChildrenAtIndex(DynamicLayoutHierarchy * eL, int index, bool removeEmptyChildren) {
  /* Before the merge, remove ay empty child that would be on the left or on the
   * right of the inserted layout.
   * If the layout to insert starts with a vertical offset layout, any empty
   * layout child directly on the left of the inserted layout (if there is one)
   * should not be removed: it will be the base for the VerticalOffsetLayout. */
  bool shouldRemoveOnLeft = eL->numberOfChildren() > 0 ? !(eL->child(0)->mustHaveLeftSibling()) : true;
  int newIndex = removeEmptyChildBeforeInsertionAtIndex(index, shouldRemoveOnLeft);
  DynamicLayoutHierarchy::mergeChildrenAtIndex(eL, newIndex, removeEmptyChildren);
}

int HorizontalLayout::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (numberOfChildren() == 0) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[0] = 0;
    return 0;
  }
  return LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "");
}

ExpressionLayoutCursor HorizontalLayout::equivalentCursor(ExpressionLayoutCursor cursor) {
  ExpressionLayoutCursor result;
  ExpressionLayout * newPointedLayout = nullptr;
  ExpressionLayoutCursor::Position newPosition = ExpressionLayoutCursor::Position::Left;
  if (cursor.pointedExpressionLayout() == this) {
    // First or last child, if any
    if(numberOfChildren() == 0) {
      return result;
    } else {
      newPointedLayout = editableChild(cursor.position() == ExpressionLayoutCursor::Position::Left ? 0 : numberOfChildren() - 1);
      newPosition = cursor.position();
    }
  } else {
    // Left or right child
    int indexOfPointedLayout = indexOfChild(cursor.pointedExpressionLayout());
    if (indexOfPointedLayout < 0) {
      return result;
    } else if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
      if (indexOfPointedLayout == 0) {
        newPointedLayout = this;
        newPosition = ExpressionLayoutCursor::Position::Left;
      } else {
        newPointedLayout = editableChild(indexOfPointedLayout - 1);
        newPosition = ExpressionLayoutCursor::Position::Right;
      }
    } else {
      assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
      if (indexOfPointedLayout == numberOfChildren() - 1) {
        newPointedLayout = this;
        newPosition = ExpressionLayoutCursor::Position::Right;
      } else {
        newPointedLayout = editableChild(indexOfPointedLayout + 1);
        newPosition = ExpressionLayoutCursor::Position::Left;
      }
    }
  }
  result.setPointedExpressionLayout(newPointedLayout);
  result.setPosition(newPosition);
  return result;
}

KDSize HorizontalLayout::computeSize() {
  KDCoordinate totalWidth = 0;
  int i = 0;
  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;
  while (ExpressionLayout * c = editableChild(i++)) {
    KDSize childSize = c->size();
    totalWidth += childSize.width();
    if (childSize.height() - c->baseline() > max_under_baseline) {
      max_under_baseline = childSize.height() - c->baseline() ;
    }
    if (c->baseline() > max_above_baseline) {
      max_above_baseline = c->baseline();
    }
  }
  return KDSize(totalWidth, max_under_baseline + max_above_baseline);
}

void HorizontalLayout::computeBaseline() {
  m_baseline = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    if (editableChild(i)->baseline() > m_baseline) {
      m_baseline = editableChild(i)->baseline();
    }
  }
  m_baselined = true;
}

KDPoint HorizontalLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  int index = indexOfChild(child);
  if (index > 0) {
    ExpressionLayout * previousChild = editableChild(index-1);
    assert(previousChild != nullptr);
    x = previousChild->origin().x() + previousChild->size().width();
  }
  y = baseline() - child->baseline();
  return KDPoint(x, y);
}

void HorizontalLayout::privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) {
  // Add the "sibling" as a child.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    int indexForInsertion = 0;
    /* If the first child is empty, remove it before adding the layout, unless
     * the new sibling needs the empty layout as a base. */
    if (numberOfChildren() > 0 && editableChild(0)->isEmpty()) {
      if (sibling->mustHaveLeftSibling()) {
        indexForInsertion = 1;
      } else {
        /* We force the removing of the child even followed by a neighbourg
         * requiring a left sibling as we are about to add a sibling in first
         * position anyway. */
        privateRemoveChildAtIndex(0, true, true);
      }
    }
    if (moveCursor) {
      if (numberOfChildren() > indexForInsertion) {
        cursor->setPointedExpressionLayout(editableChild(indexForInsertion));
      } else {
        cursor->setPointedExpressionLayout(this);
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      }
    }
    addOrMergeChildAtIndex(sibling, indexForInsertion, false);
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // If the last child is empty, remove it before adding the layout.
  int childrenCount = numberOfChildren();
  if (childrenCount > 0 && editableChild(childrenCount - 1)->isEmpty() && !sibling->mustHaveLeftSibling()) {
    /* Force remove the last child. */
    privateRemoveChildAtIndex(childrenCount - 1, true, true);
  }
  addOrMergeChildAtIndex(sibling, numberOfChildren(), false);
  if (moveCursor) {
    cursor->setPointedExpressionLayout(this);
  }
}

void HorizontalLayout::privateRemoveChildAtIndex(int index, bool deleteAfterRemoval, bool forceRemove) {
  /* Remove the child before potentially adding an EmptyLayout. Indeed, adding
   * a new child would remove any EmptyLayout surrounding the new child and in
   * the case the child to be removed was an Empty layout, it would result in
   * removing it twice if we remove it afterwards. */
  DynamicLayoutHierarchy::removeChildAtIndex(index, deleteAfterRemoval);
  /* If the child to remove is at index 0 and its right sibling must have a left
   * sibling (e.g. it is a VerticalOffsetLayout), replace the child with an
   * EmptyLayout instead of removing it. */
  if (!forceRemove && index == 0 && numberOfChildren() > 0 && child(0)->mustHaveLeftSibling()) {
    addChildAtIndex(new EmptyLayout(), 0);
  }
}

int HorizontalLayout::removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft) {
  int newIndex = index;
  /* If empty, remove the child that would be on the right of the inserted
   * layout. */
  if (newIndex < numberOfChildren()
      && child(newIndex)->isEmpty())
  {
    privateRemoveChildAtIndex(newIndex, true, true);
  }
  /* If empty, remove the child that would be on the left of the inserted
   * layout. */
  if (shouldRemoveOnLeft
      && newIndex - 1 >= 0
      && newIndex - 1 <= numberOfChildren() -1
      && child(newIndex - 1)->isEmpty())
  {
    privateRemoveChildAtIndex(newIndex-1, true, true);
    newIndex = index - 1;
  }
  return newIndex;
}

}
