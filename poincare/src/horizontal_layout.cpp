#include <poincare/horizontal_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/serialization_helper.h>
#include <algorithm>

namespace Poincare {

// LayoutNode

void HorizontalLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (this == cursor->layoutNode()) {
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
      cursor->setLayoutNode(childAtIndex(childrenCount-1));
    } else {
      cursor->setPosition(LayoutCursor::Position::Left);
    }
    return cursor->moveLeft(shouldRecomputeLayout);
  }

  // Case: The cursor is Left of a child.
  assert(cursor->position() == LayoutCursor::Position::Left);
  int childIndex = indexOfChild(cursor->layoutNode());
  assert(childIndex >= 0);
  if (childIndex == 0) {
    // Case: the child is the leftmost. Ask the parent.
    if (parent() != nullptr) {
      cursor->setLayoutNode(this);
      return cursor->moveLeft(shouldRecomputeLayout);
    }
    return;
  }
  // Case: the child is not the leftmost. Go to its left sibling and move Left.
  cursor->setLayoutNode(childAtIndex(childIndex-1));
  cursor->setPosition(LayoutCursor::Position::Right);
  cursor->moveLeft(shouldRecomputeLayout);
}

void HorizontalLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (this == cursor->layoutNode()) {
    if (cursor->position() == LayoutCursor::Position::Right) {
      // Case: Right. Ask the parent.
      LayoutNode * parentNode = parent();
      if (parentNode != nullptr) {
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
    cursor->setLayoutNode(firstChild);
    return firstChild->moveCursorRight(cursor, shouldRecomputeLayout);
  }

  // Case: The cursor is Right of a child.
  assert(cursor->position() == LayoutCursor::Position::Right);
  int childIndex = indexOfChild(cursor->layoutNode());
  assert(childIndex >= 0);
  if (childIndex == numberOfChildren() - 1) {
    // Case: the child is the rightmost. Ask the parent.
    LayoutNode * parentNode = parent();
    if (parentNode != nullptr) {
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

LayoutCursor HorizontalLayoutNode::equivalentCursor(LayoutCursor * cursor) {
  if (cursor->layoutNode() == this) {
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

void HorizontalLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  LayoutNode * p = parent();
  if (p == nullptr
      && cursor->layoutNode() == this
      && (cursor->position() == LayoutCursor::Position::Left
        || numberOfChildren() == 0))
  {
    /* Case: Left and this is the main layout or Right and this is the main
     * layout with no children. Return. */
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    int indexOfPointedLayout = indexOfChild(cursor->layoutNode());
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
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Point to the last child and perform backspace.
    cursor->setLayoutNode(childAtIndex(numberOfChildren() - 1));
    cursor->performBackspace();
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

LayoutNode * HorizontalLayoutNode::layoutToPointWhenInserting(Expression * correspondingExpression) {
  assert(correspondingExpression != nullptr);
  if (correspondingExpression->numberOfChildren() > 0) {
    Layout layoutToPointTo = Layout(this).recursivelyMatches(
      [](Poincare::Layout layout) {
        return layout.type() == LayoutNode::Type::LeftParenthesisLayout || layout.isEmpty();
      }
    );
    if (!layoutToPointTo.isUninitialized()) {
      return layoutToPointTo.node();
    }
  }
  return this;
}

int HorizontalLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return serializeChildrenBetweenIndexes(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, false);
}

int HorizontalLayoutNode::serializeChildrenBetweenIndexes(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, bool forceIndexes, int firstIndex, int lastIndex) const {
  if (bufferSize == 0) {
    return -1;
  }
  int childrenCount = numberOfChildren();
  if (childrenCount == 0 || bufferSize == 1) {
    buffer[0] = 0;
    return 0;
  }

  int numberOfChar = 0;
  // Write the children, adding multiplication signs if needed
  int index1 = forceIndexes ? firstIndex : 0;
  int index2 = forceIndexes ? lastIndex + 1 : childrenCount;
  assert(index1 >= 0 && index2 <= childrenCount && index1 <= index2);
  LayoutNode * currentChild = childAtIndex(index1);
  LayoutNode * nextChild = nullptr;
  for (int i = index1; i < index2; i++) {
    // Write the child
    numberOfChar+= currentChild->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (i != childrenCount - 1) {
      nextChild = childAtIndex(i+1);
      // Write the multiplication sign if needed
      LayoutNode::Type nextChildType = nextChild->type();
      if ((nextChildType == LayoutNode::Type::AbsoluteValueLayout
            || nextChildType == LayoutNode::Type::BinomialCoefficientLayout
            || nextChildType == LayoutNode::Type::CeilingLayout
            || nextChildType == LayoutNode::Type::ConjugateLayout
            || nextChildType == LayoutNode::Type::CeilingLayout
            || nextChildType == LayoutNode::Type::FloorLayout
            || nextChildType == LayoutNode::Type::IntegralLayout
            || (nextChildType == LayoutNode::Type::NthRootLayout
              && !static_cast<NthRootLayoutNode *>(nextChild)->isSquareRoot())
            || nextChildType == LayoutNode::Type::ProductLayout
            || nextChildType == LayoutNode::Type::SumLayout)
          && currentChild->canBeOmittedMultiplicationLeftFactor())
      {
        assert(nextChildType != LayoutNode::Type::HorizontalLayout);
        numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize - numberOfChar, '*');
        if (numberOfChar >= bufferSize-1) {
          assert(buffer[bufferSize - 1] == 0);
          return bufferSize - 1;
        }
      }
    }
    currentChild = nextChild;
  }
  if (numberOfChar >= bufferSize-1) {
    assert(buffer[bufferSize - 1] == 0);
    return bufferSize - 1;
  }

  assert(buffer[numberOfChar] == 0);
  return numberOfChar;
}

bool HorizontalLayoutNode::hasText() const {
  if (numberOfChildren() == 0) {
    return false;
  }
  if (numberOfChildren() == 1 && !(const_cast<HorizontalLayoutNode *>(this)->childAtIndex(0)->hasText())) {
    return false;
  }
  return true;
}

// Protected

KDSize HorizontalLayoutNode::computeSize() {
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (LayoutNode * l : children()) {
    KDSize childSize = l->layoutSize();
    totalWidth += childSize.width();
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - l->baseline());
    maxAboveBaseline = std::max(maxAboveBaseline, l->baseline());
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate HorizontalLayoutNode::computeBaseline() {
  KDCoordinate result = 0;
  for (LayoutNode * l : children()) {
    result = std::max(result, l->baseline());
  }
  return result;
}

KDPoint HorizontalLayoutNode::positionOfChild(LayoutNode * l) {
  assert(hasChild(l));
  KDCoordinate x = 0;
  for (LayoutNode * c : children()) {
    if (c == l) {
      break;
    }
    KDSize childSize = c->layoutSize();
    x += childSize.width();
  }
  KDCoordinate y = baseline() - l->baseline();
  return KDPoint(x, y);
}

KDRect HorizontalLayoutNode::relativeSelectionRect(const Layout * selectionStart, const Layout * selectionEnd) const {
  assert(selectionStart != nullptr && !selectionStart->isUninitialized());
  assert(selectionEnd != nullptr && !selectionEnd->isUninitialized());
  HorizontalLayout thisLayout = HorizontalLayout(const_cast<HorizontalLayoutNode *>(this));
  assert(thisLayout.hasChild(*selectionStart));
  assert(thisLayout.hasChild(*selectionEnd));
  assert(thisLayout.indexOfChild(*selectionStart) <= thisLayout.indexOfChild(*selectionEnd));

  // Compute the positions
  KDCoordinate selectionXStart = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(selectionStart->node()).x();
  KDCoordinate selectionXEnd = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(selectionEnd->node()).x() + selectionEnd->layoutSize().width();
  KDCoordinate drawWidth = selectionXEnd - selectionXStart;

  // Compute the height
  int firstSelectedNodeIndex = thisLayout.indexOfChild(*selectionStart);
  int secondSelectedNodeIndex = thisLayout.indexOfChild(*selectionEnd);
  if (firstSelectedNodeIndex == 0 && secondSelectedNodeIndex == numberOfChildren() - 1) {
    return KDRect(KDPointZero, const_cast<HorizontalLayoutNode *>(this)->layoutSize());
  }
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = firstSelectedNodeIndex; i <= secondSelectedNodeIndex; i++) {
    Layout childi = thisLayout.childAtIndex(i);
    KDSize childSize = childi.layoutSize();
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - childi.baseline());
    maxAboveBaseline = std::max(maxAboveBaseline, childi.baseline());
  }
  return KDRect(KDPoint(selectionXStart, const_cast<HorizontalLayoutNode *>(this)->baseline() - maxAboveBaseline), KDSize(drawWidth, maxUnderBaseline + maxAboveBaseline));
}


// Private

bool HorizontalLayoutNode::willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) {
  if (m_numberOfChildren > 0) {
    HorizontalLayout thisRef(this);
    thisRef.removeEmptyChildBeforeInsertionAtIndex(index, currentNumberOfChildren, !l->mustHaveLeftSibling(), cursor);
    *currentNumberOfChildren = thisRef.numberOfChildren();
  }
  return true;
}

bool HorizontalLayoutNode::willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) {
  HorizontalLayout thisRef(this);
  int newChildIndex = cursor->position() == LayoutCursor::Position::Left ? 0 : numberOfChildren();
  thisRef.addOrMergeChildAtIndex(sibling, newChildIndex, true, cursor);
  return false;
}

bool HorizontalLayoutNode::willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) {
  if (!force && numberOfChildren() == 1) {
    assert(childAtIndex(0) == l);
    LayoutNode * p = parent();
    if (p != nullptr) {
      Layout(p).removeChild(HorizontalLayout(this), cursor);
      // WARNING: Do not call "this" afterwards
      return false;
    }
  }
  return true;
}

void HorizontalLayoutNode::didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  /* If the child to remove was at index 0 and its right sibling must have a left
   * sibling (e.g. a VerticalOffsetLayout), add an empty layout at index 0 */

  if (!force && index == 0 && numberOfChildren() > 0 && childAtIndex(0)->mustHaveLeftSibling()) {
    Layout(this).addChildAtIndex(EmptyLayout::Builder(), 0, numberOfChildren(), cursor);
  }
}

bool HorizontalLayoutNode::willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) {
  if (oldChild == newChild) {
    return false;
  }
  if (force) {
    return true;
  }
  HorizontalLayout thisRef(this);
  int oldChildIndex = indexOfChild(oldChild);
  if (newChild->isEmpty()) {
    if (numberOfChildren() > 1) {
      /* If the new layout is empty and the horizontal layout has other
       * children, just remove the old child. */
      thisRef.removeChild(oldChild, nullptr);
      if (cursor != nullptr) {
        if (oldChildIndex == 0) {
          cursor->setLayout(thisRef);
          cursor->setPosition(LayoutCursor::Position::Left);
        } else {
          cursor->setLayout(thisRef.childAtIndex(oldChildIndex -1));
          cursor->setPosition(LayoutCursor::Position::Right);
        }
      }
      return false;
    }
    /* The old layout was the only horizontal layout child, so if this has a
     * a parent, replace this with the new empty layout. */
    LayoutNode * p = parent();
    if (p != nullptr) {
      thisRef.replaceWith(newChild, cursor);
      // WARNING: do not call "this" afterwards
      return false;
    }
    /* This is the main horizontal layout, the old child is its only child and
     * the new child is Empty: remove the old child and delete the new child. */
    assert(p == nullptr);
    thisRef.removeChild(oldChild, nullptr);
    // WARNING: do not call "this" afterwards
    if (cursor != nullptr) {
      cursor->setLayout(thisRef);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
    return false;
  }
  /* If the new child is also an horizontal layout, steal the children of the
   * new layout then destroy it. */
  bool oldWasAncestorOfNewLayout = newChild->hasAncestor(oldChild, false);
  if (newChild->type() == LayoutNode::Type::HorizontalLayout) {
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
    thisRef.mergeChildrenAtIndex(HorizontalLayout(static_cast<HorizontalLayoutNode *>(newChild)), indexForInsertion + 1, true);
    // WARNING: do not call "this" afterwards
    if (!oldChildRemovedAtMerge) {
      thisRef.removeChildAtIndex(indexForInsertion, cursor);
    }
    return false;
  }
  // Else, just replace the child.
  if (cursor != nullptr && !oldWasAncestorOfNewLayout) {
    cursor->setPosition(LayoutCursor::Position::Right);
  }
  return true;
}

void HorizontalLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Fill the background
  KDSize s = layoutSize();
  ctx->fillRect(KDRect(p, s), backgroundColor);
  // Fill the selection background
  HorizontalLayout thisLayout = HorizontalLayout(this);
  bool childrenAreSelected = selectionStart != nullptr && selectionEnd != nullptr
    && !selectionStart->isUninitialized() && !selectionStart->isUninitialized()
    && thisLayout.hasChild(*selectionStart);
  if (childrenAreSelected) {
    assert(thisLayout.hasChild(*selectionEnd));
    KDRect selectionRectangle = HorizontalLayout(this).relativeSelectionRect(selectionStart, selectionEnd);
    ctx->fillRect(selectionRectangle.translatedBy(p), selectionColor);
  }
}

// HorizontalLayout

void HorizontalLayout::addOrMergeChildAtIndex(Layout l, int index, bool removeEmptyChildren, LayoutCursor * cursor) {
  if (l.type() == LayoutNode::Type::HorizontalLayout) {
    mergeChildrenAtIndex(HorizontalLayout(static_cast<HorizontalLayoutNode *>(l.node())), index, removeEmptyChildren, cursor);
  } else {
    addChildAtIndex(l, index, numberOfChildren(), cursor, removeEmptyChildren);
  }
}

void HorizontalLayout::addChildAtIndex(Layout l, int index, int currentNumberOfChildren, LayoutCursor * cursor, bool removeEmptyChildren) {
  if (!removeEmptyChildren
      || !l.isEmpty()
      || numberOfChildren() == 0
      || (index < numberOfChildren()
        && childAtIndex(index).mustHaveLeftSibling()))
  {
    Layout::addChildAtIndex(l, index, currentNumberOfChildren, cursor);
  }
}

void HorizontalLayout::mergeChildrenAtIndex(HorizontalLayout h, int index, bool removeEmptyChildren, LayoutCursor * cursor) {
  int newIndex = index;

  // Remove h if it is a child
  int indexOfh = indexOfChild(h);
  if (indexOfh >= 0) {
    removeChildAtIndexInPlace(indexOfh);
    if (indexOfh < newIndex) {
      newIndex--;
    }
  }

  if (h.numberOfChildren() == 0) {
    return;
  }

  /* Remove any empty child that would be next to the inserted layout.
   * If the layout to insert starts with a vertical offset layout, any empty
   * layout child directly on the left of the inserted layout (if there is one)
   * should not be removed: it will be the base for the VerticalOffsetLayout. */
  bool shouldRemoveOnLeft = h.numberOfChildren() == 0 ? true : !(h.childAtIndex(0).mustHaveLeftSibling());
  removeEmptyChildBeforeInsertionAtIndex(&newIndex, nullptr, shouldRemoveOnLeft);
  assert(newIndex >= 0 && newIndex <= numberOfChildren());

  // Prepare the next cursor position
  Layout nextPointedLayout;
  LayoutCursor::Position nextPosition = LayoutCursor::Position::Left;
  if (newIndex < numberOfChildren()) {
    nextPointedLayout = childAtIndex(newIndex);
    nextPosition = LayoutCursor::Position::Left;
  } else {
    nextPointedLayout = *this;
    nextPosition = LayoutCursor::Position::Right;
  }

  // Merge the horizontal layout
  for (int i = 0; i < h.numberOfChildren(); i++) {
    if (i == 0
        && h.childAtIndex(0).isEmpty()
        && h.numberOfChildren() > 1
        && h.childAtIndex(1).mustHaveLeftSibling()
        && newIndex > 0)
    {
      /* If the first added child is Empty because its right sibling needs a
       * left sibling, remove it if any previous child could be such a left
       * sibling. */
      continue;
    }
    if (!removeEmptyChildren
        || !h.childAtIndex(i).isEmpty()
        || (numberOfChildren() > 0
          && childAtIndex(0).mustHaveLeftSibling())
        || (i < h.numberOfChildren()-1 && h.childAtIndex(i+1).mustHaveLeftSibling()))
    {
      addChildAtIndexInPlace(h.childAtIndex(i), newIndex, numberOfChildren());
      newIndex++;
    }
  }

  // Set the cursor
  if (cursor != nullptr) {
    cursor->setLayout(nextPointedLayout);
    cursor->setPosition(nextPosition);
  }
}

Layout HorizontalLayout::squashUnaryHierarchyInPlace() {
  if (numberOfChildren() == 1) {
    Layout child = childAtIndex(0);
    replaceWithInPlace(child);
    return child;
  }
  return *this;
}

void HorizontalLayout::serializeChildren(int firstIndex, int lastIndex, char * buffer, int bufferSize) {
  static_cast<HorizontalLayoutNode *>(node())->serializeChildrenBetweenIndexes(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits(), true, firstIndex, lastIndex);
}

void HorizontalLayout::removeEmptyChildBeforeInsertionAtIndex(int * index, int * currentNumberOfChildren, bool shouldRemoveOnLeft, LayoutCursor * cursor) {
  int childrenCount = currentNumberOfChildren == nullptr ? numberOfChildren() : *currentNumberOfChildren;
  assert(*index >= 0 && *index <= childrenCount);
  /* If empty, remove the child that would be on the right of the inserted
   * layout. */
  if (*index < childrenCount) {
    Layout c = childAtIndex(*index);
    if (c.isEmpty()) {
      removeChild(c, cursor, true);
      childrenCount--;
      if (currentNumberOfChildren != nullptr) {
        *currentNumberOfChildren = childrenCount;
      }
    }
  }
  /* If empty, remove the child that would be on the left of the inserted
   * layout. */
  if (shouldRemoveOnLeft && *index - 1 >= 0 && *index - 1 < childrenCount) {
    Layout c = childAtIndex(*index - 1);
    if (c.isEmpty()) {
      removeChild(c, cursor, true);
      *index = *index - 1;
      childrenCount--;
      if (currentNumberOfChildren != nullptr) {
        *currentNumberOfChildren = childrenCount;
      }
    }
  }
}

}
