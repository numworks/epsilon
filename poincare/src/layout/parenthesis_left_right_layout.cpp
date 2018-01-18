#include "parenthesis_left_right_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ParenthesisLeftRightLayout::ParenthesisLeftRightLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeightComputed(false)
{
}

void ParenthesisLeftRightLayout::invalidAllSizesPositionsAndBaselines() {
  m_operandHeightComputed = false;
  ExpressionLayout::invalidAllSizesPositionsAndBaselines();
}

bool ParenthesisLeftRightLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

bool ParenthesisLeftRightLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
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

KDSize ParenthesisLeftRightLayout::computeSize() {
  return KDSize(parenthesisWidth(), operandHeight());
}

KDCoordinate ParenthesisLeftRightLayout::operandHeight() {
  if (!m_operandHeightComputed) {
    computeOperandHeight();
    m_operandHeightComputed = true;
  }
  return m_operandHeight;
}

KDPoint ParenthesisLeftRightLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
