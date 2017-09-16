#include "bracket_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

BracketLayout::BracketLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline();
}

BracketLayout::~BracketLayout() {
  delete m_operandLayout;
}

void BracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  const KDCoordinate k_widthMargin = widthMargin();
  KDSize operandSize = m_operandLayout->size();
  ctx->fillRect(KDRect(p.x(), p.y(), k_lineThickness, m_operandLayout->size().height()), expressionColor);
  ctx->fillRect(KDRect(p.x()+operandSize.width()+2*k_widthMargin+k_lineThickness, p.y(), k_lineThickness, m_operandLayout->size().height()), expressionColor);
  if (renderTopBar()) {
    ctx->fillRect(KDRect(p.x(), p.y(), k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_lineThickness+operandSize.width()+2*k_widthMargin-k_bracketWidth, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  }
  if (renderBottomBar()) {
    ctx->fillRect(KDRect(p.x(), p.y()+operandSize.height()-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x()+k_lineThickness+operandSize.width()+2*k_widthMargin-k_bracketWidth, p.y()+operandSize.height()-k_lineThickness, k_bracketWidth, k_lineThickness), expressionColor);
  }
}

KDSize BracketLayout::computeSize() {
  const KDCoordinate k_widthMargin = widthMargin();
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width() + 2*k_widthMargin + 2*k_lineThickness, operandSize.height());
}

ExpressionLayout * BracketLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint BracketLayout::positionOfChild(ExpressionLayout * child) {
  const KDCoordinate k_widthMargin = widthMargin();
  return KDPoint(k_widthMargin+k_lineThickness, 0);
}

}
