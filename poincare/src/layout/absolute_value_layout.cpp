#include "absolute_value_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

AbsoluteValueLayout::AbsoluteValueLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline();;
}

AbsoluteValueLayout::~AbsoluteValueLayout() {
  delete m_operandLayout;
}

void AbsoluteValueLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x(), p.y(), k_straightBracketWidth, m_operandLayout->size().height()), expressionColor);
  ctx->fillRect(KDRect(p.x() +  m_operandLayout->size().width() + k_straightBracketWidth + 2*k_straightBracketMargin,
    p.y(), k_straightBracketWidth, m_operandLayout->size().height()), expressionColor);
}

KDSize AbsoluteValueLayout::computeSize() {
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width() + 2*k_straightBracketWidth + 2*k_straightBracketMargin, operandSize.height());
}

ExpressionLayout * AbsoluteValueLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint AbsoluteValueLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(k_straightBracketWidth + k_straightBracketMargin, 0);
}

}
