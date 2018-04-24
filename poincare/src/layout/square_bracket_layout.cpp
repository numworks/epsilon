#include "square_bracket_layout.h"
#include <escher/metric.h>
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

SquareBracketLayout::SquareBracketLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeightComputed(false)
{
}

void SquareBracketLayout::invalidAllSizesPositionsAndBaselines() {
  m_operandHeightComputed = false;
  ExpressionLayout::invalidAllSizesPositionsAndBaselines();
}

ExpressionLayoutCursor SquareBracketLayout::cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

ExpressionLayoutCursor SquareBracketLayout::cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

KDSize SquareBracketLayout::computeSize() {
  return KDSize(k_externWidthMargin + k_lineThickness + k_widthMargin, operandHeight() + k_lineThickness);
}

void SquareBracketLayout::computeBaseline() {
  assert(m_parent != nullptr);
  m_baseline = operandHeight()/2;
  int currentNumberOfOpenBrackets = 1;
  int increment = isLeftBracket() ? 1 : -1;
  int numberOfSiblings = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + increment; i >= 0 && i < numberOfSiblings; i+=increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if ((isRightBracket() && sibling->isLeftBracket())
        || (isLeftBracket() && sibling->isRightBracket()))
    {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if ((isRightBracket() && sibling->isRightBracket())
        || (isLeftBracket() && sibling->isLeftBracket()))
    {
      currentNumberOfOpenBrackets++;
    }
    if (sibling->baseline() > m_baseline) {
      m_baseline = sibling->baseline();
    }
  }
  m_baselined = true;
}

KDCoordinate SquareBracketLayout::operandHeight() {
  if (!m_operandHeightComputed) {
    computeOperandHeight();
  }
  return m_operandHeight;
}

void SquareBracketLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  int currentNumberOfOpenBrackets = 1;
  int increment = isLeftBracket() ? 1 : -1;
  int numberOfSiblings = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + increment; i >= 0 && i < numberOfSiblings; i+=increment) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if ((isRightBracket() && sibling->isLeftBracket())
       || (isLeftBracket() && sibling->isRightBracket()))
    {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if ((isRightBracket() && sibling->isRightBracket())
       || (isLeftBracket() && sibling->isLeftBracket()))
    {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate siblingHeight = sibling->size().height();
    if (siblingHeight > m_operandHeight) {
      m_operandHeight = siblingHeight;
    }
  }
  m_operandHeightComputed = true;
}

KDPoint SquareBracketLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
