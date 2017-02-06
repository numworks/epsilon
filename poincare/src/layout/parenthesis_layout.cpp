#include "parenthesis_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},

};

const uint8_t bottomLeftCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},

};

const uint8_t topRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},

};

const uint8_t bottomRightCurve[ParenthesisLayout::k_parenthesisCurveHeight][ParenthesisLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},

};

ParenthesisLayout::ParenthesisLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline() + k_heightMargin;
}

ParenthesisLayout::~ParenthesisLayout() {
  delete m_operandLayout;
}

void ParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize operandSize = m_operandLayout->size();
  KDColor workingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
  KDRect frame(p.x(), p.y(), k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)workingBuffer);
  frame = KDRect(p.x(), p.y() + operandSize.height() + 2*k_heightMargin - k_parenthesisCurveHeight, 
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)workingBuffer);
  frame = KDRect(p.x() + operandSize.width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y(), 
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)workingBuffer);
  frame = KDRect(p.x() + operandSize.width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y() + operandSize.height() + 2*k_heightMargin - k_parenthesisCurveHeight, 
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)workingBuffer);

  ctx->fillRect(KDRect(p.x(), p.y()+k_parenthesisCurveHeight, k_lineThickness, m_operandLayout->size().height()+2*k_heightMargin - 2*k_parenthesisCurveHeight), expressionColor);
  ctx->fillRect(KDRect(p.x()+operandSize.width()+2*k_widthMargin+k_lineThickness, p.y()+k_parenthesisCurveHeight, k_lineThickness, m_operandLayout->size().height()+2*k_heightMargin - 2*k_parenthesisCurveHeight), expressionColor);
}

KDSize ParenthesisLayout::computeSize() {
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width() + 2*k_widthMargin + 2*k_lineThickness, operandSize.height()+2*k_heightMargin);
}

ExpressionLayout * ParenthesisLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint ParenthesisLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(k_widthMargin+k_lineThickness, k_heightMargin);
}

}
