#include "integral_layout.h"
#include <poincare/expression_layout_cursor.h>
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

bool IntegralLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left the upper or lower bound.
  // Go Left of the integral.
  if (((m_upperBoundLayout
        && cursor->pointedExpressionLayout() == m_upperBoundLayout)
      || (m_lowerBoundLayout
        && cursor->pointedExpressionLayout() == m_lowerBoundLayout))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left the integrand.
  // Go Right of the lower bound.
 if (m_integrandLayout
     && cursor->pointedExpressionLayout() == m_integrandLayout
     && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(m_lowerBoundLayout != nullptr);
    cursor->setPointedExpressionLayout(m_lowerBoundLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right of the integral.
  // Go Right of the integrand, Left of "dx".
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(m_integrandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_integrandLayout);
    return m_integrandLayout->moveLeft(cursor);
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left of the brackets.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

bool IntegralLayout::moveRight(ExpressionLayoutCursor * cursor) {
  // Case: Right the upper or lower bound.
  // Go Left of the integrand.
  if (((m_upperBoundLayout
        && cursor->pointedExpressionLayout() == m_upperBoundLayout)
      || (m_lowerBoundLayout
        && cursor->pointedExpressionLayout() == m_lowerBoundLayout))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(m_integrandLayout != nullptr);
    cursor->setPointedExpressionLayout(m_integrandLayout);
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right the integrand.
  // Go Right and move Right.
 if (m_integrandLayout
     && cursor->pointedExpressionLayout() == m_integrandLayout
     && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return m_parent->moveRight(cursor);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left of the integral.
  // Go ti the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(m_upperBoundLayout != nullptr);
    cursor->setPointedExpressionLayout(m_upperBoundLayout);
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
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
