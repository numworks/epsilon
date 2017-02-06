#include "integral_layout.h"
#include <string.h>
#include <assert.h>

namespace Poincare {

const uint8_t topSymbolPixel[IntegralLayout::k_symbolHeight][IntegralLayout::k_symbolWidth] = {
  {0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00},
  {0xFF, 0xFF, 0x00, 0x00},
};

const uint8_t bottomSymbolPixel[IntegralLayout::k_symbolHeight][IntegralLayout::k_symbolWidth] = {
  {0x00, 0x00, 0xFF, 0xFF},
  {0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00},
};

IntegralLayout::IntegralLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * integrandLayout) :
  ExpressionLayout(),
  m_lowerBoundLayout(lowerBoundLayout),
  m_upperBoundLayout(upperBoundLayout),
  m_integrandLayout(integrandLayout)
{
  m_lowerBoundLayout->setParent(this);
  m_upperBoundLayout->setParent(this);
  m_integrandLayout->setParent(this);
  m_baseline = m_upperBoundLayout->size().height() + k_integrandHeigthMargin + m_integrandLayout->baseline();
}

IntegralLayout::~IntegralLayout() {
  delete m_lowerBoundLayout;
  delete m_upperBoundLayout;
  delete m_integrandLayout;
}

void IntegralLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize integrandSize = m_integrandLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];
  KDRect topSymbolFrame(p.x() + k_symbolWidth + k_lineThickness, p.y() + upperBoundSize.height() - k_boundHeightMargin,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(topSymbolFrame, expressionColor, (const uint8_t *)topSymbolPixel, (KDColor *)workingBuffer);
  KDRect bottomSymbolFrame(p.x(),
    p.y() + upperBoundSize.height() + 2*k_integrandHeigthMargin + integrandSize.height() + k_boundHeightMargin - k_symbolHeight,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(bottomSymbolFrame, expressionColor, (const uint8_t *)bottomSymbolPixel, (KDColor *)workingBuffer);
  ctx->fillRect(KDRect(p.x() + k_symbolWidth, p.y() + upperBoundSize.height() - k_boundHeightMargin, k_lineThickness,
    2*k_boundHeightMargin+2*k_integrandHeigthMargin+integrandSize.height()), expressionColor);
}

KDSize IntegralLayout::computeSize() {
  KDSize integrandSize = m_integrandLayout->size();
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  return KDSize(
    k_symbolWidth+k_lineThickness+k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin+integrandSize.width(),
    upperBoundSize.height()+ 2*k_integrandHeigthMargin+integrandSize.height()+lowerBoundSize.height());
}

ExpressionLayout * IntegralLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_upperBoundLayout;
    case 1:
      return m_lowerBoundLayout;
    case 2:
      return m_integrandLayout;
    default:
      return nullptr;
  }
}

KDPoint IntegralLayout::positionOfChild(ExpressionLayout * child) {
  KDSize integrandSize = m_integrandLayout->size();
  KDSize lowerBoundSize = m_lowerBoundLayout->size();
  KDSize upperBoundSize = m_upperBoundLayout->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_lowerBoundLayout) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;
    y = upperBoundSize.height()+2*k_integrandHeigthMargin+integrandSize.height();
  } else if (child == m_upperBoundLayout) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;;
    y = 0;
  } else if (child == m_integrandLayout) {
    x = k_symbolWidth +k_lineThickness+ k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin;
    y = upperBoundSize.height()+k_integrandHeigthMargin;
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

}
