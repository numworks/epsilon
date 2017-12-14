#include "parenthesis_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},

};

const uint8_t bottomLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},

};

const uint8_t topRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},

};

ParenthesisLayout::ParenthesisLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline();
}

ParenthesisLayout::~ParenthesisLayout() {
  delete m_operandLayout;
}

bool ParenthesisLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left of the operand.
  // Go Left of the brackets.
  if (m_operandLayout
    && cursor->pointedExpressionLayout() == m_operandLayout
    && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
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

KDColor s_parenthesisWorkingBuffer[ParenthesisLayout::k_parenthesisCurveHeight*ParenthesisLayout::k_parenthesisCurveWidth];

void ParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize operandSize = m_operandLayout->size();
  KDRect frame(p.x()+k_externWidthMargin, p.y()+k_externHeightMargin, k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x()+k_externWidthMargin, p.y() + operandSize.height() - k_parenthesisCurveHeight - k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x()+k_externWidthMargin + operandSize.width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y() + k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x() +k_externWidthMargin + operandSize.width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y() + operandSize.height() - k_parenthesisCurveHeight - k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)s_parenthesisWorkingBuffer);

  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+k_parenthesisCurveHeight+k_externHeightMargin, k_lineThickness, m_operandLayout->size().height() - 2*(k_parenthesisCurveHeight+k_externHeightMargin)), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+operandSize.width()+2*k_widthMargin+k_lineThickness, p.y()+k_parenthesisCurveHeight+2, k_lineThickness, m_operandLayout->size().height()- 2*(k_parenthesisCurveHeight+k_externHeightMargin)), expressionColor);
}

KDSize ParenthesisLayout::computeSize() {
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width() + 2*k_widthMargin + 2*k_lineThickness+2*k_externWidthMargin, operandSize.height());
}

ExpressionLayout * ParenthesisLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(k_widthMargin+k_lineThickness+k_externWidthMargin, 0);
}

}
