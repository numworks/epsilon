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

bool ParenthesisLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool ParenthesisLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go Right.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

KDSize ParenthesisLayout::computeSize() {
  return KDSize(parenthesisWidth(), operandHeight() + k_verticalMargin);
}

void ParenthesisLayout::computeBaseline() {
  assert(m_parent != nullptr);
  bool isParenthesisLeft = isLeftParenthesis();
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfBrothers = m_parent->numberOfChildren();
  if ((isParenthesisLeft && indexInParent == numberOfBrothers - 1)
      || (!isParenthesisLeft && indexInParent == 0)
      || (isParenthesisLeft && indexInParent < numberOfBrothers - 1 && m_parent->child(indexInParent + 1)->isVerticalOffset()))
  {
    /* The parenthesis does not have brothers on its open direction, or it is a
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
  for (int i = indexInParent + increment; i >= 0 && i <= numberOfBrothers - 1; i+= increment) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if ((isParenthesisLeft && brother->isRightParenthesis())
        || (!isParenthesisLeft && brother->isLeftParenthesis()))
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
    } else if ((isParenthesisLeft && brother->isLeftParenthesis())
        || (!isParenthesisLeft && brother->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses++;
    }
    if (brother->baseline() > m_baseline) {
      m_baseline = brother->baseline();
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
  int numberOfBrothers = m_parent->numberOfChildren();
  if (isParenthesisLeft
      && indexInParent < numberOfBrothers - 1
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
  for (int i = indexInParent + increment; i >= 0 && i < numberOfBrothers; i+= increment) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if ((!isParenthesisLeft && brother->isLeftParenthesis())
        || (isParenthesisLeft && brother->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        if (max_under_baseline + max_above_baseline > m_operandHeight) {
          m_operandHeight = max_under_baseline + max_above_baseline;
        }
        return;
      }
    } else if ((isParenthesisLeft && brother->isLeftParenthesis())
        || (!isParenthesisLeft && brother->isRightParenthesis()))
    {
      currentNumberOfOpenParentheses++;
    }
    KDCoordinate brotherHeight = brother->size().height();
    KDCoordinate brotherBaseline = brother->baseline();
    if (brotherHeight - brotherBaseline > max_under_baseline) {
      max_under_baseline = brotherHeight - brotherBaseline ;
    }
    if (brotherBaseline > max_above_baseline) {
      max_above_baseline = brotherBaseline;
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
