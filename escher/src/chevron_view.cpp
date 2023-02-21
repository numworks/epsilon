#include <escher/chevron_view.h>
#include <escher/palette.h>

namespace Escher {

const uint8_t chevronMask[ChevronView::k_chevronHeight]
                         [ChevronView::k_chevronWidth] = {
                             {0x0C, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0x00, 0x00, 0x45, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0x00, 0x00, 0x00, 0x0C, 0xE1, 0xFF, 0xFF, 0xFF},
                             {0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xFF, 0xFF},
                             {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x45},
                             {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x45},
                             {0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xFF, 0xFF},
                             {0x00, 0x00, 0x00, 0x0C, 0xE1, 0xFF, 0xFF, 0xFF},
                             {0x00, 0x00, 0x45, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0x0C, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};

void ChevronView::drawRect(KDContext *ctx, KDRect rect) const {
  /* Draw the chevron aligned on the right of the view and vertically centered.
   * The heightCenter is the coordinate of the vertical middle of the view. That
   * way, (heightCenter-switchHalfHeight) indicates the top the chevron. */
  KDCoordinate width = bounds().width();
  KDCoordinate heightCenter = bounds().height() / 2;
  KDCoordinate chevronHalfHeight = k_chevronHeight / 2;
  KDRect frame(width - k_chevronWidth, heightCenter - chevronHalfHeight,
               k_chevronWidth, k_chevronHeight);
  KDColor
      workingBuffer[ChevronView::k_chevronWidth * ChevronView::k_chevronHeight];
  ctx->blendRectWithMask(frame, Palette::YellowDark,
                         (const uint8_t *)chevronMask, workingBuffer);
}

KDSize ChevronView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_chevronWidth, k_chevronHeight);
}

}  // namespace Escher