#include <poincare/integral_layout.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

static inline KDCoordinate max(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

const uint8_t topSymbolPixel[IntegralLayoutNode::k_symbolHeight][IntegralLayoutNode::k_symbolWidth] = {
  {0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00},
  {0xFF, 0xFF, 0x00, 0x00},
};

const uint8_t bottomSymbolPixel[IntegralLayoutNode::k_symbolHeight][IntegralLayoutNode::k_symbolWidth] = {
  {0x00, 0x00, 0xFF, 0xFF},
  {0x00, 0x00, 0xFF, 0xFF},
  {0xFF, 0x00, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x00, 0x00},
};

void IntegralLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Left
      && (cursor->layoutNode() == upperBoundLayout()
        || cursor->layoutNode() == lowerBoundLayout()))
  {
    // Case: Left the upper or lower bound. Go Left of the integral.
    cursor->setLayoutNode(this);
    return;
  }
 if (cursor->layoutNode() == integrandLayout()
     && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left the integrand. Go Right of the lower bound.
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right of the integral. Go to the integrand.
    cursor->setLayoutNode(integrandLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  // Case: Left of the brackets. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void IntegralLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
  if (cursor->position() == LayoutCursor::Position::Right &&
      (cursor->layoutNode() == upperBoundLayout()
       || cursor->layoutNode() == lowerBoundLayout()))
  {
    // Case: Right the upper or lower bound. Go Left of the integrand.
    cursor->setLayoutNode(integrandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == integrandLayout()
     && cursor->position() == LayoutCursor::Position::Right)
  {
    // Case: Right the integrand. Go Right.
    cursor->setLayoutNode(this);
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left of the integral. Go to the upper bound.
    cursor->setLayoutNode(upperBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void IntegralLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (cursor->layoutNode()->hasAncestor(lowerBoundLayout(), true)) {
    // If the cursor is inside the lower bound, move it to the upper bound.
    upperBoundLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  if (cursor->isEquivalentTo(LayoutCursor(integrandLayout(), LayoutCursor::Position::Left))) {
    // If the cursor is Left of the integrand, move it to the upper bound.
    upperBoundLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void IntegralLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) {
  if (cursor->layoutNode()->hasAncestor(upperBoundLayout(), true)) {
    // If the cursor is inside the upper bound, move it to the lower bound.
    lowerBoundLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  // If the cursor is Left of the integrand, move it to the lower bound.
  if (cursor->isEquivalentTo(LayoutCursor(integrandLayout(), LayoutCursor::Position::Left))) {
    lowerBoundLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
    return;
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void IntegralLayoutNode::deleteBeforeCursor(LayoutCursor * cursor) {
  if (cursor->isEquivalentTo(LayoutCursor(integrandLayout(), LayoutCursor::Position::Left))) {
    // Case: Left of the integrand. Delete the layout, keep the integrand.
    Layout thisRef = Layout(this);
    Layout integrand = Layout(integrandLayout());
    thisRef.replaceChildWithGhostInPlace(integrand);
    // WARNING: Do not use "this" afterwards
    cursor->setLayout(thisRef.childAtIndex(0));
    cursor->setPosition(LayoutCursor::Position::Left);
    thisRef.replaceWith(integrand, cursor);
    return;
  }
  LayoutNode::deleteBeforeCursor(cursor);
}

int IntegralLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
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
  numberOfChar += const_cast<IntegralLayoutNode *>(this)->integrandLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the lower bound
  numberOfChar += const_cast<IntegralLayoutNode *>(this)->lowerBoundLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the comma
  buffer[numberOfChar++] = ',';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the upper bound
  numberOfChar += const_cast<IntegralLayoutNode *>(this)->upperBoundLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

  // Write the closing parenthesis
  buffer[numberOfChar++] = ')';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

KDSize IntegralLayoutNode::computeSize() {
  KDSize dxSize = k_font->stringSize("dx");
  KDSize integrandSize = integrandLayout()->layoutSize();
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDCoordinate width = k_symbolWidth+k_lineThickness+k_boundWidthMargin+max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin+integrandSize.width()+dxSize.width();
  KDCoordinate height = upperBoundSize.height()+ 2*k_integrandHeigthMargin+max(integrandSize.height(), dxSize.height())+lowerBoundSize.height();
  return KDSize(width, height);
}

KDCoordinate IntegralLayoutNode::computeBaseline() {
  return upperBoundLayout()->layoutSize().height() + k_integrandHeigthMargin + integrandLayout()->baseline();
}

KDPoint IntegralLayoutNode::positionOfChild(LayoutNode * child) {
  KDSize integrandSize = integrandLayout()->layoutSize();
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
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

void IntegralLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDSize integrandSize = integrandLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];

  // Render the integral symbol
  KDRect topSymbolFrame(p.x() + k_symbolWidth + k_lineThickness, p.y() + upperBoundSize.height() - k_boundHeightMargin,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(topSymbolFrame, expressionColor, (const uint8_t *)topSymbolPixel, (KDColor *)workingBuffer);
  KDRect bottomSymbolFrame(p.x(),
    p.y() + upperBoundSize.height() + 2*k_integrandHeigthMargin + integrandSize.height() + k_boundHeightMargin - k_symbolHeight,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(bottomSymbolFrame, expressionColor, (const uint8_t *)bottomSymbolPixel, (KDColor *)workingBuffer);
  ctx->fillRect(KDRect(p.x() + k_symbolWidth, p.y() + upperBoundSize.height() - k_boundHeightMargin, k_lineThickness,
    2*k_boundHeightMargin+2*k_integrandHeigthMargin+integrandSize.height()), expressionColor);

  // Render "dx"
  KDPoint dxPosition = p.translatedBy(positionOfChild(integrandLayout())).translatedBy(KDPoint(integrandSize.width(), integrandLayout()->baseline() - k_font->glyphSize().height()/2));
  ctx->drawString("dx", dxPosition, k_font, expressionColor, backgroundColor);
}

}
