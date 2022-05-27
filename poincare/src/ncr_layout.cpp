#include <poincare/binomial_coefficient.h>
#include <poincare/ncr_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

const uint8_t symbolUpperHalf[] = {
  0xFF, 0xFF, 0xFF, 0xE9, 0x8D, 0x01, 0x00, 0x00, 0x26, 0x9C, 0xFE, 0xFF,
  0xFF, 0xFF, 0xB7, 0x00, 0x6B, 0xD0, 0xFF, 0xFF, 0xD2, 0x57, 0x59, 0xE8,
  0xFF, 0xDD, 0x00, 0xA9, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0x9C, 0xDC,
  0xFF, 0x42, 0x7D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
  0xD1, 0x00, 0xDD, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE,
  0x9D, 0x58, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
  0x6B, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE,
  0x6B, 0x8D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
};

void NCRLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Left
      && cursor->layoutNode() == kLayout())
  {
    // Case: Left of the right children. Go right of the left children.
    cursor->setPosition(LayoutCursor::Position::Right);
    cursor->setLayoutNode(nLayout());
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left
      && cursor->layoutNode() == nLayout())
  {
    // Case: Left of the left children. Go Left.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the kLayout.
    cursor->setLayoutNode(kLayout());
    return;
  }
  // Case: Left. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Left);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void NCRLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->position() == LayoutCursor::Position::Right
      && cursor->layoutNode() == nLayout())
  {
    // Case: Right of the left children. Go left of the right children.
    cursor->setPosition(LayoutCursor::Position::Left);
    cursor->setLayoutNode(kLayout());
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Right
      && cursor->layoutNode() == kLayout())
  {
    // Case: Right of the right children. Go Right.
    cursor->setLayoutNode(this);
    return;
  }

  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Left) {
    // Case: Left. Go Left of the nLayout.
    cursor->setLayoutNode(nLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

void NCRLayoutNode::moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(kLayout(), true)) {
    // Case: kLayout. Move to nLayout.
    return nLayout()->moveCursorUpInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

void NCRLayoutNode::moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection) {
  if (cursor->layoutNode()->hasAncestor(nLayout(), true)) {
    // Case: nLayout. Move to kLayout.
    return kLayout()->moveCursorDownInDescendants(cursor, shouldRecomputeLayout);
  }
  LayoutNode::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited);
}

int NCRLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomialCoefficient::s_functionHelper.name(), true);
}

KDCoordinate NCRLayoutNode::aboveSymbol() {
  return std::max<KDCoordinate>(nLayout()->baseline(), kLayout()->baseline() - k_symbolHeight);
}

KDCoordinate NCRLayoutNode::totalHeight() {
    KDCoordinate underSymbol = std::max<KDCoordinate>(kLayout()->layoutSize().height() - kLayout()->baseline(), nLayout()->layoutSize().height() - nLayout()->baseline() - k_symbolHeight);
    return aboveSymbol() + k_symbolHeight + underSymbol;
}

KDSize NCRLayoutNode::computeSize() {
  KDCoordinate width = nLayout()->layoutSize().width() + k_symbolWidthWithMargins + kLayout()->layoutSize().width();
  return KDSize(width, totalHeight());
}

KDCoordinate NCRLayoutNode::computeBaseline() {
  return std::max(0, aboveSymbol() + k_symbolBaseline);
}

KDPoint NCRLayoutNode::positionOfChild(LayoutNode * child) {
  if (child == nLayout()) {
    return KDPoint(0, aboveSymbol() - nLayout()->baseline());
  }
  assert(child == kLayout());
  return KDPoint(nLayout()->layoutSize().width() + k_symbolWidthWithMargins,
                 aboveSymbol() + k_symbolHeight - kLayout()->baseline());
}

void NCRLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  // Render the parentheses.
  uint8_t symbolPixel[k_symbolHeight * k_symbolWidth];
  static_assert(k_symbolHeight%2 == 0, "ncr_layout : k_symbolHeight is odd");

  for (int i = 0; i < k_symbolHeight / 2; i++) {
    for (int j = 0; j < k_symbolWidth; j++) {
      symbolPixel[i * k_symbolWidth + j] = symbolUpperHalf[i * k_symbolWidth + j];
      symbolPixel[(k_symbolHeight - 1 - i) * k_symbolWidth + j] = symbolUpperHalf[i * k_symbolWidth + j];
    }
  }

  KDCoordinate combinationSymbolX = nLayout()->layoutSize().width() + k_margin;
  KDCoordinate combinationSymbolY = aboveSymbol();

  // Render the nCr symbol.
  KDColor workingBuffer[k_symbolWidth * k_symbolHeight];
  KDPoint base = p.translatedBy(KDPoint(combinationSymbolX, combinationSymbolY));
  KDRect symbolFrame(base.x(), base.y(),
                     k_symbolWidth, k_symbolHeight);

  ctx->blendRectWithMask(symbolFrame, expressionColor, (const uint8_t *)symbolPixel, (KDColor *)workingBuffer);

}

}
