#include "parenthesis_left_right_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ParenthesisLeftRightLayout::ParenthesisLeftRightLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeight(36) //TODO
{
  computeBaseline();
}

bool ParenthesisLeftRightLayout::moveLeft(ExpressionLayoutCursor * cursor) {
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
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool ParenthesisLeftRightLayout::moveRight(ExpressionLayoutCursor * cursor) {
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
    return m_parent->moveRight(cursor);
  }
  return false;
}

KDSize ParenthesisLeftRightLayout::computeSize() {
  //TODO: compute the operandHeight according to the brothers
  return KDSize(k_widthMargin + k_lineThickness + k_externWidthMargin, m_operandHeight);
}

void ParenthesisLeftRightLayout::computeBaseline() {
  m_baseline = m_operandHeight/2; //TODO
  m_baselined = true;
}

KDPoint ParenthesisLeftRightLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
