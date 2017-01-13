#include "subscript_layout.h"
#include <string.h>
#include <assert.h>

SubscriptLayout::SubscriptLayout(ExpressionLayout * baseLayout, ExpressionLayout * subscriptLayout) :
  ExpressionLayout(),
  m_baseLayout(baseLayout),
  m_subscriptLayout(subscriptLayout)
{
  m_baseLayout->setParent(this);
  m_subscriptLayout->setParent(this);
  m_baseline = m_baseLayout->baseline();
}

SubscriptLayout::~SubscriptLayout() {
  delete m_baseLayout;
  delete m_subscriptLayout;
}

void SubscriptLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a power, only the position of the children matters
}

KDSize SubscriptLayout::computeSize() {
  KDSize baseSize = m_baseLayout->size();
  KDSize subscriptSize = m_subscriptLayout->size();
  return KDSize(baseSize.width() + subscriptSize.width(), m_baseLayout->baseline() + subscriptSize.height() - k_subscriptHeight);
}

ExpressionLayout * SubscriptLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_baseLayout;
    case 1:
      return m_subscriptLayout;
    default:
      return nullptr;
  }
}

KDPoint SubscriptLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_subscriptLayout) {
    x = m_baseLayout->size().width();
    y = m_baseLayout->baseline() - k_subscriptHeight;
  }
  return KDPoint(x,y);
}
