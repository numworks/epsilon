#include "bracket_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

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

KDSize BracketLayout::computeSize() {
  return KDSize(k_externWidthMargin + k_lineThickness + k_widthMargin, operandHeight() + k_lineThickness);
}

KDCoordinate BracketLayout::operandHeight() {
  if (!m_operandHeightComputed) {
    computeOperandHeight();
    m_operandHeightComputed = true;
  }
  return m_operandHeight;
}

KDPoint BracketLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
