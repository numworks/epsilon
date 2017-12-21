#include "parenthesis_layout.h"
#include <poincare/expression_layout_cursor.h>
#include "parenthesis_left_layout.h"
#include "parenthesis_right_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ParenthesisLayout::ParenthesisLayout(ExpressionLayout * operand, bool cloneOperands) :
  StaticLayoutHierarchy<3>()
{
  ExpressionLayout * leftParenthesis = new ParenthesisLeftLayout();
  ExpressionLayout * rightParenthesis = new ParenthesisRightLayout();
  build(ExpressionLayout::ExpressionLayoutArray3(leftParenthesis, operand, rightParenthesis), 3, cloneOperands);
}

ExpressionLayout * ParenthesisLayout::clone() const {
  ParenthesisLayout * layout = new ParenthesisLayout(const_cast<ParenthesisLayout *>(this)->operandLayout(), true);
  return layout;
}

bool ParenthesisLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the Right parenthesis.
  // Go to the operand and move left.
  if (rightParenthesisLayout()
    && cursor->pointedExpressionLayout() == rightParenthesisLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return operandLayout()->moveLeft(cursor);
  }
  // Case: Left of the operand.
  // Go Left.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left of the Left parenthesis.
  // Ask the parent.
  if (leftParenthesisLayout()
    && cursor->pointedExpressionLayout() == leftParenthesisLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->moveLeft(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right of the parentheses.
  // Go Right of the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left of the parentheses.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool ParenthesisLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the Left parenthesis.
  // Go to the operand and move Right.
  if (leftParenthesisLayout()
    && cursor->pointedExpressionLayout() == leftParenthesisLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return operandLayout()->moveRight(cursor);
  }
  // Case: Right of the operand.
  // Go Right.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Right of the Right parenthesis.
  // Ask the parent.
  if (rightParenthesisLayout()
    && cursor->pointedExpressionLayout() == rightParenthesisLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    if (m_parent) {
      return m_parent->moveRight(cursor);
    }
    return false;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left of the parentheses.
  // Go Left of the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right of the parentheses.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

KDSize ParenthesisLayout::computeSize() {
  KDSize operandSize = operandLayout()->size();
  return KDSize(operandSize.width() + 2*leftParenthesisLayout()->size().width(), operandSize.height());
}

void ParenthesisLayout::computeBaseline() {
  m_baseline = operandLayout()->baseline();
  m_baselined = true;
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  if (child == leftParenthesisLayout()) {
    return KDPoint(0, 0);
  }
  if (child == operandLayout()) {
    return KDPoint(leftParenthesisLayout()->size().width(), 0);
  }
  if (child == rightParenthesisLayout()) {
    return KDPoint(operandLayout()->origin().x() + operandLayout()->size().width(), 0);
  }
  assert(false);
  return KDPointZero;
}

ExpressionLayout * ParenthesisLayout::operandLayout() {
  return editableChild(1);
}

ExpressionLayout * ParenthesisLayout::leftParenthesisLayout() {
  return editableChild(0);
}

ExpressionLayout * ParenthesisLayout::rightParenthesisLayout() {
  return editableChild(2);
}

}
