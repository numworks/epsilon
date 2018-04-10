#include "bracket_layout.h"
#include <poincare/expression_layout_cursor.h>
#include <poincare/layout_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ExpressionLayout * BracketLayout::clone() const {
  BracketLayout * layout = new BracketLayout(const_cast<BracketLayout *>(this)->operandLayout(), true);
  return layout;
}

void BracketLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->pointedExpressionLayout() == this
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    // Case: Right.
    // Delete the layout, keep the operand.
    replaceWithAndMoveCursor(operandLayout(), true, cursor);
    return;
  }
  ExpressionLayout::backspaceAtCursor(cursor);
}

bool BracketLayout::moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Left of the operand.
  // Go Left of the brackets.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right of the brackets.
  // Go Right of the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left of the brackets.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor, shouldRecomputeLayout);
  }
  return false;
}

bool BracketLayout::moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) {
  // Case: Right of the operand.
  // Go Right of the brackets.
  if (operandLayout()
    && cursor->pointedExpressionLayout() == operandLayout()
    && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left of the brackets.
  // Go Left of the operand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(operandLayout() != nullptr);
    cursor->setPointedExpressionLayout(operandLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right of the brackets.
  // Ask the parent.
  cursor->setPointedExpressionLayout(this);
  if (m_parent) {
    return m_parent->moveRight(cursor, shouldRecomputeLayout);
  }
  return false;
}

int BracketLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the opening bracket
  int numberOfChar = 0;
  buffer[numberOfChar++] = '[';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1;}

  // Write the argument
  numberOfChar += const_cast<Poincare::BracketLayout *>(this)->operandLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing bracket
  buffer[numberOfChar++] = ']';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

ExpressionLayout * BracketLayout::operandLayout()  {
  return editableChild(0);
}

void BracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  const KDCoordinate k_widthMargin = widthMargin();
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  KDSize operandSize = operandLayout()->size();
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, operandLayout()->size().height()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+operandSize.width()+2*k_widthMargin+k_lineThickness, p.y(), k_lineThickness, operandLayout()->size().height()), expressionColor);
  if (renderTopBar()) {
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+operandSize.width()+2*k_widthMargin-k_bracketWidth, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar()) {
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+operandSize.height()-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_externWidthMargin+2*k_lineThickness+operandSize.width()+2*k_widthMargin-k_bracketWidth, p.y()+operandSize.height()-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }
}

KDSize BracketLayout::computeSize() {
  const KDCoordinate k_widthMargin = widthMargin();
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  KDSize operandSize = operandLayout()->size();
  return KDSize(operandSize.width() + 2*k_externWidthMargin + 2*k_widthMargin + 2*k_lineThickness, operandSize.height());
}

void BracketLayout::computeBaseline() {
  m_baseline = operandLayout()->baseline();
  m_baselined = true;
}

KDPoint BracketLayout::positionOfChild(ExpressionLayout * child) {
  const KDCoordinate k_widthMargin = widthMargin();
  const KDCoordinate k_externWidthMargin = externWidthMargin();
  return KDPoint(k_widthMargin+k_externWidthMargin+k_lineThickness, 0);
}

}
