#include "parenthesis_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

static inline int max(int x, int y) { return (x>y ? x : y); }


ParenthesisLayout::ParenthesisLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeightComputed(false)
{
}

void ParenthesisLayout::invalidAllSizesPositionsAndBaselines() {
  m_operandHeightComputed = false;
  ExpressionLayout::invalidAllSizesPositionsAndBaselines();
}

ExpressionLayoutCursor ParenthesisLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

ExpressionLayoutCursor ParenthesisLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

KDSize ParenthesisLayout::computeSize() {
  return KDSize(parenthesisWidth(), operandHeight() + k_verticalMargin);
}

void ParenthesisLayout::computeBaseline() {
  assert(m_parent != nullptr);
  bool isParenthesisLeft = isLeftParenthesis();
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfSiblings = m_parent->numberOfChildren();
  if ((isParenthesisLeft && indexInParent == numberOfSiblings - 1)
      || (!isParenthesisLeft && indexInParent == 0)
      || (isParenthesisLeft && indexInParent < numberOfSiblings - 1 && m_parent->child(indexInParent + 1)->isVerticalOffset()))
  {
    /* The parenthesis does not have siblings on its open direction, or it is a
     * left parenthesis that is base of a superscript layout. In the latter
     * case, it should have a default baseline, else it creates an infinite loop
     * because the parenthesis needs the superscript height, which needs the
     * parenthesis baseline. */
    m_baseline = size().height()/2;
    m_baselined = true;
    return;
  }

  int currentNumberOfOpenParentheses = 1;
  m_baseline = 0;
  int increment = isParenthesisLeft ? 1 : -1;
  for (int i = indexInParent + increment; i >= 0 && i <= numberOfSiblings - 1; i+= increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if ((isParenthesisLeft && sibling->isRightParenthesis())
        || (!isParenthesisLeft && sibling->isLeftParenthesis()))
    {
      if (i == indexInParent + increment) {
        /* If the parenthesis is immediately closed, we set the baseline to half
         * the parenthesis height. */
        m_baseline = size().height()/2;
        m_baselined = true;
        return;
      }
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        break;
      }
    } else if ((isParenthesisLeft && sibling->isLeftParenthesis())
        || (!isParenthesisLeft && sibling->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses++;
    }
    if (sibling->baseline() > m_baseline) {
      m_baseline = sibling->baseline();
    }
  }
  m_baseline += (size().height() - operandHeight()) / 2;
  m_baselined = true;
}

KDCoordinate ParenthesisLayout::operandHeight() {
  if (!m_operandHeightComputed) {
    computeOperandHeight();
    m_operandHeightComputed = true;
  }
  return m_operandHeight;
}

void ParenthesisLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  bool isParenthesisLeft = isLeftParenthesis();
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfSiblings = m_parent->numberOfChildren();
  if (isParenthesisLeft
      && indexInParent < numberOfSiblings - 1
      && m_parent->child(indexInParent + 1)->isVerticalOffset())
  {
    /* If a left parenthesis is the base of a superscript layout, it should have
     * a default height, else it creates an infinite loop because the
     * parenthesis needs the superscript height, which needs the parenthesis
     * height. */
    return;
  }

  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;

  int currentNumberOfOpenParentheses = 1;
  int increment = isParenthesisLeft ? 1 : -1;
  for (int i = indexInParent + increment; i >= 0 && i < numberOfSiblings; i+= increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if ((!isParenthesisLeft && sibling->isLeftParenthesis())
        || (isParenthesisLeft && sibling->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        if (max_under_baseline + max_above_baseline > m_operandHeight) {
          m_operandHeight = max_under_baseline + max_above_baseline;
        }
        return;
      }
    } else if ((isParenthesisLeft && sibling->isLeftParenthesis())
        || (!isParenthesisLeft && sibling->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses++;
    }
    KDCoordinate siblingHeight = sibling->size().height();
    KDCoordinate siblingBaseline = sibling->baseline();
    if (siblingHeight - siblingBaseline > max_under_baseline) {
      max_under_baseline = siblingHeight - siblingBaseline ;
    }
    if (siblingBaseline > max_above_baseline) {
      max_above_baseline = siblingBaseline;
    }
  }
  if (max_under_baseline + max_above_baseline > m_operandHeight) {
    m_operandHeight = max_under_baseline + max_above_baseline;
  }
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
