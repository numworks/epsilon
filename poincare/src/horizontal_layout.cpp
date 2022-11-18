#include <poincare/horizontal_layout.h>
#include <poincare/autocompleted_bracket_pair_layout.h>
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

LayoutNode * HorizontalLayoutNode::layoutToPointWhenInserting(Expression * correspondingExpression, bool * forceCursorLeftOfText) {
  assert(correspondingExpression != nullptr);
  if (correspondingExpression->isUninitialized() || correspondingExpression->numberOfChildren() > 0) {
    Layout layoutToPointTo = Layout(this).recursivelyMatches(
      [](Poincare::Layout layout) {
        return AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(layout.type())
            || layout.isEmpty() ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
      }
    );
    if (!layoutToPointTo.isUninitialized()) {
      if (!layoutToPointTo.isEmpty()) {
        layoutToPointTo = layoutToPointTo.childAtIndex(0);
        if (forceCursorLeftOfText) {
          *forceCursorLeftOfText = true;
        }
      }
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
    return bufferSize-1;
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
    assert(currentChild);
    numberOfChar+= currentChild->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
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
            || nextChildType == LayoutNode::Type::LetterAWithSubAndSuperscriptLayout
            || nextChildType == LayoutNode::Type::LetterCWithSubAndSuperscriptLayout
            || (nextChildType == LayoutNode::Type::NthRootLayout
              && !static_cast<NthRootLayoutNode *>(nextChild)->isSquareRoot())
            || nextChildType == LayoutNode::Type::ProductLayout
            || nextChildType == LayoutNode::Type::SumLayout
            || nextChildType == LayoutNode::Type::VectorNormLayout)
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

KDSize HorizontalLayoutNode::computeSize(KDFont::Size font) {
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (LayoutNode * l : children()) {
    KDSize childSize = l->layoutSize(font);
    totalWidth += childSize.width();
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - l->baseline(font));
    maxAboveBaseline = std::max(maxAboveBaseline, l->baseline(font));
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate HorizontalLayoutNode::computeBaseline(KDFont::Size font) {
  KDCoordinate result = 0;
  for (LayoutNode * l : children()) {
    result = std::max(result, l->baseline(font));
  }
  return result;
}

KDPoint HorizontalLayoutNode::positionOfChild(LayoutNode * l, KDFont::Size font) {
  assert(hasChild(l));
  KDCoordinate x = 0;
  for (LayoutNode * c : children()) {
    if (c == l) {
      break;
    }
    KDSize childSize = c->layoutSize(font);
    x += childSize.width();
  }
  KDCoordinate y = baseline(font) - l->baseline(font);
  return KDPoint(x, y);
}

KDRect HorizontalLayoutNode::relativeSelectionRect(const Layout * selectionStart, const Layout * selectionEnd, KDFont::Size font) const {
  assert(selectionStart != nullptr && !selectionStart->isUninitialized());
  assert(selectionEnd != nullptr && !selectionEnd->isUninitialized());
  HorizontalLayout thisLayout = HorizontalLayout(const_cast<HorizontalLayoutNode *>(this));
  assert(thisLayout.hasChild(*selectionStart));
  assert(thisLayout.hasChild(*selectionEnd));
  assert(thisLayout.indexOfChild(*selectionStart) <= thisLayout.indexOfChild(*selectionEnd));

  // Compute the positions
  KDCoordinate selectionXStart = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(selectionStart->node(), font).x();
  KDCoordinate selectionXEnd = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(selectionEnd->node(), font).x() + selectionEnd->layoutSize(font).width();
  KDCoordinate drawWidth = selectionXEnd - selectionXStart;

  // Compute the height
  int firstSelectedNodeIndex = thisLayout.indexOfChild(*selectionStart);
  int secondSelectedNodeIndex = thisLayout.indexOfChild(*selectionEnd);
  if (firstSelectedNodeIndex == 0 && secondSelectedNodeIndex == numberOfChildren() - 1) {
    return KDRect(KDPointZero, const_cast<HorizontalLayoutNode *>(this)->layoutSize(font));
  }
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = firstSelectedNodeIndex; i <= secondSelectedNodeIndex; i++) {
    Layout childi = thisLayout.childAtIndex(i);
    KDSize childSize = childi.layoutSize(font);
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - childi.baseline(font));
    maxAboveBaseline = std::max(maxAboveBaseline, childi.baseline(font));
  }
  return KDRect(KDPoint(selectionXStart, const_cast<HorizontalLayoutNode *>(this)->baseline(font) - maxAboveBaseline), KDSize(drawWidth, maxUnderBaseline + maxAboveBaseline));
}


// Private

bool HorizontalLayoutNode::willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) {
  if (m_numberOfChildren > 0) {
    HorizontalLayout thisRef(this);
    thisRef.removeEmptyChildBeforeInsertionAtIndex(index, currentNumberOfChildren, !l->mustHaveLeftSibling(), !l->mustHaveRightSibling(), cursor);
    *currentNumberOfChildren = thisRef.numberOfChildren();
  }
  return true;
}

bool HorizontalLayoutNode::willAddSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) {
  HorizontalLayout thisRef(this);
  int nChildren = numberOfChildren();
  int newChildIndex, siblingIndex;
  if (cursor->position() == LayoutCursor::Position::Left) {
    newChildIndex = siblingIndex = 0;
  } else {
    newChildIndex = nChildren;
    siblingIndex = nChildren - 1;
  }
  if (nChildren == 0 || childAtIndex(siblingIndex)->willAddSibling(cursor, sibling, moveCursor)) {
    bool layoutWillBeMerged = sibling->type() == LayoutNode::Type::HorizontalLayout;
    thisRef.addOrMergeChildAtIndex(*sibling, newChildIndex, true, cursor);
    if (layoutWillBeMerged) {
      *sibling = thisRef;
    }
  }
  return false;
}

int HorizontalLayoutNode::willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) {
  if (!force && numberOfChildren() == 1) {
    assert(childAtIndex(0) == l);
    LayoutNode * p = parent();
    if (p != nullptr) {
      return Layout(p).removeChild(HorizontalLayout(this), cursor);
      // WARNING: Do not call "this" afterwards
    }
  }
  return -1;
}

int HorizontalLayoutNode::didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  int currentNumberOfChildren = numberOfChildren();
  if (force || currentNumberOfChildren == 0) {
    return 0;
  }

  /* If the removed child was the last valid sibling of a layout that requires
   * a left or right sibling, add an empty layout. */
  if ((index == 0 && childAtIndex(index)->mustHaveLeftSibling())
   || (index == currentNumberOfChildren && childAtIndex(index - 1)->mustHaveRightSibling())
   /* Both children relying on each other for sizes and baselines causes inifinite loops. */
   || (index < currentNumberOfChildren && index > 0 && childAtIndex(index - 1)->mustHaveRightSibling() && childAtIndex(index)->mustHaveLeftSibling()))
  {
    Layout(this).addChildAtIndex(EmptyLayout::Builder(), index, currentNumberOfChildren, cursor);
    return 0;
  }

  /* If an empty layout was required by a now deleted sibling, remove it. */
  int emptyChildIndex;
  if (index < currentNumberOfChildren && childAtIndex(index)->type() == Type::EmptyLayout) {
    emptyChildIndex = index;
  } else if (index > 0 && childAtIndex(index - 1)->type() == Type::EmptyLayout) {
    emptyChildIndex = index - 1;
  } else {
    return 0;
  }
  if (!((emptyChildIndex > 0 && childAtIndex(emptyChildIndex - 1)->mustHaveRightSibling())
     || (emptyChildIndex + 1 < currentNumberOfChildren && childAtIndex(emptyChildIndex + 1)->mustHaveLeftSibling())))
  {
    bool deletedSiblingIsLeftOfLayout = emptyChildIndex < index;
    /* This assert ensure we do not have to account for anything deleted right
     * of emptyChildIndex. */
    assert(!deletedSiblingIsLeftOfLayout || emptyChildIndex == index - 1);
    return deletedSiblingIsLeftOfLayout + Layout(this).removeChild(Layout(childAtIndex(emptyChildIndex)), cursor);
  }
  return 0;
}

static void makePermanentIfBracket(LayoutNode * l, bool hasLeftSibling, bool hasRightSibling) {
  if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(l->type())) {
    AutocompletedBracketPairLayoutNode * bracket = static_cast<AutocompletedBracketPairLayoutNode *>(l);
    if (hasLeftSibling) {
      bracket->makePermanent(AutocompletedBracketPairLayoutNode::Side::Left);
    }
    if (hasRightSibling) {
      bracket->makePermanent(AutocompletedBracketPairLayoutNode::Side::Right);
    }
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
      assert(numberOfChildren() > 0);
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
    Layout oldChildRef(oldChild);
    HorizontalLayout newChildRef(static_cast<HorizontalLayoutNode *>(newChild));
    if (!oldChildRemovedAtMerge) {
      assert(!cursor || cursor->layout().node() != oldChild);
      // Do not alter the cursor. It should already be well-placed
      indexForInsertion -= thisRef.removeChild(oldChildRef, nullptr, true);
    }
    thisRef.mergeChildrenAtIndex(newChildRef, indexForInsertion, true);
    // WARNING: do not call "this" afterwards
    return false;
  }
  // Else, just replace the child.
  makePermanentIfBracket(newChild, oldChildIndex > 0, oldChildIndex < numberOfChildren() - 1);
  return true;
}

void HorizontalLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Fill the selection background
  HorizontalLayout thisLayout = HorizontalLayout(this);
  bool childrenAreSelected = selectionStart != nullptr && selectionEnd != nullptr
    && !selectionStart->isUninitialized() && !selectionEnd->isUninitialized()
    && thisLayout.hasChild(*selectionStart);
  if (childrenAreSelected) {
    assert(thisLayout.hasChild(*selectionEnd));
    KDRect selectionRectangle = HorizontalLayout(this).relativeSelectionRect(selectionStart, selectionEnd, font);
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
  if (l.isEmpty()
      && removeEmptyChildren
      && numberOfChildren() > 0
      && (index == numberOfChildren() || !childAtIndex(index).mustHaveLeftSibling())
      && (index == 0 || !childAtIndex(index - 1).mustHaveRightSibling()))
  {
    return;
  }

  if ((index == 0 && l.mustHaveLeftSibling()) || (index == numberOfChildren() && l.mustHaveRightSibling())) {
    // If a layout must have left and right siblings, this if should be split
    assert(l.mustHaveLeftSibling() != l.mustHaveRightSibling());
    addChildAtIndex(EmptyLayout::Builder(), index, numberOfChildren(), cursor, false);
    assert(childAtIndex(index).type() == LayoutNode::Type::EmptyLayout);
    index += l.mustHaveLeftSibling();
  }

  currentNumberOfChildren = numberOfChildren();

  makePermanentIfBracket(l.node(), index > 0, index < currentNumberOfChildren - 1);
  Layout::addChildAtIndex(l, index, currentNumberOfChildren, cursor);
}

void HorizontalLayout::mergeChildrenAtIndex(HorizontalLayout h, int index, bool removeEmptyChildren, LayoutCursor * cursor) {
  int newIndex = index;
  bool margin = h.node()->leftMargin() > 0;
  bool marginIsLocked = h.node()->marginIsLocked();
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

  if (index > 1) {
    makePermanentIfBracket(childAtIndex(index - 1).node(), index > 2, true);
  }
  if (index < numberOfChildren()) {
    makePermanentIfBracket(childAtIndex(index).node(), true, index < numberOfChildren() - 1);
  }

  /* Remove any empty child that would be next to the inserted layout.
   * If the layout to insert starts with a vertical offset layout, any empty
   * layout child directly on the left of the inserted layout (if there is one)
   * should not be removed: it will be the base for the VerticalOffsetLayout,
   * and conversely for a VerticalOffsetLayout that needs a base on its right. */
  int childrenNumber = h.numberOfChildren();
  bool shouldRemoveOnLeft = childrenNumber == 0 ? true : !(h.childAtIndex(0).mustHaveLeftSibling());
  bool shouldRemoveOnRight = childrenNumber == 0 ? true : !(h.childAtIndex(childrenNumber - 1).mustHaveRightSibling());
  removeEmptyChildBeforeInsertionAtIndex(&newIndex, nullptr, shouldRemoveOnLeft, shouldRemoveOnRight);
  assert(newIndex >= 0 && newIndex <= numberOfChildren());

  // Merge the horizontal layout
  childrenNumber = h.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    int n = numberOfChildren();
    Layout c = h.childAtIndex(i);
    bool firstAddedChild = (i == 0);
    bool lastAddedChild = (i == childrenNumber - 1);
    bool hasPreviousLayout = newIndex > 0;
    bool hasFollowingLayout = newIndex < n;
    if (c.isEmpty()) {
      bool nextInsertedLayoutMustHaveLeftSibling = (!lastAddedChild && h.childAtIndex(i + 1).mustHaveLeftSibling());
      bool followingLayoutMustHaveLeftSibling = (hasFollowingLayout && childAtIndex(newIndex).mustHaveLeftSibling());
      bool previousLayoutMustHaveRightSibling = (hasPreviousLayout && childAtIndex(newIndex - 1).mustHaveRightSibling());
      /* Remove empty layout if :
       * - The first added child is Empty because its right sibling needs a left
       *   sibling, and the previous layout could be such a left sibling. */
      if ((firstAddedChild && !lastAddedChild && nextInsertedLayoutMustHaveLeftSibling && hasPreviousLayout && !previousLayoutMustHaveRightSibling)
      /* - The last added child is Empty because its left sibling needs a right
       *   sibling, and the following layout could be such a right sibling. */
       || (lastAddedChild && !firstAddedChild && previousLayoutMustHaveRightSibling && hasFollowingLayout && !followingLayoutMustHaveLeftSibling)
      /* - No sibling layout needs an empty layout. */
       || (removeEmptyChildren && !previousLayoutMustHaveRightSibling && !nextInsertedLayoutMustHaveLeftSibling && !followingLayoutMustHaveLeftSibling)) {
        continue;
      }
    }
    makePermanentIfBracket(c.node(), hasPreviousLayout, hasFollowingLayout || !lastAddedChild);
    addChildAtIndexInPlace(c, newIndex, n);
    if (firstAddedChild) {
      LayoutNode * l = childAtIndex(newIndex).node();
      l->setMargin(margin);
      l->lockMargin(marginIsLocked);
    }
    newIndex++;
  }

  // Set the cursor
  if (cursor != nullptr) {
    if (newIndex > 0) {
      assert(newIndex <= numberOfChildren());
      cursor->setLayout(childAtIndex(newIndex - 1));
      cursor->setPosition(LayoutCursor::Position::Right);
    } else {
      cursor->setLayout(*this);
      cursor->setPosition(LayoutCursor::Position::Left);
    }
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

void HorizontalLayout::removeEmptyChildBeforeInsertionAtIndex(int * index, int * currentNumberOfChildren, bool shouldRemoveOnLeft, bool shouldRemoveOnRight, LayoutCursor * cursor) {
  int childrenCount = currentNumberOfChildren == nullptr ? numberOfChildren() : *currentNumberOfChildren;
  assert(*index >= 0 && *index <= childrenCount);
  /* If empty, remove the child that would be on the right of the inserted
   * layout. */
  if (shouldRemoveOnRight && *index < childrenCount) {
    Layout c = childAtIndex(*index);
    if (c.isEmpty()) {
      removeChild(c, cursor, true);
      childrenCount = numberOfChildren();
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
      *index -= 1 + removeChild(c, cursor, true);
      if (currentNumberOfChildren != nullptr) {
        *currentNumberOfChildren = numberOfChildren();
      }
      assert(*index >= 0 && *index <= numberOfChildren());
    }
  }
}

}
