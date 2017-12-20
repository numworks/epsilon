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

IntegralLayout::IntegralLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * integrand, bool cloneOperands) :
  StaticLayoutHierarchy<3>(upperBound, lowerBound, integrand, cloneOperands)
{
  computeBaseline();
}

ExpressionLayout * IntegralLayout::clone() const {
  IntegralLayout * layout = new IntegralLayout(const_cast<IntegralLayout *>(this)->lowerBoundLayout(), const_cast<IntegralLayout *>(this)->upperBoundLayout(), const_cast<IntegralLayout *>(this)->integrandLayout(), true);
  return layout;
}

bool IntegralLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  // Case: Left the upper or lower bound.
  // Go Left of the integral.
  if (((upperBoundLayout()
        && cursor->pointedExpressionLayout() == upperBoundLayout())
      || (lowerBoundLayout()
        && cursor->pointedExpressionLayout() == lowerBoundLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    cursor->setPointedExpressionLayout(this);
    return true;
  }
  // Case: Left the integrand.
  // Go Right of the lower bound.
 if (integrandLayout()
     && cursor->pointedExpressionLayout() == integrandLayout()
     && cursor->position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(lowerBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(lowerBoundLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return true;
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right of the integral.
  // Go Right of the integrand, Left of "dx".
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(integrandLayout() != nullptr);
    cursor->setPointedExpressionLayout(integrandLayout());
    return integrandLayout()->moveLeft(cursor);
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
  if (((upperBoundLayout()
        && cursor->pointedExpressionLayout() == upperBoundLayout())
      || (lowerBoundLayout()
        && cursor->pointedExpressionLayout() == lowerBoundLayout()))
      && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(integrandLayout() != nullptr);
    cursor->setPointedExpressionLayout(integrandLayout());
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  // Case: Right the integrand.
  // Go Right and move Right.
 if (integrandLayout()
     && cursor->pointedExpressionLayout() == integrandLayout()
     && cursor->position() == ExpressionLayoutCursor::Position::Right)
  {
    cursor->setPointedExpressionLayout(this);
    return m_parent->moveRight(cursor);
  }
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Left of the integral.
  // Go ti the upper bound.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left) {
    assert(upperBoundLayout() != nullptr);
    cursor->setPointedExpressionLayout(upperBoundLayout());
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveRight(cursor);
  }
  return false;
}

bool IntegralLayout::moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the lower bound, move it to the upper bound.
  if (lowerBoundLayout() && previousLayout == lowerBoundLayout()) {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor);
  }
  // If the cursor is Left of the integrand, move it to the upper bound.
  if (integrandLayout()
      && previousLayout == integrandLayout()
      && cursor->positionIsEquivalentTo(integrandLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->moveUpInside(cursor);
  }
  return ExpressionLayout::moveUp(cursor, previousLayout, previousPreviousLayout);
}

bool IntegralLayout::moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) {
  // If the cursor is inside the upper bound, move it to the lower bound.
  if (upperBoundLayout() && previousLayout == upperBoundLayout()) {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor);
  }
  // If the cursor is Left of the integrand, move it to the lower bound.
  if (integrandLayout()
      && previousLayout == integrandLayout()
      && cursor->positionIsEquivalentTo(integrandLayout(), ExpressionLayoutCursor::Position::Left))
  {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->moveDownInside(cursor);
  }
  return ExpressionLayout::moveDown(cursor, previousLayout, previousPreviousLayout);
}

void IntegralLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize integrandSize = integrandLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
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
  KDSize integrandSize = integrandLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  return KDSize(
    k_symbolWidth+k_lineThickness+k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin+integrandSize.width(),
    upperBoundSize.height()+ 2*k_integrandHeigthMargin+integrandSize.height()+lowerBoundSize.height());
}

void IntegralLayout::computeBaseline() {
  m_baseline = upperBoundLayout()->size().height() + k_integrandHeigthMargin + integrandLayout()->baseline();
  m_baselined = true;
}

KDPoint IntegralLayout::positionOfChild(ExpressionLayout * child) {
  KDSize integrandSize = integrandLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == lowerBoundLayout()) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;
    y = upperBoundSize.height()+2*k_integrandHeigthMargin+integrandSize.height();
  } else if (child == upperBoundLayout()) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;;
    y = 0;
  } else if (child == integrandLayout()) {
    x = k_symbolWidth +k_lineThickness+ k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin;
    y = upperBoundSize.height()+k_integrandHeigthMargin;
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

ExpressionLayout * IntegralLayout::upperBoundLayout() {
  return editableChild(0);
}

ExpressionLayout * IntegralLayout::lowerBoundLayout() {
  return editableChild(1);
}

ExpressionLayout * IntegralLayout::integrandLayout() {
  return editableChild(2);
}

}
