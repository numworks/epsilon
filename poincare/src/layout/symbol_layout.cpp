#include "symbol_layout.h"
#include <string.h>
#include <assert.h>

SymbolLayout::SymbolLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * argumentLayout) :
  ExpressionLayout(),
  m_lowerBoundLayout(lowerBoundLayout),
  m_upperBoundLayout(upperBoundLayout),
  m_argumentLayout(argumentLayout)
{
  m_lowerBoundLayout->setParent(this);
  m_upperBoundLayout->setParent(this);
  m_argumentLayout->setParent(this);
  m_baseline = max(m_upperBoundLayout->size().height()+k_boundHeightMargin+k_symbolHeight, m_argumentLayout->baseline());
}

SymbolLayout::~SymbolLayout() {
  delete m_lowerBoundLayout;
  delete m_upperBoundLayout;
  delete m_argumentLayout;
}

KDSize SymbolLayout::computeSize() {
  KDSize argumentSize = m_argumentLayout->size();
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  return KDSize(
    max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin+argumentSize.width(),
    m_baseline + max(k_boundHeightMargin+lowerBoundSize.height(), argumentSize.height() - m_argumentLayout->baseline())
  );
}

ExpressionLayout * SymbolLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_upperBoundLayout;
    case 1:
      return m_lowerBoundLayout;
    case 2:
      return m_argumentLayout;
    default:
      return nullptr;
  }
}

KDPoint SymbolLayout::positionOfChild(ExpressionLayout * child) {
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_lowerBoundLayout) {
    x = max(max(0, (k_symbolWidth-lowerBoundSize.width())/2), (upperBoundSize.width()-lowerBoundSize.width())/2);
    y = m_baseline + k_boundHeightMargin;
  } else if (child == m_upperBoundLayout) {
    x = max(max(0, (k_symbolWidth-upperBoundSize.width())/2), (lowerBoundSize.width()-upperBoundSize.width())/2);
    y = m_baseline - k_symbolHeight- k_boundHeightMargin-upperBoundSize.height();
  } else if (child == m_argumentLayout) {
    x = max(max(k_symbolWidth, lowerBoundSize.width()), upperBoundSize.width())+k_argumentWidthMargin;
    y = m_baseline - m_argumentLayout->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}
