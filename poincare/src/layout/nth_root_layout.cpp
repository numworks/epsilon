#include <string.h>
#include <assert.h>
#include "nth_root_layout.h"

const uint8_t radixPixel[NthRootLayout::k_leftRadixHeight][NthRootLayout::k_leftRadixWidth] = {
  {0x00, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0x00},
};

NthRootLayout::NthRootLayout(ExpressionLayout * radicandLayout, ExpressionLayout * indexLayout) :
  ExpressionLayout(),
  m_radicandLayout(radicandLayout),
  m_indexLayout(indexLayout)
{
  m_radicandLayout->setParent(this);
  m_indexLayout->setParent(this);
  m_baseline = max(m_radicandLayout->baseline() + k_radixLineThickness + k_heightMargin,
    m_indexLayout->size().height() + k_leftRadixHeight - m_radicandLayout->size().height() - k_heightMargin + m_radicandLayout->baseline());
}

NthRootLayout::~NthRootLayout() {
  delete m_radicandLayout;
  delete m_indexLayout;
}

void NthRootLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize radicandSize = m_radicandLayout->size();
  KDSize indexSize = m_indexLayout->size();
  KDColor workingBuffer[k_leftRadixWidth*k_leftRadixHeight];
  KDRect leftRadixFrame(p.x() + indexSize.width() + k_widthMargin - k_leftRadixWidth,
    p.y() + max(indexSize.height(), k_radixLineThickness+2*k_heightMargin+radicandSize.height()-k_leftRadixHeight),
    k_leftRadixWidth, k_leftRadixHeight);
  ctx->blendRectWithMask(leftRadixFrame, expressionColor, (const uint8_t *)radixPixel, (KDColor *)workingBuffer);
  if (indexSize.height() + k_leftRadixHeight > k_radixLineThickness + radicandSize.height() + 2*k_heightMargin) {
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() + k_leftRadixHeight - radicandSize.height() - k_radixLineThickness - 2*k_heightMargin,
                         k_radixLineThickness,
                         radicandSize.height() + 2*k_heightMargin + k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y() + indexSize.height() + k_leftRadixHeight - radicandSize.height() - k_radixLineThickness - 2*k_heightMargin,
                         radicandSize.width() + 2*k_widthMargin,
                         k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin + radicandSize.width() + 2*k_widthMargin,
                         p.y() + indexSize.height() + k_leftRadixHeight - radicandSize.height() - k_radixLineThickness - 2*k_heightMargin,
                         k_radixLineThickness,
                         k_rightRadixHeight + k_radixLineThickness), expressionColor);
  } else {
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         k_radixLineThickness,
                         radicandSize.height() + 2*k_heightMargin + k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin,
                         p.y(),
                         radicandSize.width() + 2*k_widthMargin,
                         k_radixLineThickness), expressionColor);
    ctx->fillRect(KDRect(p.x() + indexSize.width() + k_widthMargin + radicandSize.width() + 2*k_widthMargin,
                         p.y(),
                         k_radixLineThickness,
                         k_rightRadixHeight + k_radixLineThickness), expressionColor);
  }

}

KDSize NthRootLayout::computeSize() {
  KDSize radicandSize = m_radicandLayout->size();
  KDSize indexSize = m_indexLayout->size();
  return KDSize(
      indexSize.width() + 3*k_widthMargin + 2*k_radixLineThickness + radicandSize.width(),
      max(k_radixLineThickness + 2*k_heightMargin + radicandSize.height(), indexSize.height() + k_leftRadixHeight)
      );
}

ExpressionLayout * NthRootLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_indexLayout;
    case 1:
      return m_radicandLayout;
    default:
      return nullptr;
  }
}

KDPoint NthRootLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDSize radicandSize = m_radicandLayout->size();
  KDSize indexSize = m_indexLayout->size();
  if (child == m_indexLayout) {
    x = 0;
    if (indexSize.height() + k_leftRadixHeight > k_radixLineThickness + radicandSize.height() + 2*k_heightMargin) {
      y = 0;
    } else {
      y = k_radixLineThickness+2*k_heightMargin+radicandSize.height()-k_leftRadixHeight - indexSize.height();
    }
  } else if (child == m_radicandLayout) {
    x = indexSize.width() + 2*k_widthMargin + k_radixLineThickness;
    if (indexSize.height() + k_leftRadixHeight > k_radixLineThickness + radicandSize.height() + 2*k_heightMargin) {
      y = indexSize.height() + k_leftRadixHeight - radicandSize.height() - k_heightMargin;
    } else {
      y = k_radixLineThickness + k_heightMargin;
    }
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}
