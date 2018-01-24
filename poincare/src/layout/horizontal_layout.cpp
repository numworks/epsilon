#include "horizontal_layout.h"
#include "empty_visible_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * HorizontalLayout::clone() const {
  HorizontalLayout * layout = new HorizontalLayout(const_cast<ExpressionLayout **>(children()), numberOfChildren(), true);
  return layout;
}

void HorizontalLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Left
      && m_parent == nullptr)
  {
    // Case: Left and this is the main layout.
    // Return.
    return;
  }
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right
      && m_parent == nullptr
      && numberOfChildren() == 0)
  {
    // Case: Right and this is the main layout with no children.
    // Return.
    return;
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    int indexOfPointedExpression = indexOfChild(cursor->pointedExpressionLayout());
    if (indexOfPointedExpression >= 0) {
      // Case: Left of a child.
      // Point Right of the previous child. If there is no previous child, point
      // Left of this. Perform another backspace.
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
    // Case: Right.
    // Point to the last child and perform backspace.
    cursor->setPointedExpressionLayout(editableChild(numberOfChildren() - 1));
    cursor->performBackspace();
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
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
      // If the new layout is empty and the horizontal layout has other
      // children, just delete the old child.
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
    // If the new layout is empty and it was the only horizontal layout child,
    // replace the horizontal layout with this empty layout (only if this is not
    // the main layout, so only if the layout has a parent).
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
    // If this is the main horizontal layout, the old child its only child and
    // the new child is Empty, remove the old child and delete the new child.
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
  // If the new child is also an horizontal layout, steal the children of the
  // new layout then destroy it.
  if (newChild->isHorizontal()) {
    int indexForInsertion = indexOfChild(oldChild);
    if (cursor != nullptr) {
      // If the old layout is not an ancestor of the new layout, or if the
      // cursor was on the right of the new layout, place the cursor on the
      // right of the new layout, which is left of the next brother or right of
      // the parent.
      if (!oldWasAncestorOfNewLayout || cursor->position() == ExpressionLayoutCursor::Position::Right) {
        if (oldChildIndex == numberOfChildren() - 1) {
          cursor->setPointedExpressionLayout(this);
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        } else {
          cursor->setPointedExpressionLayout(editableChild(oldChildIndex + 1));
          cursor->setPosition(ExpressionLayoutCursor::Position::Left);
        }
      } else {
        // Else place the cursor on the left of the new layout, which is right
        // of the previous brother or left of the parent.
        if (oldChildIndex == 0) {
          cursor->setPointedExpressionLayout(this);
          cursor->setPosition(ExpressionLayoutCursor::Position::Left);
        } else {
          cursor->setPointedExpressionLayout(editableChild(oldChildIndex - 1));
          cursor->setPosition(ExpressionLayoutCursor::Position::Right);
        }
      }
    }
    mergeChildrenAtIndex(static_cast<HorizontalLayout *>(newChild), indexForInsertion + 1, true);
    removeChildAtIndex(indexForInsertion, deleteOldChild);
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
  int newIndex = index;
  if (numberOfChildren() > 0 && child(0)->isEmpty()) {
    removeChildAtIndex(0, true);
    newIndex = index == 0 ? 0 : index - 1;
  }
  if (eL->isHorizontal()) {
    mergeChildrenAtIndex(static_cast<HorizontalLayout *>(eL), newIndex, removeEmptyChildren);
    return;
  }
  addChildAtIndex(eL, newIndex);
}

bool HorizontalLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left.
  // Ask the parent.
  if (cursor->pointedExpressionLayout() == this) {
    if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
      if (m_parent) {
        return m_parent->moveLeft(cursor, shouldRecomputeLayout);
      }
      return false;
    }
    assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
    // Case: Right.
    // Go to the last child if there is one, and move Left.
    // Else go Left and ask the parent.
    if (numberOfChildren() < 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Left);
      if (m_parent) {
        return m_parent->moveLeft(cursor, shouldRecomputeLayout);
      }
      return false;
    }
    ExpressionLayout * lastChild = editableChild(numberOfChildren()-1);
    assert(lastChild != nullptr);
    cursor->setPointedExpressionLayout(lastChild);
    return lastChild->moveLeft(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Left of a child.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  assert(childIndex >= 0);
  if (childIndex == 0) {
    // Case: the child is the leftmost.
    // Ask the parent.
    if (m_parent) {
      cursor->setPointedExpressionLayout(this);
      return m_parent->moveLeft(cursor, shouldRecomputeLayout);
    }
    return false;
  }
  // Case: the child is not the leftmost.
  // Go to its left brother and move Left.
  cursor->setPointedExpressionLayout(editableChild(childIndex-1));
  cursor->setPosition(ExpressionLayoutCursor::Position::Right);
  return editableChild(childIndex-1)->moveLeft(cursor, shouldRecomputeLayout);
}

bool HorizontalLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right.
  // Ask the parent.
  if (cursor->pointedExpressionLayout() == this) {
    if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
      if (m_parent) {
        return m_parent->moveRight(cursor, shouldRecomputeLayout);
      }
      return false;
    }
    assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
    // Case: Left.
    // Go to the first child if there is one, and move Right.
    // Else go Right and ask the parent.
    if (numberOfChildren() < 1) {
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      if (m_parent) {
        return m_parent->moveRight(cursor, shouldRecomputeLayout);
      }
      return false;
    }
    ExpressionLayout * firstChild = editableChild(0);
    assert(firstChild != nullptr);
    cursor->setPointedExpressionLayout(firstChild);
    return firstChild->moveRight(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Right of a child.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  int childIndex = indexOfChild(cursor->pointedExpressionLayout());
  assert(childIndex >= 0);
  if (childIndex == numberOfChildren() - 1) {
    // Case: the child is the rightmost.
    // Ask the parent.
    if (m_parent) {
      cursor->setPointedExpressionLayout(this);
      return m_parent->moveRight(cursor, shouldRecomputeLayout);
    }
    return false;
  }
  // Case: the child is not the rightmost.
  // Go to its right brother and move Right.
  cursor->setPointedExpressionLayout(editableChild(childIndex+1));
  cursor->setPosition(ExpressionLayoutCursor::Position::Left);
  return editableChild(childIndex+1)->moveRight(cursor, shouldRecomputeLayout);
}

bool HorizontalLayout::moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  return moveVertically(ExpressionLayout::VerticalDirection::Up, cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

bool HorizontalLayout::moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  return moveVertically(ExpressionLayout::VerticalDirection::Down, cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

void HorizontalLayout::removeChildAtIndex(int index, bool deleteAfterRemoval) {
  // If the child to remove is at index 0 and its right brother must have a left
  // brother (e.g. it is a VerticalOffsetLayout), replace the child with an
  // EmptyVisibleLayout instead of removing it.
  if (index == 0 && numberOfChildren() > 1 && child(1)->mustHaveLeftBrother()) {
    addChildAtIndex(new EmptyVisibleLayout(), index + 1);
  }
  DynamicLayoutHierarchy::removeChildAtIndex(index, deleteAfterRemoval);
}

int HorizontalLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (numberOfChildren() == 0) {
    if (bufferSize == 0) {
      return -1;
    }
    buffer[0] = 0;
    return 0;
  }
  return LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "");
}

bool HorizontalLayout::isEmpty() const {
  if (m_numberOfChildren == 1 && child(0)->isEmpty())
  {
    return true;
  }
  return false;
}

void HorizontalLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
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

void HorizontalLayout::privateAddBrother(ExpressionLayoutCursor * cursor, ExpressionLayout * brother, bool moveCursor) {
  // Add the "brother" as a child.
  // If there is only one empty child, remove it before adding the layout.
  if (numberOfChildren() == 1 && editableChild(0)->isEmpty()) {
    removeChildAtIndex(0, true);
  }
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    if (moveCursor) {
      if (numberOfChildren() > 0) {
        cursor->setPointedExpressionLayout(editableChild(0));
      } else {
        cursor->setPointedExpressionLayout(this);
        cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      }
    }
    addChildAtIndex(brother, 0);
    return;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  addChildAtIndex(brother, numberOfChildren());
  if (moveCursor) {
    cursor->setPointedExpressionLayout(this);
  }
}

bool HorizontalLayout::moveVertically(ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // Prevent looping fom child to parent
  if (previousPreviousLayout == this) {
    if (direction == ExpressionLayout::VerticalDirection::Up) {
      return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
    }
    assert(direction == ExpressionLayout::VerticalDirection::Down);
    return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
  }
  // If the cursor Left or Right of a child, try moving it up from its brother.
  int previousLayoutIndex = indexOfChild(previousLayout);
  if (previousLayoutIndex > -1) {
    ExpressionLayout * brother = nullptr;
    ExpressionLayoutCursor::Position newPosition = ExpressionLayoutCursor::Position::Right;
    if (cursor->position() == ExpressionLayoutCursor::Position::Left && previousLayoutIndex > 0) {
      brother = editableChild(previousLayoutIndex - 1);
      newPosition = ExpressionLayoutCursor::Position::Right;
    }
    if (cursor->position() == ExpressionLayoutCursor::Position::Right && previousLayoutIndex < numberOfChildren() - 1) {
      brother = editableChild(previousLayoutIndex + 1);
      newPosition = ExpressionLayoutCursor::Position::Left;
    }
    if (brother && cursor->positionIsEquivalentTo(brother, newPosition)) {
      ExpressionLayout * previousPointedLayout = cursor->pointedExpressionLayout();
      ExpressionLayoutCursor::Position previousPosition = cursor->position();
      cursor->setPointedExpressionLayout(brother);
      cursor->setPosition(newPosition);
      if (direction == ExpressionLayout::VerticalDirection::Up && brother->moveUp(cursor, shouldRecomputeLayout, this, previousLayout)) {
        return true;
      }
      if (direction == ExpressionLayout::VerticalDirection::Down && brother->moveDown(cursor, shouldRecomputeLayout, this, previousLayout)) {
        return true;
      }
      cursor->setPointedExpressionLayout(previousPointedLayout);
      cursor->setPosition(previousPosition);
    }
  }
  if (direction == ExpressionLayout::VerticalDirection::Up) {
    return ExpressionLayout::moveUp(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
  }
  assert(direction == ExpressionLayout::VerticalDirection::Down);
  return ExpressionLayout::moveDown(cursor, shouldRecomputeLayout, previousLayout, previousPreviousLayout);
}

}
