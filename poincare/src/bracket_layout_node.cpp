#include <poincare/bracket_layout_node.h>
#include <escher/metric.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

void BracketLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->layoutReference() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go Left.
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentLayout = parent();
  if (parentLayout) {
    parentLayout->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void BracketLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->layoutReference() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go Right.
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  LayoutNode * parentLayout = parent();
  if (parentLayout) {
    parentLayout->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void BracketLayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_childHeightComputed = false;
  LayoutNode::invalidAllSizesPositionsAndBaselines();
}

void BracketLayoutNode::computeBaseline() {
  LayoutNode * parentLayout = parent();
  assert(parentLayout != nullptr);
  int idxInParent = parentLayout->indexOfChild(this);
  int numberOfSiblings = parentLayout->numberOfChildren();
  if (((isLeftParenthesis() || isLeftBracket()) && idxInParent == numberOfSiblings - 1)
      || ((isRightParenthesis() || isRightBracket()) && idxInParent == 0)
      || ((isLeftParenthesis() || isLeftBracket()) && idxInParent < numberOfSiblings - 1 && parentLayout->childAtIndex(idxInParent + 1)->isVerticalOffset()))
  {
    /* The bracket does not have siblings on its open direction, or it is a left
     * bracket that is base of a superscript layout. In the latter case, it
     * should have a default baseline, else it creates an infinite loop as the
     * bracket needs the superscript baseline, which needs the bracket baseline.*/
    m_baseline = layoutSize().height()/2;
    m_baselined = true;
    return;
  }

  int currentNumberOfOpenBrackets = 1;
  m_baseline = 0;
  int increment = (isLeftParenthesis() || isLeftBracket()) ? 1 : -1;
  for (int i = idxInParent + increment; i >= 0 && i < numberOfSiblings; i+=increment) {
    LayoutNode * sibling = parentLayout->childAtIndex(i);
    if ((isLeftParenthesis() && sibling->isRightParenthesis())
        || (isLeftBracket() && sibling->isRightBracket())
        || (isRightParenthesis() && sibling->isLeftParenthesis())
        || (isRightBracket() && sibling->isLeftBracket()))
    {
      if (i == idxInParent + increment) {
        /* If the bracket is immediately closed, we set the baseline to half the
         * bracket height. */
        m_baseline = layoutSize().height()/2;
        m_baselined = true;
        return;
      }
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if ((isLeftParenthesis() && sibling->isLeftParenthesis())
        || (isLeftBracket() && sibling->isLeftBracket())
        || (isRightParenthesis() && sibling->isRightParenthesis())
        || (isRightBracket() && sibling->isRightBracket()))
    {
      currentNumberOfOpenBrackets++;
    }
    m_baseline = max(m_baseline, sibling->baseline());
  }
  m_baseline += (layoutSize().height() - childHeight()) / 2;
  m_baselined = true;
}

KDCoordinate BracketLayoutNode::childHeight() {
  if (!m_childHeightComputed) {
    computeChildHeight();
  }
  return m_childHeight;
}

void BracketLayoutNode::computeChildHeight() {
  LayoutNode * parentLayout = parent();
  assert(parentLayout != nullptr);
  m_childHeight = Metric::MinimalBracketAndParenthesisHeight;
  int idxInParent = parentLayout->indexOfChild(this);
  int numberOfSiblings = parentLayout->numberOfChildren();
  if ((isLeftParenthesis() || isLeftBracket())
      && idxInParent < numberOfSiblings - 1
      && parentLayout->childAtIndex(idxInParent + 1)->isVerticalOffset())
  {
    /* If a left bracket is the base of a superscript layout, it should have a
     * a default height, else it creates an infinite loop because the bracket
     * needs the superscript height, which needs the bracket height. */
    m_childHeightComputed = true;
    return;
  }

  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;

  int currentNumberOfOpenBrackets = 1;
  int increment = (isLeftParenthesis() || isLeftBracket()) ? 1 : -1;
  for (int i = idxInParent + increment; i >= 0 && i < numberOfSiblings; i+= increment) {
    LayoutNode * sibling = parentLayout->childAtIndex(i);
    if ((isLeftParenthesis() && sibling->isRightParenthesis())
        || (isLeftBracket() && sibling->isRightBracket())
        || (isRightParenthesis() && sibling->isLeftParenthesis())
        || (isRightBracket() && sibling->isLeftBracket()))
    {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if ((isLeftParenthesis() && sibling->isLeftParenthesis())
        || (isLeftBracket() && sibling->isLeftBracket())
        || (isRightParenthesis() && sibling->isRightParenthesis())
        || (isRightBracket() && sibling->isRightBracket()))
    {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate siblingHeight = sibling->layoutSize().height();
    KDCoordinate siblingBaseline = sibling->baseline();
    maxUnderBaseline = max(maxUnderBaseline, siblingHeight - siblingBaseline);
    maxAboveBaseline = max(maxAboveBaseline, siblingBaseline);
  }
  m_childHeight = max(m_childHeight, maxUnderBaseline + maxAboveBaseline);
  m_childHeightComputed = true;

}

KDPoint BracketLayoutNode::positionOfChild(LayoutNode * child) {
  assert(false);
  return KDPointZero;
}

}
