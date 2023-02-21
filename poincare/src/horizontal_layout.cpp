#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/serialization_helper.h>

#include <algorithm>

namespace Poincare {

Layout HorizontalLayoutNode::deepChildToPointToWhenInserting() const {
  Layout layoutToPointTo =
      Layout(this).recursivelyMatches([](Poincare::Layout layout) {
        if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(
                layout.type())) {
          /* If the inserted bracket is temp on the left, do not put cursor
           * inside it so that the cursor is put right when inserting ")". */
          return static_cast<AutocompletedBracketPairLayoutNode *>(
                     layout.node())
                         ->isTemporary(
                             AutocompletedBracketPairLayoutNode::Side::Left)
                     ? TrinaryBoolean::False
                     : TrinaryBoolean::True;
        }
        return layout.isEmpty() ? TrinaryBoolean::True
                                : TrinaryBoolean::Unknown;
      });
  return layoutToPointTo;
}

int HorizontalLayoutNode::serialize(
    char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return serializeChildrenBetweenIndexes(buffer, bufferSize, floatDisplayMode,
                                         numberOfSignificantDigits, false);
}

int HorizontalLayoutNode::serializeChildrenBetweenIndexes(
    char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits, bool forceIndexes, int firstIndex,
    int lastIndex) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
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
  LayoutNode *currentChild = childAtIndex(index1);
  LayoutNode *nextChild = nullptr;
  for (int i = index1; i < index2; i++) {
    // Write the child
    assert(currentChild);
    numberOfChar += currentChild->serialize(
        buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
        numberOfSignificantDigits);
    if (numberOfChar >= bufferSize - 1) {
      assert(buffer[bufferSize - 1] == 0);
      return bufferSize - 1;
    }
    if (i != childrenCount - 1) {
      nextChild = childAtIndex(i + 1);
      // Write the multiplication sign if needed
      LayoutNode::Type nextChildType = nextChild->type();
      if ((nextChildType == LayoutNode::Type::AbsoluteValueLayout ||
           nextChildType == LayoutNode::Type::BinomialCoefficientLayout ||
           nextChildType == LayoutNode::Type::CeilingLayout ||
           nextChildType == LayoutNode::Type::ConjugateLayout ||
           nextChildType == LayoutNode::Type::CeilingLayout ||
           nextChildType == LayoutNode::Type::FloorLayout ||
           nextChildType == LayoutNode::Type::IntegralLayout ||
           nextChildType ==
               LayoutNode::Type::LetterAWithSubAndSuperscriptLayout ||
           nextChildType ==
               LayoutNode::Type::LetterCWithSubAndSuperscriptLayout ||
           (nextChildType == LayoutNode::Type::NthRootLayout &&
            !static_cast<NthRootLayoutNode *>(nextChild)->isSquareRoot()) ||
           nextChildType == LayoutNode::Type::ProductLayout ||
           nextChildType == LayoutNode::Type::SumLayout ||
           nextChildType == LayoutNode::Type::VectorNormLayout) &&
          currentChild->canBeOmittedMultiplicationLeftFactor()) {
        assert(nextChildType != LayoutNode::Type::HorizontalLayout);
        numberOfChar += SerializationHelper::CodePoint(
            buffer + numberOfChar, bufferSize - numberOfChar, '*');
        if (numberOfChar >= bufferSize - 1) {
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

// Private

KDPoint HorizontalLayoutNode::positionOfChild(LayoutNode *l,
                                              KDFont::Size font) {
  assert(hasChild(l));
  KDCoordinate x = 0;
  for (LayoutNode *c : children()) {
    if (c == l) {
      break;
    }
    KDSize childSize = c->layoutSize(font);
    x += childSize.width();
  }
  KDCoordinate y = baseline(font) - l->baseline(font);
  return KDPoint(x, y);
}

KDSize HorizontalLayoutNode::layoutSizeBetweenIndexes(int leftIndex,
                                                      int rightIndex,
                                                      KDFont::Size font) const {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= numberOfChildren());
  if (numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(font);
    KDCoordinate width =
        shouldDrawEmptyRectangle() ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    LayoutNode *childi = childAtIndex(i);
    KDSize childSize = childi->layoutSize(font);
    totalWidth += childSize.width();
    KDCoordinate childBaseline = childi->baseline(font);
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate HorizontalLayoutNode::baselineBetweenIndexes(
    int leftIndex, int rightIndex, KDFont::Size font) const {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= numberOfChildren());
  if (numberOfChildren() == 0) {
    return EmptyRectangle::RectangleBaseLine(font);
  }
  KDCoordinate result = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    result = std::max(result, childAtIndex(i)->baseline(font));
  }
  return result;
}

KDRect HorizontalLayoutNode::relativeSelectionRect(int leftIndex,
                                                   int rightIndex,
                                                   KDFont::Size font) const {
  assert(leftIndex >= 0 && rightIndex <= numberOfChildren() &&
         leftIndex < rightIndex);

  // Compute the positions
  KDCoordinate selectionXStart =
      const_cast<HorizontalLayoutNode *>(this)
          ->positionOfChild(childAtIndex(leftIndex), font)
          .x();
  KDCoordinate selectionYStart =
      const_cast<HorizontalLayoutNode *>(this)->baseline(font) -
      baselineBetweenIndexes(leftIndex, rightIndex, font);

  return KDRect(KDPoint(selectionXStart, selectionYStart),
                layoutSizeBetweenIndexes(leftIndex, rightIndex, font));
}

void HorizontalLayoutNode::render(KDContext *ctx, KDPoint p, KDFont::Size font,
                                  KDColor expressionColor,
                                  KDColor backgroundColor) {
  if (shouldDrawEmptyRectangle()) {
    // If the layout is empty, draw an empty rectangle
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font, m_emptyColor);
  }
}

bool HorizontalLayoutNode::shouldDrawEmptyRectangle() const {
  if (numberOfChildren() > 0) {
    return false;
  }
  LayoutNode *p = parent();
  if (!p || AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(
                p->type())) {
    // Never show the empty rectangle if in a parenthesis or if has no parent
    return false;
  }
  return m_emptyVisibility == EmptyRectangle::State::Visible;
}

// HorizontalLayout

void HorizontalLayout::addOrMergeChildAtIndex(Layout l, int index) {
  if (l.isHorizontal()) {
    mergeChildrenAtIndex(
        HorizontalLayout(static_cast<HorizontalLayoutNode *>(l.node())), index);
  } else {
    addChildAtIndexInPlace(l, index, numberOfChildren());
  }
}

void HorizontalLayout::mergeChildrenAtIndex(HorizontalLayout h, int index) {
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

  // Merge the horizontal layout
  int childrenNumber = h.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    int n = numberOfChildren();
    Layout c = h.childAtIndex(i);
    bool firstAddedChild = (i == 0);
    addChildAtIndexInPlace(c, newIndex, n);
    if (firstAddedChild) {
      LayoutNode *l = childAtIndex(newIndex).node();
      l->setMargin(margin);
      l->lockMargin(marginIsLocked);
    }
    newIndex++;
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

void HorizontalLayout::serializeChildren(int firstIndex, int lastIndex,
                                         char *buffer, int bufferSize) {
  static_cast<HorizontalLayoutNode *>(node())->serializeChildrenBetweenIndexes(
      buffer, bufferSize,
      Poincare::Preferences::sharedPreferences->displayMode(),
      Poincare::Preferences::sharedPreferences->numberOfSignificantDigits(),
      true, firstIndex, lastIndex);
}

}  // namespace Poincare
