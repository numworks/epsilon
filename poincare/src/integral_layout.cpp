#include <poincare/integral_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

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

void IntegralLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == upperBoundLayout()
      || cursor->layoutNode() == lowerBoundLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left the upper or lower bound. Go Left of the integral.
    cursor->setLayoutNode(this);
    return;
  }
  if (cursor->layoutNode() == differentialLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left of the variable differential. Go Right of the integrand
    cursor->setLayoutNode(integrandLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == integrandLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Left);
    // Case: Left the integrand. Go Right of the lower bound.
    cursor->setLayoutNode(lowerBoundLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right of the integral. Go to the differential.
    cursor->setLayoutNode(differentialLayout());
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  // Case: Left of the integral. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void IntegralLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == upperBoundLayout()
      || cursor->layoutNode() == lowerBoundLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right the upper or lower bound. Go Left of the integrand.
    cursor->setLayoutNode(integrandLayout());
    cursor->setPosition(LayoutCursor::Position::Left);
    return;
  }
  if (cursor->layoutNode() == integrandLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right the differential. Go Right.
    cursor->setLayoutNode(this);
    cursor->setPosition(LayoutCursor::Position::Right);
    return;
  }
  if (cursor->layoutNode() == differentialLayout())
  {
    assert(cursor->position() == LayoutCursor::Position::Right);
    // Case: Right the differential. Go Right.
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

void IntegralLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
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

void IntegralLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
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

  /* TODO
   * For now, we serialize
   *    2
   *    ∫3dx as int{{3},{x},{1},{2}}
   *    1
   * To save space, we could serialize it as int{3}{x}{1}{2} and modify the
   * parser accordingly.
   * This could be done for other layouts too. */

  /* Add system parentheses to avoid serializing:
   *   2)+(1          2),1
   *    ∫    (5)dx or  ∫    (5)dx
   *    1             1+binomial(3
   */
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize-1) {
    return bufferSize-1;
  }

  LayoutNode * argLayouts[] = {
    const_cast<IntegralLayoutNode *>(this)->integrandLayout(),
    const_cast<IntegralLayoutNode *>(this)->differentialLayout(),
    const_cast<IntegralLayoutNode *>(this)->lowerBoundLayout(),
    const_cast<IntegralLayoutNode *>(this)->upperBoundLayout()};

  for (uint8_t i = 0; i < sizeof(argLayouts)/sizeof(argLayouts[0]); i++) {
    if (i != 0) {
      // Write the comma
      numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, ',');
      if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    }

    // Write the argument with system parentheses
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointLeftSystemParenthesis);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += argLayouts[i]->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }

  // Write the closing system parenthesis
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointRightSystemParenthesis);
  return numberOfChar;
}

CodePoint IntegralLayoutNode::XNTCodePoint(int childIndex) const {
  return (childIndex == k_integrandLayoutIndex || childIndex == k_differentialLayoutIndex) ? CodePoint('x') : UCodePointNull;
}

KDSize IntegralLayoutNode::computeSize() {
  KDSize dSize = k_font->stringSize("d");
  KDSize integrandSize = integrandLayout()->layoutSize();
  KDSize differentialSize = differentialLayout()->layoutSize();
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDCoordinate width = k_symbolWidth+k_lineThickness+k_boundWidthMargin+std::max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin+integrandSize.width()+2*k_differentialWidthMargin+dSize.width()+differentialSize.width();
  KDCoordinate baseline = computeBaseline();
  KDCoordinate height = baseline + k_integrandHeigthMargin+std::max(integrandSize.height()-integrandLayout()->baseline(), differentialSize.height()-differentialLayout()->baseline())+lowerBoundSize.height();
  return KDSize(width, height);
}

KDCoordinate IntegralLayoutNode::computeBaseline() {
  return upperBoundLayout()->layoutSize().height() + k_integrandHeigthMargin + std::max(integrandLayout()->baseline(), differentialLayout()->baseline());
}

KDPoint IntegralLayoutNode::positionOfChild(LayoutNode * child) {
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == lowerBoundLayout()) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;
    y = computeSize().height()-lowerBoundSize.height();
  } else if (child == upperBoundLayout()) {
    x = k_symbolWidth+k_lineThickness+k_boundWidthMargin;;
    y = 0;
  } else if (child == integrandLayout()) {
    x = k_symbolWidth +k_lineThickness+ k_boundWidthMargin+std::max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandWidthMargin;
    y = computeBaseline()-integrandLayout()->baseline();
  } else if (child == differentialLayout()) {
    x = computeSize().width() - k_differentialWidthMargin - differentialLayout()->layoutSize().width();
    y = computeBaseline()-differentialLayout()->baseline();
  } else {
    assert(false);
  }
  return KDPoint(x,y);
}

void IntegralLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  KDSize integrandSize = integrandLayout()->layoutSize();
  KDSize differentialSize = differentialLayout()->layoutSize();
  KDSize upperBoundSize = upperBoundLayout()->layoutSize();
  KDCoordinate centralArgumentHeight =  std::max(integrandLayout()->baseline(), differentialLayout()->baseline()) + std::max(integrandSize.height()-integrandLayout()->baseline(), differentialSize.height()-differentialLayout()->baseline());

  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];

  // Render the integral symbol
  KDRect topSymbolFrame(p.x() + k_symbolWidth + k_lineThickness, p.y() + upperBoundSize.height() - k_boundHeightMargin,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(topSymbolFrame, expressionColor, (const uint8_t *)topSymbolPixel, (KDColor *)workingBuffer);
  KDRect bottomSymbolFrame(p.x(),
    p.y() + upperBoundSize.height() + 2*k_integrandHeigthMargin + centralArgumentHeight + k_boundHeightMargin - k_symbolHeight,
    k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(bottomSymbolFrame, expressionColor, (const uint8_t *)bottomSymbolPixel, (KDColor *)workingBuffer);
  ctx->fillRect(KDRect(p.x() + k_symbolWidth, p.y() + upperBoundSize.height() - k_boundHeightMargin, k_lineThickness,
    2*k_boundHeightMargin+2*k_integrandHeigthMargin+centralArgumentHeight), expressionColor);

  // Render "d"
  KDPoint dPosition = p.translatedBy(positionOfChild(integrandLayout())).translatedBy(KDPoint(integrandSize.width() + k_differentialWidthMargin, integrandLayout()->baseline() - k_font->glyphSize().height()/2));
  ctx->drawString("d", dPosition, k_font, expressionColor, backgroundColor);
}

}
