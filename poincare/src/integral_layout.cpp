#include <poincare/integral_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/integral.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

const uint8_t topSymbolPixel[IntegralLayoutNode::k_symbolHeight][IntegralLayoutNode::k_symbolWidth] = {
  {0xFF, 0xD5, 0x46, 0x09},
  {0xF1, 0x1A, 0x93, 0xF0},
  {0x98, 0x54, 0xFF, 0xFF},
  {0x53, 0xA7, 0xFF, 0xFF},
  {0x29, 0xCF, 0xFF, 0xFF},
  {0x14, 0xEA, 0xFF, 0xFF},
  {0x02, 0xFC, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF},
  {0x00, 0xFF, 0xFF, 0xFF},
};

const uint8_t bottomSymbolPixel[IntegralLayoutNode::k_symbolHeight][IntegralLayoutNode::k_symbolWidth] = {
  {0xFF, 0xFF, 0xFF, 0x00},
  {0xFF, 0xFF, 0xFF, 0x00},
  {0xFF, 0xFF, 0xFE, 0x03},
  {0xFF, 0xFF, 0xEA, 0x13},
  {0xFF, 0xFF, 0xCF, 0x29},
  {0xFF, 0xFF, 0xA5, 0x53},
  {0xFF, 0xFF, 0x54, 0x99},
  {0xF2, 0x95, 0x1A, 0xF1},
  {0x09, 0x46, 0xD5, 0xFF},
};

int IntegralLayoutNode::indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex) const {
  switch (currentIndex) {
  case k_outsideIndex:
    return direction == OMG::HorizontalDirection::Right ? k_upperBoundLayoutIndex : k_differentialLayoutIndex;
  case k_upperBoundLayoutIndex:
  case k_lowerBoundLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_integrandLayoutIndex : k_outsideIndex;
  case k_integrandLayoutIndex:
    return direction == OMG::HorizontalDirection::Right ? k_differentialLayoutIndex : k_lowerBoundLayoutIndex;
  default:
    assert(currentIndex == k_differentialLayoutIndex);
    return direction == OMG::HorizontalDirection::Right ? k_outsideIndex : k_integrandLayoutIndex;
  }
}

int IntegralLayoutNode::indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex) const {
  if (currentIndex == k_integrandLayoutIndex && positionAtCurrentIndex == PositionInLayout::Left) {
    return direction == OMG::VerticalDirection::Up ? k_upperBoundLayoutIndex : k_lowerBoundLayoutIndex;
  }
  if (currentIndex == k_upperBoundLayoutIndex && direction == OMG::VerticalDirection::Down) {
    return k_lowerBoundLayoutIndex;
  }
  if (currentIndex == k_lowerBoundLayoutIndex && direction == OMG::VerticalDirection::Up) {
    return k_upperBoundLayoutIndex;
  }
  return k_outsideIndex;
}

/*
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
  if (!deleteBeforeCursorForLayoutContainingArgument(integrandLayout(), cursor)) {
    LayoutNode::deleteBeforeCursor(cursor);
  }
}
*/
int IntegralLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Integral::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::System);
}

Layout IntegralLayoutNode::XNTLayout(int childIndex) const {
  if (childIndex == k_integrandLayoutIndex) {
    return Layout(childAtIndex(k_differentialLayoutIndex)).clone();
  }
  if (childIndex == k_differentialLayoutIndex) {
    return CodePointLayout::Builder(CodePoint(Integral::k_defaultXNTChar));
  }
  return LayoutNode::XNTLayout();
}

// Return pointer to the first or the last integral from left to right (considering multiple integrals in a row)
IntegralLayoutNode * IntegralLayoutNode::mostNestedIntegral(NestedPosition position) {
  IntegralLayoutNode * p = this;
  IntegralLayoutNode * integral = p->nestedIntegral(position);
  while (integral != nullptr) {
    p = integral;
    integral = integral->nestedIntegral(position);
  }
  return p;
}

// Return pointer to the integral immediately on the right. If none is found, return nullptr
IntegralLayoutNode * IntegralLayoutNode::nextNestedIntegral() {
  LayoutNode * integrand = integrandLayout();
  if (integrand->type() == Type::IntegralLayout) {
    // Integral can be directly in the integrand
    return static_cast<IntegralLayoutNode *>(integrand);
  } else if (integrand->isHorizontal() && integrand->numberOfChildren() == 1 && integrand->childAtIndex(0)->type() == Type::IntegralLayout) {
    // Or can be in a Horizontal layout that only contains an integral
    integrand = integrand->childAtIndex(0);
    return static_cast<IntegralLayoutNode *>(integrand);
  }
  return nullptr;
}

// Return pointer to the integral immediately on the left. If none is found, return nullptr
IntegralLayoutNode * IntegralLayoutNode::previousNestedIntegral() {
  assert(type() == Type::IntegralLayout);
  LayoutNode * p = parent();
  if (p == nullptr) {
    return nullptr;
  }
  if (p->type() == Type::IntegralLayout) {
    // Parent is an integral. Checking if the child is its integrand or not
    if (p->childAtIndex(0) == this) {
      return static_cast<IntegralLayoutNode *>(p);
    } else {
      // If this is not parent's integrand, it means that it is either a bound or differential
      return nullptr;
    }
  } else if (p->isHorizontal()) {
    // Or can be a Horizontal layout himself contained in an integral
    LayoutNode * prev = p->parent();
    if (prev == nullptr) {
      return nullptr;
    }
    if (p->numberOfChildren() == 1 && prev->type() == Type::IntegralLayout) {
      // We can consider the integrals in a row only if the horizontal layout just contains an integral
      // The horizontal layout must be the previous integral's integrand
      if (prev->childAtIndex(0) == p) {
        return static_cast<IntegralLayoutNode *>(prev);
      }
    }
  }
  return nullptr;
}

/* Return the height of the tallest upper/lower bound amongst a row of integrals
 * If the integral is alone, will return its upper/lower bound height*/
KDCoordinate IntegralLayoutNode::boundMaxHeight(BoundPosition position, KDFont::Size font) {
  IntegralLayoutNode * p = mostNestedIntegral(NestedPosition::Next);
  LayoutNode * bound = p->boundLayout(position);
  KDCoordinate max = bound->layoutSize(font).height();
  IntegralLayoutNode * first = mostNestedIntegral(NestedPosition::Previous);
  while (p != first) {
    p = p->previousNestedIntegral();
    bound = p->boundLayout(position);
    max = std::max(max, bound->layoutSize(font).height());
  }
  return max;
}

// clang-format off
/*
 * Window configuration explained :
 * Vertical margins and offsets
 * +-----------------------------------------------------------------+
 * |                                                |                |
 * |                                        k_boundVerticalMargin    |
 * |                                                |                |
 * |                                       +------------------+      |
 * |                                       | upperBoundHeight |      |
 * |                                       +------------------+      |
 * |             +++       |                        |                |
 * |            +++   k_symbolHeight     k_integrandVerticalMargin   |
 * |           +++         |                        |                |
 * |                                                                 |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||         centralArgumentHeight                     |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |           |||                                                   |
 * |                                                                 |
 * |           +++         |                      |                  |
 * |          +++    k_symbolHeight     k_integrandVerticalMargin    |
 * |         +++           |                      |                  |
 * |                                     +------------------+        |
 * |                                     | lowerBoundHeight |        |
 * |                                     +------------------+        |
 * |                                              |                  |
 * |                                      k_boundVerticalMargin      |
 * |                                              |                  |
 * +-----------------------------------------------------------------+
 *
 * Horizontal margins and offsets
 * +-------------------------------------------------------------------------------------------------------+
 * |                                                                                                       |                                                                                                |
 * |           |             |                       |+---------------+|                           |       |
 * |           |k_symbolWidth|k_boundHorizontalMargin||upperBoundWidth||k_integrandHorizontalMargin|       |
 * |           |             |                       |+---------------+|                           |       |
 * |                      ***                                                                              |
 * |                    ***                                                                                |
 * |                  ***                                                                          |       |
 * |                ***                                                                                    |
 * |              ***                                                                                      |
 * |            ***                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                 x dx   |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            |||                                                                                |       |
 * |            |||                                                                                        |
 * |            |||                                                                                        |
 * |            ***                                                                                |       |
 * |          ***                                                                                          |
 * |        ***                                                                                            |
 * |      ***                                                                                      |       |
 * |    ***                                                                                                |
 * |  ***                                                                                                  |
 * | |             |         |                       |+---------------+|                           |       |
 * | |k_symbolWidth|    a    |k_boundHorizontalMargin||lowerBoundWidth||k_integrandHorizontalMargin|       |
 * | |             |         |                       |+---------------+|                           |       |
 * |                                                                                                       |
 * +-------------------------------------------------------------------------------------------------------+
 * ||| = k_lineThickness
 * a = k_symbolWidth - k_lineThickness
 */
// clang-format on

KDSize IntegralLayoutNode::computeSize(KDFont::Size font) {
  KDSize dSize = KDFont::Font(font)->stringSize("d");
  KDSize integrandSize = integrandLayout()->layoutSize(font);
  KDSize differentialSize = differentialLayout()->layoutSize(font);
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize(font);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDCoordinate width = k_symbolWidth+k_lineThickness+k_boundHorizontalMargin+std::max(lowerBoundSize.width(), upperBoundSize.width())+k_integrandHorizontalMargin+integrandSize.width()+k_differentialHorizontalMargin+dSize.width()+k_differentialHorizontalMargin+differentialSize.width();
  IntegralLayoutNode * last = mostNestedIntegral(NestedPosition::Next);
  KDCoordinate height;
  if (this == last) {
    height = k_boundVerticalMargin + boundMaxHeight(BoundPosition::UpperBound, font) + k_integrandVerticalMargin + centralArgumentHeight(font) + k_integrandVerticalMargin + boundMaxHeight(BoundPosition::LowerBound, font) + k_boundVerticalMargin;
  } else {
    height = last->layoutSize(font).height();
  }
  return KDSize(width, height);
}

KDCoordinate IntegralLayoutNode::computeBaseline(KDFont::Size font) {
  IntegralLayoutNode * last = mostNestedIntegral(NestedPosition::Next);
  if (this == last) {
    return k_boundVerticalMargin + boundMaxHeight(BoundPosition::UpperBound, font) + k_integrandVerticalMargin + std::max(integrandLayout()->baseline(font), differentialLayout()->baseline(font));
  } else {
    // If integrals are in a row, they must have the same baseline. Since the last integral has the lowest, we take this one for all the others
    return last->baseline(font);
  }
}

KDPoint IntegralLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  KDSize lowerBoundSize = lowerBoundLayout()->layoutSize(font);
  KDSize upperBoundSize = upperBoundLayout()->layoutSize(font);
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  KDCoordinate boundOffset = 2*k_symbolWidth - k_lineThickness + k_boundHorizontalMargin;
  if (child == lowerBoundLayout()) {
    x = boundOffset;
    y = computeSize(font).height() - k_boundVerticalMargin - boundMaxHeight(BoundPosition::LowerBound, font);
  } else if (child == upperBoundLayout()) {
    x = boundOffset;
    y = k_boundVerticalMargin + boundMaxHeight(BoundPosition::UpperBound, font) - upperBoundSize.height();
  } else if (child == integrandLayout()) {
    x = boundOffset + std::max(lowerBoundSize.width(), upperBoundSize.width()) + k_integrandHorizontalMargin;
    y = computeBaseline(font)-integrandLayout()->baseline(font);
  } else {
    assert(child == differentialLayout());
    x = computeSize(font).width() - differentialLayout()->layoutSize(font).width();
    y = computeBaseline(font)-differentialLayout()->baseline(font);
  }
  return KDPoint(x,y);
}

KDCoordinate IntegralLayoutNode::centralArgumentHeight(KDFont::Size font) {
  // When integrals are in a row, the last one is the tallest. We take its central argument height to define the one of the others integrals
  IntegralLayoutNode * last = mostNestedIntegral(NestedPosition::Next);
  if (this == last) {
    KDCoordinate integrandHeight = integrandLayout()->layoutSize(font).height();
    KDCoordinate integrandBaseline = integrandLayout()->baseline(font);
    KDCoordinate differentialHeight = differentialLayout()->layoutSize(font).height();
    KDCoordinate differentialBaseline = differentialLayout()->baseline(font);
    return std::max(integrandBaseline, differentialBaseline) + std::max(integrandHeight-integrandBaseline, differentialHeight - differentialBaseline);
  } else {
    return last->centralArgumentHeight(font);
  }
}

void IntegralLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  KDSize integrandSize = integrandLayout()->layoutSize(font);
  KDCoordinate centralArgHeight = centralArgumentHeight(font);
  KDColor workingBuffer[k_symbolWidth*k_symbolHeight];


  // Render the integral symbol
  KDCoordinate offsetX = p.x() + k_symbolWidth;
  KDCoordinate offsetY = p.y() + k_boundVerticalMargin + boundMaxHeight(BoundPosition::UpperBound, font) + k_integrandVerticalMargin - k_symbolHeight;

  // Upper part
  KDRect topSymbolFrame(offsetX, offsetY, k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(topSymbolFrame, expressionColor, (const uint8_t *)topSymbolPixel, (KDColor *)workingBuffer);

  // Central bar
  offsetY = offsetY + k_symbolHeight;
  ctx->fillRect(KDRect(offsetX, offsetY, k_lineThickness, centralArgHeight), expressionColor);

  // Lower part
  offsetX = offsetX - k_symbolWidth + k_lineThickness;
  offsetY = offsetY + centralArgHeight;
  KDRect bottomSymbolFrame(offsetX,offsetY,k_symbolWidth, k_symbolHeight);
  ctx->blendRectWithMask(bottomSymbolFrame, expressionColor, (const uint8_t *)bottomSymbolPixel, (KDColor *)workingBuffer);

  // Render "d"
  KDPoint dPosition = p.translatedBy(positionOfChild(integrandLayout(), font)).translatedBy(KDPoint(integrandSize.width() + k_differentialHorizontalMargin, integrandLayout()->baseline(font) - KDFont::GlyphHeight(font)/2));
  ctx->drawString("d", dPosition, font, expressionColor, backgroundColor);
}

}
