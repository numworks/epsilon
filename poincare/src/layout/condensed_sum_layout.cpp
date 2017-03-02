#include "condensed_sum_layout.h"
#include <string.h>
#include <assert.h>

namespace Poincare {

CondensedSumLayout::CondensedSumLayout(ExpressionLayout * baseLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) :
  ExpressionLayout(),
  m_baseLayout(baseLayout),
  m_subscriptLayout(subscriptLayout),
  m_superscriptLayout(superscriptLayout)
{
  m_baseLayout->setParent(this);
  m_subscriptLayout->setParent(this);
  if (m_superscriptLayout) {
    m_superscriptLayout->setParent(this);
  }
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  m_baseline = m_baseLayout->baseline() + max(0, superscriptSize.height() - m_baseLayout->size().height()/2);
}

CondensedSumLayout::~CondensedSumLayout() {
  delete m_baseLayout;
  delete m_subscriptLayout;
  if (m_superscriptLayout) {
    delete m_superscriptLayout;
  }
}

void CondensedSumLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // Nothing to draw
}

KDSize CondensedSumLayout::computeSize() {
  KDSize baseSize = m_baseLayout->size();
  KDSize subscriptSize = m_subscriptLayout->size();
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  return KDSize(baseSize.width() + max(subscriptSize.width(), superscriptSize.width()), max(baseSize.height()/2, subscriptSize.height()) + max(baseSize.height()/2, superscriptSize.height()));
}

ExpressionLayout * CondensedSumLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_baseLayout;
    case 1:
      return m_subscriptLayout;
    case 2:
      return m_superscriptLayout;
    default:
      return nullptr;
  }
}

KDPoint CondensedSumLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize baseSize = m_baseLayout->size();
  KDSize superscriptSize = m_superscriptLayout == nullptr ? KDSizeZero : m_superscriptLayout->size();
  if (child == m_baseLayout) {
    y = max(0, superscriptSize.height() - baseSize.height()/2);
  }
  if (child == m_subscriptLayout) {
    x = baseSize.width();
    y = max(baseSize.height()/2, superscriptSize.height());
  }
  if (child == m_superscriptLayout) {
    x = baseSize.width();
  }
  return KDPoint(x,y);
}

}

