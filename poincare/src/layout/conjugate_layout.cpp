#include "conjugate_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

ConjugateLayout::ConjugateLayout(ExpressionLayout * operandLayout) :
  ExpressionLayout(),
  m_operandLayout(operandLayout)
{
  m_operandLayout->setParent(this);
  m_baseline = m_operandLayout->baseline()+k_overlineWidth+k_overlineMargin;
}

ConjugateLayout::~ConjugateLayout() {
  delete m_operandLayout;
}

void ConjugateLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x(), p.y(), m_operandLayout->size().width(), k_overlineWidth), expressionColor);
}

KDSize ConjugateLayout::computeSize() {
  KDSize operandSize = m_operandLayout->size();
  return KDSize(operandSize.width(), operandSize.height()+k_overlineWidth+k_overlineMargin);
}

ExpressionLayout * ConjugateLayout::child(uint16_t index) {
  if (index == 0) {
    return m_operandLayout;
  }
  return nullptr;
}

KDPoint ConjugateLayout::positionOfChild(ExpressionLayout * child) {
  return KDPoint(0, k_overlineWidth+k_overlineMargin);
}

}

