#include "parenthesis_layout.h"
#include <poincare/expression_layout_cursor.h>
#include "parenthesis_left_layout.h"
#include "parenthesis_right_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ParenthesisLayout::ParenthesisLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_leftParenthesisLayout = new ParenthesisLeftLayout();
  m_rightParenthesisLayout = new ParenthesisRightLayout();
  m_operandLayout->setParent(this);
  m_leftParenthesisLayout->setParent(this);
  m_rightParenthesisLayout->setParent(this);
  m_baseline = m_operandLayout->baseline();
}

ParenthesisLayout::~ParenthesisLayout() {
  delete m_operandLayout;
  delete m_rightParenthesisLayout;
  delete m_leftParenthesisLayout;
}

bool ParenthesisLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the Right parenthesis.
  // Go to the operand and move left.
  if (m_rightParenthesisLayout
    && cursor->pointedExpressionLayout() == m_rightParenthesisLayout
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(m_operandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_operandLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return m_operandLayout->moveLeft(cursor);
  }
  // Case: Left of the operand.
  // Go Left.
  if (m_operandLayout
    && cursor->pointedExpressionLayout() == m_operandLayout
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left of the Left parenthesis.
  // Ask the parent.
  if (m_leftParenthesisLayout
    && cursor->pointedExpressionLayout() == m_leftParenthesisLayout
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
    assert(m_operandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_operandLayout);
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
  if (m_leftParenthesisLayout
    && cursor->pointedExpressionLayout() == m_leftParenthesisLayout
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(m_operandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_operandLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return m_operandLayout->moveRight(cursor);
  }
  // Case: Right of the operand.
  // Go Right.
  if (m_operandLayout
    && cursor->pointedExpressionLayout() == m_operandLayout
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Right of the Right parenthesis.
  // Ask the parent.
  if (m_rightParenthesisLayout
    && cursor->pointedExpressionLayout() == m_rightParenthesisLayout
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
    assert(m_operandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_operandLayout);
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
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width() + 2*m_leftParenthesisLayout->size().width(), operandSize.height());
}

ExpressionLayout * ParenthesisLayout::child(uint16_t index) {
  if (index == 0) {
    return m_leftParenthesisLayout;
  }
  if (index == 1) {
    return m_operandLayout;
  }
  if (index == 2) {
    return m_rightParenthesisLayout;
  }
  return nullptr;
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  if (child == m_leftParenthesisLayout) {
    return KDPoint(0, 0);
  }
  if (child == m_operandLayout) {
    return KDPoint(m_leftParenthesisLayout->size().width(), 0);
  }
  if (child == m_rightParenthesisLayout) {
    return KDPoint(m_operandLayout->origin().x() + m_operandLayout->size().width(), 0);
  }
  assert(false);
  return KDPointZero;
}

}
