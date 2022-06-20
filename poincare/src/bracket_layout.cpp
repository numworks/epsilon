#include <poincare/bracket_layout.h>
#include <escher/metric.h>
#include <assert.h>
#include <stdlib.h>
#include <algorithm>

namespace Poincare {

void BracketLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go Left.
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentLayout = parent();
  if (parentLayout != nullptr) {
    parentLayout->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void BracketLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go Right.
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  LayoutNode * parentLayout = parent();
  if (parentLayout != nullptr) {
    parentLayout->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BracketLayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_childHeightComputed = false;
  LayoutNode::invalidAllSizesPositionsAndBaselines();
}

bool BracketLayoutNode::isCollapsable(int * numberOfOpenBrackets, bool goingLeft) const {
  if (goingLeft == IsRightBracket(type())) {
    /* This brace is an opening brace. */
    *numberOfOpenBrackets = *numberOfOpenBrackets + 1;
    return true;
  }

  /* This brace is a closing brace. We do not want to absorb it if
   * there is no corresponding opening brace, as the absorber should be
   * enclosed by this brace. */
  assert((goingLeft && IsLeftBracket(type())) || (!goingLeft && IsRightBracket(type())));
  if (*numberOfOpenBrackets == 0) {
    return false;
  }
  *numberOfOpenBrackets = *numberOfOpenBrackets - 1;
  return true;
}

KDCoordinate BracketLayoutNode::computeBaseline() {
  LayoutNode * parentLayout = parent();
  assert(parentLayout != nullptr);
  int idxInParent = parentLayout->indexOfChild(this);
  int numberOfSiblings = parentLayout->numberOfChildren();
  if ((IsLeftBracket(type()) && (idxInParent == numberOfSiblings - 1 || (idxInParent < numberOfSiblings - 1 && parentLayout->childAtIndex(idxInParent + 1)->type() == Type::VerticalOffsetLayout)))
   || (IsRightBracket(type()) && idxInParent == 0))
  {
    /* The bracket does not have siblings on its open direction, or it is a left
     * bracket that is base of a superscript layout. In the latter case, it
     * should have a default baseline, else it creates an infinite loop as the
     * bracket needs the superscript baseline, which needs the bracket baseline.*/
    return layoutSize().height()/2;
  }

  int currentNumberOfOpenBrackets = 1;
  KDCoordinate result = 0;
  if (parentLayout->type() != LayoutNode::Type::HorizontalLayout) {
    /* The bracket has no true sibling. Those that might be founded by
     * numberOfSiblings = parentLayout->numberOfChildren() are likely children
     * of Narylayout where the bracket is alone in one of the sublayouts. To
     * prevent it from defining its height and baseline according to the other
     * sublayouts, we use this escape case.*/
    return result;
  }
  int increment = IsLeftBracket(type()) ? 1 : -1;
  for (int i = idxInParent + increment; i >= 0 && i < numberOfSiblings; i+=increment) {
    LayoutNode * sibling = parentLayout->childAtIndex(i);
    /* Nested parentheses and curly braces are treated as the same layout for
     * computing baseline and size. */
    if ((IsLeftBracket(type()) && IsLeftBracket(sibling->type()))
        || (IsRightBracket(type()) && IsRightBracket(sibling->type())))
    {
      currentNumberOfOpenBrackets++;
    } else if ((IsLeftBracket(type()) && IsRightBracket(sibling->type()))
        || (IsRightBracket(type()) && IsLeftBracket(sibling->type())))
    {
      if (i == idxInParent + increment) {
        /* If the bracket is immediately closed, we set the baseline to half the
          * bracket height. */
        return layoutSize().height()/2;
      }
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    }
    result = std::max(result, sibling->baseline());
  }
  return result + (layoutSize().height() - childHeight()) / 2;
}

KDCoordinate BracketLayoutNode::childHeight() {
  if (!m_childHeightComputed) {
    m_childHeight = computeChildHeight();
    m_childHeightComputed = true;
  }
  return m_childHeight;
}

KDCoordinate BracketLayoutNode::computeChildHeight() {
  LayoutNode * parentLayout = parent();
  assert(parentLayout != nullptr);
  KDCoordinate result = Escher::Metric::MinimalBracketAndParenthesisHeight;
  int idxInParent = parentLayout->indexOfChild(this);
  if (parentLayout->type() != LayoutNode::Type::HorizontalLayout) {
    /* The bracket has no true sibling. Those that might be founded by
     * numberOfSiblings = parentLayout->numberOfChildren() are likely children
     * of Narylayout where the bracket is alone in one of the sublayouts. To
     * prevent it from defining its height and baseline according to the other
     * sublayouts, we use this escape case.*/
    return result;
  }
  int numberOfSiblings = parentLayout->numberOfChildren();
  if (IsLeftBracket(type())
   && idxInParent < numberOfSiblings - 1
   && parentLayout->childAtIndex(idxInParent + 1)->type() == Type::VerticalOffsetLayout)
  {
    /* If a left bracket is the base of a superscript layout, it should have a
     * a default height, else it creates an infinite loop because the bracket
     * needs the superscript height, which needs the bracket height. */
    return result;
  }
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;

  int currentNumberOfOpenBrackets = 1;
  int increment = IsLeftBracket(type()) ? 1 : -1;
  for (int i = idxInParent + increment; i >= 0 && i < numberOfSiblings; i+= increment) {
    LayoutNode * sibling = parentLayout->childAtIndex(i);
    /* Nested parentheses and curly braces are treated as the same layout for
     * computing baseline and size. */
    if ((IsLeftBracket(type()) && IsLeftBracket(sibling->type()))
        || (IsRightBracket(type()) && IsRightBracket(sibling->type())))
    {
      currentNumberOfOpenBrackets++;
    } else if ((IsLeftBracket(type()) && IsRightBracket(sibling->type()))
        || (IsRightBracket(type()) && IsLeftBracket(sibling->type())))
    {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    }
    KDCoordinate siblingHeight = sibling->layoutSize().height();
    KDCoordinate siblingBaseline = sibling->baseline();
    maxUnderBaseline = std::max<KDCoordinate>(maxUnderBaseline, siblingHeight - siblingBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, siblingBaseline);
  }
  return std::max<KDCoordinate>(result, maxUnderBaseline + maxAboveBaseline);
}

KDPoint BracketLayoutNode::positionOfChild(LayoutNode * child) {
  assert(false);
  return KDPointZero;
}

bool BracketLayoutNode::IsLeftBracket(Type t) {
  return t == Type::LeftCurlyBraceLayout || t == Type::LeftParenthesisLayout;
}

bool BracketLayoutNode::IsRightBracket(Type t) {
  return t == Type::RightCurlyBraceLayout || t == Type::RightParenthesisLayout;
}

}
