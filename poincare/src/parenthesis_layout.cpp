#include <poincare/parenthesis_layout.h>

namespace Poincare {

constexpr static uint8_t topLeftCurve[ParenthesisLayoutNode::k_curveHeight]
                                     [ParenthesisLayoutNode::k_curveWidth] = {
                                         {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
                                         {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
                                         {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
                                         {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
                                         {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
                                         {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
                                         {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
};

constexpr static uint8_t bottomLeftCurve[ParenthesisLayoutNode::k_curveHeight]
                                        [ParenthesisLayoutNode::k_curveWidth] =
                                            {
                                                {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
                                                {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
                                                {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
                                                {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
                                                {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
                                                {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
                                                {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
};

constexpr static uint8_t topRightCurve[ParenthesisLayoutNode::k_curveHeight]
                                      [ParenthesisLayoutNode::k_curveWidth] = {
                                          {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
                                          {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
                                          {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
                                          {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
                                          {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
                                          {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
                                          {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

constexpr static uint8_t bottomRightCurve[ParenthesisLayoutNode::k_curveHeight]
                                         [ParenthesisLayoutNode::k_curveWidth] =
                                             {
                                                 {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
                                                 {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
                                                 {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
                                                 {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
                                                 {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
                                                 {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
                                                 {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

void ParenthesisLayoutNode::RenderWithChildHeight(bool left,
                                                  KDCoordinate childHeight,
                                                  KDContext *ctx, KDPoint p,
                                                  KDColor expressionColor,
                                                  KDColor backgroundColor) {
  KDColor parenthesisWorkingBuffer[k_curveHeight * k_curveWidth];
  KDCoordinate parenthesisHeight = HeightGivenChildHeight(childHeight);

  KDRect frame(k_widthMargin, k_verticalMargin, k_curveWidth, k_curveHeight);
  ctx->blendRectWithMask(frame.translatedBy(p), expressionColor,
                         (const uint8_t *)(left ? topLeftCurve : topRightCurve),
                         parenthesisWorkingBuffer);

  frame = KDRect(k_widthMargin,
                 parenthesisHeight - k_curveHeight - k_verticalMargin,
                 k_curveWidth, k_curveHeight);
  ctx->blendRectWithMask(
      frame.translatedBy(p), expressionColor,
      (const uint8_t *)(left ? bottomLeftCurve : bottomRightCurve),
      parenthesisWorkingBuffer);

  KDCoordinate barX =
      k_widthMargin + (left ? 0 : k_curveWidth - k_lineThickness);
  KDCoordinate barHeight =
      parenthesisHeight - 2 * (k_curveHeight + k_verticalMargin);
  ctx->fillRect(
      KDRect(barX, k_curveHeight + k_verticalMargin, k_lineThickness, barHeight)
          .translatedBy(p),
      expressionColor);
}

}  // namespace Poincare
