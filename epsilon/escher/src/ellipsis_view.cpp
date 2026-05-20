#include <escher/ellipsis_view.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>
#include <kandinsky/color.h>

namespace Escher {

const uint8_t ellipsisMask[EllipsisView::k_ellipsisHeight]
                          [EllipsisView::k_ellipsisWidth] = {
                              {0xFF, 0xDA, 0xBB, 0xD1, 0xFF, 0xFF, 0xFF, 0xDA,
                               0xBB, 0xD1, 0xFF, 0xFF, 0xFF, 0xDA, 0xBB, 0xD1,
                               0xFF},
                              {0xFF, 0xBA, 0x8C, 0xA1, 0xFF, 0xFF, 0xFF, 0xBA,
                               0x8C, 0xA1, 0xFF, 0xFF, 0xFF, 0xBA, 0x8C, 0xA1,
                               0xFF},
                              {0xFF, 0xD1, 0xA1, 0xC1, 0xFF, 0xFF, 0xFF, 0xD1,
                               0xA1, 0xC1, 0xFF, 0xFF, 0xFF, 0xD1, 0xA1, 0xC1,
                               0xFF},
};

/* Same shape as ellipsisMask but with ~50% lower alpha values so the
 * dots appear more vivid against dark backgrounds (e.g. dark mode). */
const uint8_t ellipsisMaskLowBlend[EllipsisView::k_ellipsisHeight]
                                  [EllipsisView::k_ellipsisWidth] = {
                                      {0xFF, 0x6D, 0x5E, 0x69, 0xFF, 0xFF, 0xFF,
                                       0x6D, 0x5E, 0x69, 0xFF, 0xFF, 0xFF, 0x6D,
                                       0x5E, 0x69, 0xFF},
                                      {0xFF, 0x5D, 0x46, 0x51, 0xFF, 0xFF, 0xFF,
                                       0x5D, 0x46, 0x51, 0xFF, 0xFF, 0xFF, 0x5D,
                                       0x46, 0x51, 0xFF},
                                      {0xFF, 0x69, 0x51, 0x61, 0xFF, 0xFF, 0xFF,
                                       0x69, 0x51, 0x61, 0xFF, 0xFF, 0xFF, 0x69,
                                       0x51, 0x61, 0xFF},
};

void EllipsisView::drawRect(KDContext* ctx, KDRect rect) const {
  /* Draw the ellipsis vertically and horizontally centered in the view.
   * The heightCenter is the coordinate of the vertical middle of the view. That
   * way, (heightCenter-switchHalfHeight) indicates the top the ellipsis. */
  KDCoordinate widthCenter = bounds().width() / 2;
  KDCoordinate ellipsisHalfWidth = k_ellipsisWidth / 2;
  KDCoordinate heightCenter = bounds().height() / 2;
  KDCoordinate ellipsisHalfHeight = k_ellipsisHeight / 2;
  KDRect frame(widthCenter - ellipsisHalfWidth,
               heightCenter - ellipsisHalfHeight, k_ellipsisWidth,
               k_ellipsisHeight);
  KDColor ellipsisWorkingBuffer[EllipsisView::k_ellipsisWidth *
                                EllipsisView::k_ellipsisHeight];
  /* TODO: in dark mode, blending makes the ellipsis very hard to see (violet
   * deep blended with a black background). We should reconsider blending in
   * light mode. Instead of blending black with a white / WallScreen background,
   * we could use a fixed TokenGrayDark or similar component. Alternatively, we
   * could have a theme-specific behavior here (blending in light mode but not
   * in dark mode), but it would be less generic if we add custom themes. */
  ctx->blendRectWithMask(
      frame, ColorPicker::Resolve(Highlight(k_color, m_isHighlighted)),
      (const uint8_t*)ellipsisMask, ellipsisWorkingBuffer);
}

KDSize EllipsisView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_ellipsisWidth, k_ellipsisHeight);
}

}  // namespace Escher
