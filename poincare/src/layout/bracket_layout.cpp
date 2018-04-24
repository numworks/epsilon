#include "bracket_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return (x>y ? x : y); }

BracketLayout::BracketLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeightComputed(false)
{
}

void BracketLayout::invalidAllSizesPositionsAndBaselines() {
  m_operandHeightComputed = false;
  ExpressionLayout::invalidAllSizesPositionsAndBaselines();
}

ExpressionLayoutCursor BracketLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right. Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor BracketLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left. Go Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

void BracketLayout::computeBaseline() {
  assert(m_parent != nullptr);
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfSiblings = m_parent->numberOfChildren();
  if (((isLeftParenthesis() || isLeftBracket()) && indexInParent == numberOfSiblings - 1)
      || ((isRightParenthesis() || isRightBracket()) && indexInParent == 0)
      || ((isLeftParenthesis() || isLeftBracket()) && indexInParent < numberOfSiblings - 1 && m_parent->child(indexInParent + 1)->isVerticalOffset()))
  {
    /* The bracket does not have siblings on its open direction, or it is a left
     * bracket that is base of a superscript layout. In the latter case, it
     * should have a default baseline, else it creates an infinite loop as the
     * bracket needs the superscript baseline, which needs the bracket baseline.*/
    m_baseline = size().height()/2;
    m_baselined = true;
    return;
  }

  int currentNumberOfOpenBrackets = 1;
  m_baseline = 0;
  int increment = (isLeftParenthesis() || isLeftBracket()) ? 1 : -1;
  for (int i = indexInParent + increment; i >= 0 && i < numberOfSiblings; i+=increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if ((isLeftParenthesis() && sibling->isRightParenthesis())
        || (isLeftBracket() && sibling->isRightBracket())
        || (isRightParenthesis() && sibling->isLeftParenthesis())
        || (isRightBracket() && sibling->isLeftBracket()))
    {
      if (i == indexInParent + increment) {
        /* If the bracket is immediately closed, we set the baseline to half the
         * bracket height. */
        m_baseline = size().height()/2;
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
  m_baseline += (size().height() - operandHeight()) / 2;
  m_baselined = true;
}

KDCoordinate BracketLayout::operandHeight() {
  if (!m_operandHeightComputed) {
    computeOperandHeight();
  }
  return m_operandHeight;
}

void BracketLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfSiblings = m_parent->numberOfChildren();
  if ((isLeftParenthesis() || isLeftBracket())
      && indexInParent < numberOfSiblings - 1
      && m_parent->child(indexInParent + 1)->isVerticalOffset())
  {
    /* If a left bracket is the base of a superscript layout, it should have a
     * a default height, else it creates an infinite loop because the bracket
     * needs the superscript height, which needs the bracket height. */
    m_operandHeightComputed = true;
    return;
  }

  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;

  int currentNumberOfOpenBrackets = 1;
  int increment = (isLeftParenthesis() || isLeftBracket()) ? 1 : -1;
  for (int i = indexInParent + increment; i >= 0 && i < numberOfSiblings; i+= increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
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
    KDCoordinate siblingHeight = sibling->size().height();
    KDCoordinate siblingBaseline = sibling->baseline();
    maxUnderBaseline = max(maxUnderBaseline, siblingHeight - siblingBaseline);
    maxAboveBaseline = max(maxAboveBaseline, siblingBaseline);
  }
  m_operandHeight = max(m_operandHeight, maxUnderBaseline + maxAboveBaseline);
  m_operandHeightComputed = true;

}

KDPoint BracketLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
