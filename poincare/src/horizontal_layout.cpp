#include <poincare/horizontal_layout.h>
#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/serialization_helper.h>
#include <algorithm>

namespace Poincare {

// LayoutNode

void HorizontalLayoutNode::moveCursorHorizontally(OMG::NewHorizontalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  LayoutCursor::Position dir = direction.isLeft() ? LayoutCursor::Position::Left : LayoutCursor::Position::Right;
  LayoutCursor::Position oppositeDir = direction.isLeft() ? LayoutCursor::Position::Right : LayoutCursor::Position::Left;

  if (this == cursor->layoutNode()) {
    // Case 1: cursor it at this
    if (cursor->position() == dir) {
      // Case 1.1: position = direction. Ask the parent.
      askParentToMoveCursorHorizontally(direction, cursor, shouldRecomputeLayout);
      return;
    }
    assert(cursor->position() == oppositeDir);
    // Case 1.2: position = opposite direction.
    int childrenCount = numberOfChildren();
    if (childrenCount == 0) {
      // Case 1.2.1: If there are no children, set position to direction and ask the parent
      cursor->setPosition(dir);
      askParentToMoveCursorHorizontally(direction, cursor, shouldRecomputeLayout);
      return;
    }
    // Case 1.2.2: If there is at least one child, set the cursor to the closest child and move in direction
    int closestChildToCursor = direction.isLeft() ? childrenCount - 1 : 0;
    cursor->setLayoutNode(childAtIndex(closestChildToCursor));
    cursor->move(direction, shouldRecomputeLayout);
    return;
  }

  // Case 2: cursor is at a child
  assert(cursor->position() == dir);
  int childIndex = indexOfChild(cursor->layoutNode());
  assert(childIndex >= 0);
  int furthestChildInDirection = direction.isLeft() ? 0 : numberOfChildren() - 1;

  if (childIndex == furthestChildInDirection) {
    // Case 2.1: the child is the last in direction. Ask the parent.
    if (parent() != nullptr) {
      cursor->setLayoutNode(this);
      askParentToMoveCursorHorizontally(direction, cursor, shouldRecomputeLayout);
    }
    return;
  }

  // Case 2.2: the child is not the last in direction. Go to its next sibling in direction and move in direction.
  int step = direction.isLeft() ? -1 : 1;
  cursor->setLayoutNode(childAtIndex(childIndex + step));
  cursor->setPosition(oppositeDir);
  cursor->move(direction, shouldRecomputeLayout);
}

void HorizontalLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  moveCursorHorizontally(OMG::NewDirection::Left(), cursor, shouldRecomputeLayout);
}

void HorizontalLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  moveCursorHorizontally(OMG::NewDirection::Right(), cursor, shouldRecomputeLayout);
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
  return numberOfChildren() > 1 || (numberOfChildren() == 1 && childAtIndex(0)->hasText());
}

// Private

KDSize HorizontalLayoutNode::computeSize(KDFont::Size font) {
  if (numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(font);
    KDCoordinate width = shouldDrawEmptyRectangle() ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
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
  if (numberOfChildren() == 0) {
    return EmptyRectangle::RectangleSize(font).height() / 2;
  }
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

KDRect HorizontalLayoutNode::relativeSelectionRect(int leftIndex, int rightIndex, KDFont::Size font) const {
  HorizontalLayout thisLayout = HorizontalLayout(const_cast<HorizontalLayoutNode *>(this));
  if (numberOfChildren() == 0) {
    assert(leftIndex == 0 && rightIndex == 1);
    return KDRectZero;
  }

  assert(leftIndex >= 0 && rightIndex <= numberOfChildren() && leftIndex < rightIndex);
  int firstSelectedChildIndex = leftIndex;
  int lastSelectedChildIndex = rightIndex - 1;
  LayoutNode * firstSelectedChild = childAtIndex(leftIndex);
  LayoutNode * lastSelectedChild = childAtIndex(rightIndex);

  // Compute the positions
  KDCoordinate selectionXStart = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(firstSelectedChild, font).x();
  KDCoordinate selectionXEnd = const_cast<HorizontalLayoutNode *>(this)->positionOfChild(lastSelectedChild, font).x() + lastSelectedChild->layoutSize(font).width();
  KDCoordinate drawWidth = selectionXEnd - selectionXStart;

  // Compute the height
  if (firstSelectedChildIndex == 0 && lastSelectedChildIndex == numberOfChildren() - 1) {
    return KDRect(KDPointZero, const_cast<HorizontalLayoutNode *>(this)->layoutSize(font));
  }
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = firstSelectedChildIndex; i <= lastSelectedChildIndex; i++) {
    Layout childi = thisLayout.childAtIndex(i);
    KDSize childSize = childi.layoutSize(font);
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, childSize.height() - childi.baseline(font));
    maxAboveBaseline = std::max(maxAboveBaseline, childi.baseline(font));
  }
  return KDRect(KDPoint(selectionXStart, const_cast<HorizontalLayoutNode *>(this)->baseline(font) - maxAboveBaseline), KDSize(drawWidth, maxUnderBaseline + maxAboveBaseline));
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
    thisRef.addOrMergeChildAtIndex(*sibling, newChildIndex, cursor);
    if (layoutWillBeMerged) {
      *sibling = thisRef;
    }
  }
  return false;
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

void HorizontalLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  if (shouldDrawEmptyRectangle()) {
    // If the layout is empty, draw an empty rectangle
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font, m_emptyColor);
  }
}

bool HorizontalLayoutNode::shouldDrawEmptyRectangle() const {
  if (numberOfChildren() > 0) {
    return false;
  }
  LayoutNode * p = parent();
  if (!p || (p && AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(p->type()))) {
    // Never show the empty child of a parenthesis of if has no parent
    return false;
  }
  return m_emptyVisibility == EmptyRectangle::State::Visible;
}

// HorizontalLayout

void HorizontalLayout::addOrMergeChildAtIndex(Layout l, int index, LayoutCursor * cursor) {
  if (l.type() == LayoutNode::Type::HorizontalLayout) {
    mergeChildrenAtIndex(HorizontalLayout(static_cast<HorizontalLayoutNode *>(l.node())), index, cursor);
  } else {
    addChildAtIndex(l, index, numberOfChildren(), cursor);
  }
}

void HorizontalLayout::mergeChildrenAtIndex(HorizontalLayout h, int index, LayoutCursor * cursor) {
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

  // Merge the horizontal layout
  int childrenNumber = h.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    int n = numberOfChildren();
    Layout c = h.childAtIndex(i);
    bool firstAddedChild = (i == 0);
    bool lastAddedChild = (i == childrenNumber - 1);
    bool hasPreviousLayout = newIndex > 0;
    bool hasFollowingLayout = newIndex < n;
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
  static_cast<HorizontalLayoutNode *>(node())->serializeChildrenBetweenIndexes(buffer, bufferSize, Poincare::Preferences::sharedPreferences->displayMode(), Poincare::Preferences::sharedPreferences->numberOfSignificantDigits(), true, firstIndex, lastIndex);
}

}
