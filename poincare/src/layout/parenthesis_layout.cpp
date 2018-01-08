#include "parenthesis_layout.h"
#include "horizontal_layout.h"
#include "parenthesis_left_layout.h"
#include "parenthesis_left_right_layout.h"
#include "parenthesis_right_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <poincare/expression_layout_array.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * ParenthesisLayout::clone() const {
  return new ParenthesisLayout(const_cast<ParenthesisLayout *>(this)->operandLayout(), true);
}

bool ParenthesisLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the operand.
  // Go Left.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go to the operand and move left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return operandLayout()->moveLeft(cursor);
  }

  // Case: Left.
  // Ask the parent.
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool ParenthesisLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right of the operand.
  // Go Right.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }

  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left.
  // Go Left of the operand and moveRight.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return operandLayout()->moveRight(cursor);;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

void ParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Render the parentheses.
  ParenthesisLeftLayout * dummyLeftParenthesis = new ParenthesisLeftLayout();
  ParenthesisRightLayout * dummyRightParenthesis = new ParenthesisRightLayout();
  HorizontalLayout dummyLayout(dummyLeftParenthesis, operandLayout()->clone(), dummyRightParenthesis, false);
  KDPoint leftParenthesisPoint = positionOfChild(operandLayout()).translatedBy(dummyLayout.positionOfChild(dummyLeftParenthesis)).translatedBy(dummyLayout.positionOfChild(dummyLayout.editableChild(1)).opposite());
  KDPoint rightParenthesisPoint = positionOfChild(operandLayout()).translatedBy(dummyLayout.positionOfChild(dummyRightParenthesis)).translatedBy(dummyLayout.positionOfChild(dummyLayout.editableChild(1)).opposite());
  dummyLeftParenthesis->render(ctx, p.translatedBy(leftParenthesisPoint), expressionColor, backgroundColor);
  dummyRightParenthesis->render(ctx, p.translatedBy(rightParenthesisPoint), expressionColor, backgroundColor);
}

KDSize ParenthesisLayout::computeSize() {
  KDSize operandSize = operandLayout()->size();
  return KDSize(operandSize.width() + 2*ParenthesisLeftRightLayout::parenthesisWidth(), operandSize.height());
}

void ParenthesisLayout::computeBaseline() {
  m_baseline = operandLayout()->baseline();
  m_baselined = true;
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  if (child == operandLayout()) {
    return KDPoint(ParenthesisLeftRightLayout::parenthesisWidth(), 0);
  }
  assert(false);
  return KDPointZero;
}

ExpressionLayout * ParenthesisLayout::operandLayout() {
  return editableChild(0);
}

}
