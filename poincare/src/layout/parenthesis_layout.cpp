#include "parenthesis_layout.h"
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
  if (m_operandLayout) {
    m_operandLayout->setParent(this);
    m_baseline = m_operandLayout->baseline();
  } else {
    m_baseline = (KDText::charSize(KDText::FontSize::Large).height()+1)/2;
  }
}

ParenthesisLayout::~ParenthesisLayout() {
  if (m_operandLayout) {
    delete m_operandLayout;
  }
}

KDColor s_parenthesisWorkingBuffer[ParenthesisLayout::k_parenthesisCurveHeight*ParenthesisLayout::k_parenthesisCurveWidth];

void ParenthesisLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame(p.x()+k_externWidthMargin, p.y()+k_externHeightMargin, k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x()+k_externWidthMargin, p.y() + operandSize().height() - k_parenthesisCurveHeight - k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x()+k_externWidthMargin + operandSize().width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y() + k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)s_parenthesisWorkingBuffer);
  frame = KDRect(p.x() +k_externWidthMargin + operandSize().width() + 2*k_widthMargin + 2*k_lineThickness - k_parenthesisCurveWidth, p.y() + operandSize().height() - k_parenthesisCurveHeight - k_externHeightMargin,
    k_parenthesisCurveWidth, k_parenthesisCurveHeight);
  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)s_parenthesisWorkingBuffer);

  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y()+k_parenthesisCurveHeight+k_externHeightMargin, k_lineThickness, operandSize().height() - 2*(k_parenthesisCurveHeight+k_externHeightMargin)), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin+operandSize().width()+2*k_widthMargin+k_lineThickness, p.y()+k_parenthesisCurveHeight+2, k_lineThickness, operandSize().height()- 2*(k_parenthesisCurveHeight+k_externHeightMargin)), expressionColor);
}

KDSize ParenthesisLayout::computeSize() {
  return KDSize(operandSize().width() + 2*k_widthMargin + 2*k_lineThickness+2*k_externWidthMargin, operandSize().height());
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

KDSize ParenthesisLayout::operandSize() {
  return (m_operandLayout ? m_operandLayout->size() : KDSize(0, KDText::charSize(KDText::FontSize::Large).height()));
}

}
