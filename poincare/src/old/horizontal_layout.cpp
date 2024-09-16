#include <poincare/layout.h>
#include <poincare/old/serialization_helper.h>

#include <algorithm>

namespace Poincare {

OLayout HorizontalLayoutNode::deepChildToPointToWhenInserting() const {
  OLayout layoutToPointTo =
      OLayout(this).recursivelyMatches([](Poincare::OLayout layout) {
        if (AutocompletedBracketPairLayoutNode::IsAutoCompletedBracketPairType(
                layout.otype())) {
          /* If the inserted bracket is temp on the left, do not put cursor
           * inside it so that the cursor is put right when inserting ")". */
          return static_cast<AutocompletedBracketPairLayoutNode *>(
                     layout.node())
                         ->isTemporary(
                             AutocompletedBracketPairLayoutNode::Side::Left)
                     ? OMG::Troolean::False
                     : OMG::Troolean::True;
        }
        return layout.isEmpty() ? OMG::Troolean::True : OMG::Troolean::Unknown;
      });
  return layoutToPointTo;
}

size_t HorizontalLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  int childrenCount = numberOfChildren();
  if (childrenCount == 0 || bufferSize == 1) {
    buffer[0] = 0;
    return 0;
  }

  size_t numberOfChar = 0;
  // Write the children, adding multiplication signs if needed
  LayoutNode *currentChild = childAtIndex(0);
  LayoutNode *nextChild = nullptr;
  for (int i = 0; i < childrenCount; i++) {
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
      LayoutNode::Type nextChildType = nextChild->otype();
      if ((nextChildType == LayoutNode::Type::AbsoluteValueLayout ||
           nextChildType == LayoutNode::Type::BinomialCoefficientLayout ||
           nextChildType == LayoutNode::Type::CeilingLayout ||
           nextChildType == LayoutNode::Type::ConjugateLayout ||
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

// HorizontalLayout

void HorizontalLayout::addOrMergeChildAtIndex(OLayout l, int index) {
  if (l.isHorizontal()) {
    mergeChildrenAtIndex(
        HorizontalLayout(static_cast<HorizontalLayoutNode *>(l.node())), index);
  } else {
    addChildAtIndexInPlace(l, index, numberOfChildren());
  }
}

void HorizontalLayout::mergeChildrenAtIndex(HorizontalLayout h, int index) {
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

  // Merge the horizontal layout
  int childrenNumber = h.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    int n = numberOfChildren();
    OLayout c = h.childAtIndex(i);
    bool firstAddedChild = (i == 0);
    addChildAtIndexInPlace(c, newIndex, n);
    if (firstAddedChild) {
      LayoutNode *l = childAtIndex(newIndex).node();
    }
    newIndex++;
  }
}

OLayout HorizontalLayout::squashUnaryHierarchyInPlace() {
  if (numberOfChildren() == 1) {
    OLayout child = childAtIndex(0);
    replaceWithInPlace(child);
    return child;
  }
  return *this;
}

}  // namespace Poincare
