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

ExpressionLayout * IntegralLayout::clone() const {
  IntegralLayout * layout = new IntegralLayout(const_cast<IntegralLayout *>(this)->integrandLayout(), const_cast<IntegralLayout *>(this)->lowerBoundLayout(), const_cast<IntegralLayout *>(this)->upperBoundLayout(), true);
  return layout;
}

void IntegralLayout::backspaceAtCursor(ExpressionLayoutCursor * cursor) {
  // Case: Left the upper bound, lower bound or argument.
  // Delete the integral, keep the integrand.
  if (cursor->position() == ExpressionLayoutCursor::Position::Left
      && ((upperBoundLayout()
          && cursor->pointedExpressionLayout() == upperBoundLayout())
        || (lowerBoundLayout()
          && cursor->pointedExpressionLayout() == lowerBoundLayout())
        || cursor->positionIsEquivalentTo(integrandLayout(), ExpressionLayoutCursor::Position::Left)))
  {
    ExpressionLayout * previousParent = m_parent;
    int indexInParent = previousParent->indexOfChild(this);
    ExpressionLayout * dxLayout = integrandLayout()->editableChild(integrandLayout()->numberOfChildren()-1);
    replaceWith(integrandLayout(), true);
    // Remove "dx"
    int indexOfdx = dxLayout->parent()->indexOfChild(dxLayout);
    if (indexOfdx >= 0) {
      const_cast<ExpressionLayout *>(dxLayout->parent())->removeChildAtIndex(indexOfdx, true);
    }
    // Place the cursor on the right of the left brother of the integral if
    // there is one.
    if (indexInParent > 0) {
      cursor->setPointedExpressionLayout(previousParent->editableChild(indexInParent - 1));
      cursor->setPosition(ExpressionLayoutCursor::Position::Right);
      return;
    }
    // Else place the cursor on the Left of the parent.
    cursor->setPointedExpressionLayout(previousParent);
    return;
  }
  // If the cursor is on the right, move to the integrand.
  if (cursor->positionIsEquivalentTo(integrandLayout(), ExpressionLayoutCursor::Position::Right)) {
    assert(integrandLayout()->numberOfChildren() > 1);
    ExpressionLayout * layoutLeftOfdx = integrandLayout()->editableChild(integrandLayout()->numberOfChildren()-2);
    cursor->setPointedExpressionLayout(layoutLeftOfdx);
    cursor->setPosition(ExpressionLayoutCursor::Position::Right);
    return;
  }
  if (m_parent) {
    return m_parent->backspaceAtCursor(cursor);
  }
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
  // Go Left of "dx".
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    assert(integrandLayout() != nullptr);
    cursor->setPointedExpressionLayout(integrandLayout()->editableChild(integrandLayout()->numberOfChildren() - 1));
    return cursor->moveLeft();
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
    cursor->setPointedExpressionLayout(integrandLayout()->editableChild(0));
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
  // Go to the upper bound.
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

  // Write the argument without the "dx"
  ExpressionLayout * intLayout = const_cast<IntegralLayout *>(this)->integrandLayout();
  numberOfChar += LayoutEngine::writeInfixExpressionLayoutTextInBuffer(intLayout, buffer+numberOfChar, bufferSize-numberOfChar, "", 0, intLayout->numberOfChildren()-2);
  // TODO This works because the argument layout should always be an horizontal
  // layout.

  // Write the comma
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the lower bound
  numberOfChar += const_cast<IntegralLayout *>(this)->lowerBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);

  // Write the comma
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the upper bound
  numberOfChar += const_cast<IntegralLayout *>(this)->upperBoundLayout()->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);

  // Write the closing parenthesis
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
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
  return editableChild(2);
}

ExpressionLayout * IntegralLayout::lowerBoundLayout() {
  return editableChild(1);
}

ExpressionLayout * IntegralLayout::integrandLayout() {
  return editableChild(0);
}

}
