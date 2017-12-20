#include "bracket_left_right_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

BracketLeftRightLayout::BracketLeftRightLayout() :
  StaticLayoutHierarchy<0>(),
  m_operandHeight(18) //TODO
{
}

bool BracketLeftRightLayout::moveLeft(ExpressionLayoutCursor * cursor) {
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

bool BracketLeftRightLayout::moveRight(ExpressionLayoutCursor * cursor) {
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


KDSize BracketLeftRightLayout::computeSize() {
  //TODO: compute the operandHeight according to the brothers
  return KDSize(k_externWidthMargin + k_lineThickness + k_widthMargin, m_operandHeight);
}

void BracketLeftRightLayout::computeBaseline() {
  //TODO: compute the operandHeight according to the brothers
  m_baseline = m_operandHeight/2;
  m_baselined = true;
}

KDPoint BracketLeftRightLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
