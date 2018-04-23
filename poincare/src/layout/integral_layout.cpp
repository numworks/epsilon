#include "integral_layout.h"
#include "char_layout.h"
#include "horizontal_layout.h"
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

ExpressionLayout * IntegralLayout::clone() const {
  IntegralLayout * layout = new IntegralLayout(const_cast<IntegralLayout *>(this)->integrandLayout(), const_cast<IntegralLayout *>(this)->lowerBoundLayout(), const_cast<IntegralLayout *>(this)->upperBoundLayout(), true);
  return layout;
}

void IntegralLayout::deleteBeforeCursor(ExpressionLayoutCursor * cursor) {
  if (cursor->isEquivalentTo(ExpressionLayoutCursor(integrandLayout(), ExpressionLayoutCursor::Position::Left))) {
    // Case: Left of the integrand.
    // Delete the layout, keep the integrand.
    replaceWithAndMoveCursor(integrandLayout(), true, cursor);
    return;
  }
  ExpressionLayout::deleteBeforeCursor(cursor);
}

ExpressionLayoutCursor IntegralLayout::cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Left the upper or lower bound. Go Left of the integral.
  if (((upperBoundLayout()
        && cursor.pointedExpressionLayout() == upperBoundLayout())
      || (lowerBoundLayout()
        && cursor.pointedExpressionLayout() == lowerBoundLayout()))
      && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Left);
  }
  // Case: Left the integrand. Go Right of the lower bound.
 if (integrandLayout()
     && cursor.pointedExpressionLayout() == integrandLayout()
     && cursor.position() == ExpressionLayoutCursor::Position::Left)
  {
    assert(lowerBoundLayout() != nullptr);
    return ExpressionLayoutCursor(lowerBoundLayout(), ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Right of the integral. Go to the integrand.
  if (cursor.position() == ExpressionLayoutCursor::Position::Right) {
    assert(integrandLayout() != nullptr);
    return ExpressionLayoutCursor(integrandLayout(), ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor.position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left of the brackets. Ask the parent.
  if (m_parent) {
    return m_parent->cursorLeftOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor IntegralLayout::cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) {
  // Case: Right the upper or lower bound.
  // Go Left of the integrand.
  if (((upperBoundLayout()
        && cursor.pointedExpressionLayout() == upperBoundLayout())
      || (lowerBoundLayout()
        && cursor.pointedExpressionLayout() == lowerBoundLayout()))
      && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    assert(integrandLayout() != nullptr);
    return ExpressionLayoutCursor(integrandLayout(), ExpressionLayoutCursor::Position::Left);
  }
  // Case: Right the integrand. Go Right.
 if (integrandLayout()
     && cursor.pointedExpressionLayout() == integrandLayout()
     && cursor.position() == ExpressionLayoutCursor::Position::Right)
  {
    return ExpressionLayoutCursor(this, ExpressionLayoutCursor::Position::Right);
  }
  assert(cursor.pointedExpressionLayout() == this);
  // Case: Left of the integral. Go to the upper bound.
  if (cursor.position() == ExpressionLayoutCursor::Position::Left) {
    assert(upperBoundLayout() != nullptr);
    return ExpressionLayoutCursor(upperBoundLayout(), ExpressionLayoutCursor::Position::Left);
  }
  assert(cursor.position() == ExpressionLayoutCursor::Position::Right);
  // Case: Right. Ask the parent.
  if (m_parent) {
    return m_parent->cursorRightOf(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayoutCursor();
}

ExpressionLayoutCursor IntegralLayout::cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the lower bound, move it to the upper bound.
  if (lowerBoundLayout() && cursor.pointedExpressionLayout()->hasAncestor(lowerBoundLayout(), true)) {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the integrand, move it to the upper bound.
  if (integrandLayout()
      && cursor.isEquivalentTo(ExpressionLayoutCursor(integrandLayout(), ExpressionLayoutCursor::Position::Left)))
  {
    assert(upperBoundLayout() != nullptr);
    return upperBoundLayout()->cursorInDescendantsAbove(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorAbove(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

ExpressionLayoutCursor IntegralLayout::cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  // If the cursor is inside the upper bound, move it to the lower bound.
  if (upperBoundLayout() && cursor.pointedExpressionLayout()->hasAncestor(upperBoundLayout(), true)) {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  // If the cursor is Left of the integrand, move it to the lower bound.
  if (integrandLayout()
      && cursor.isEquivalentTo(ExpressionLayoutCursor(integrandLayout(), ExpressionLayoutCursor::Position::Left)))
  {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout()->cursorInDescendantsUnder(cursor, shouldRecomputeLayout);
  }
  return ExpressionLayout::cursorUnder(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int IntegralLayout::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;

  // Write the operator name
  int numberOfChar = strlcpy(buffer, "int", bufferSize);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  // Write the opening parenthesis
  buffer[numberOfChar++] = '(';
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  // Write the argument
  numberOfChar += const_cast<IntegralLayout *>(this)->integrandLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the lower bound
  numberOfChar += const_cast<IntegralLayout *>(this)->lowerBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the upper bound
  numberOfChar += const_cast<IntegralLayout *>(this)->upperBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing parenthesis
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

void IntegralLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize integrandSize = integrandLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];

  // Render the integral symbol.
  KDRect topSymbolFrame(p.x() + k_symbolWidth + k_lineThickness, p.y() + upperBoundSize.height() - k_boundHeightMargin,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(topSymbolFrame, expressionColor, (const uint8_t *)topSymbolPixel, (KDColor *)workingBuffer);
  KDRect bottomSymbolFrame(p.x(),
    p.y() + upperBoundSize.height() + 2*k_integrandHeigthMargin + integrandSize.height() + k_boundHeightMargin - k_symbolHeight,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(bottomSymbolFrame, expressionColor, (const uint8_t *)bottomSymbolPixel, (KDColor *)workingBuffer);
  ctx->fillRect(KDRect(p.x() + k_symbolWidth, p.y() + upperBoundSize.height() - k_boundHeightMargin, k_lineThickness,
    2*k_boundHeightMargin+2*k_integrandHeigthMargin+integrandSize.height()), expressionColor);

  // Render "dx".
  CharLayout * dummydx = new CharLayout('d');
  HorizontalLayout dummyLayout(integrandLayout()->clone(), dummydx, false);
  KDPoint dxPosition = dummyLayout.positionOfChild(dummydx);
  ctx->drawString("dx", dxPosition.translatedBy(p).translatedBy(positionOfChild(integrandLayout())), dummydx->fontSize(), expressionColor, backgroundColor);
}

KDSize IntegralLayout::computeSize() {
  KDSize dxSize = HorizontalLayout(new CharLayout('d'), new CharLayout('x'), false).size();
  KDSize integrandSize = integrandLayout()->size();
  KDSize lowerBoundSize = lowerBoundLayout()->size();
  KDSize upperBoundSize = upperBoundLayout()->size();
  return KDSize(
    k_symbolWidth+k_lineThickness+k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin+integrandSize.width()+dxSize.width(),
    upperBoundSize.height()+ 2*k_integrandHeigthMargin+max(integrandSize.height(), dxSize.height())+lowerBoundSize.height());
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

}
