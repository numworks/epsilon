#include <escher/toggleable_dot_view.h>
#include <escher/palette.h>

namespace Escher {

const uint8_t MediumDotMask[ToggleableDotView::k_dotSize][ToggleableDotView::k_dotSize] = {
  {0xff, 0xf6, 0x7e, 0x23, 0x06, 0x23, 0x7e, 0xf6, 0xff},
  {0xf5, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xf5},
  {0x7d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f},
  {0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
  {0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06},
  {0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21},
  {0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e},
  {0xf5, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xf5},
  {0xff, 0xf6, 0x7e, 0x23, 0x06, 0x23, 0x7e, 0xf6, 0xff}
};

const uint8_t MediumShallowDotMask[ToggleableDotView::k_dotSize][ToggleableDotView::k_dotSize] = {
  {0xff, 0xf6, 0x7e, 0x23, 0x06, 0x23, 0x7e, 0xf6, 0xff},
  {0xf5, 0x3a, 0x55, 0xd4, 0xf9, 0xd4, 0x56, 0x3a, 0xf5},
  {0x7d, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0x55, 0x7f},
  {0x23, 0xd3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd4, 0x22},
  {0x06, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x06},
  {0x23, 0xd4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd4, 0x21},
  {0x7c, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0x56, 0x7e},
  {0xf5, 0x3a, 0x57, 0xd6, 0xfa, 0xd6, 0x57, 0x3a, 0xf5},
  {0xff, 0xf6, 0x7e, 0x23, 0x06, 0x23, 0x7e, 0xf6, 0xff}
};

void ToggleableDotView::drawRect(KDContext * ctx, KDRect rect) const {
  /* Draw the view aligned on the right of the view and vertically centered
   * The heightCenter is the coordinate of the vertical middle of the view. That
   * way, (heightCenter-halfHeight) indicates the top of the StateView. */
  KDCoordinate width = bounds().width();
  KDCoordinate heightCenter =  bounds().height() / 2;
  KDCoordinate halfHeight = k_dotSize / 2;
  KDColor workingBuffer[k_dotSize * k_dotSize];

  KDRect frame(width - k_dotSize, heightCenter - halfHeight, k_dotSize, k_dotSize);
  ctx->blendRectWithMask(
    frame,
    m_state ? Palette::YellowDark : Palette::GrayDark,
    m_state ? reinterpret_cast<const uint8_t *>(MediumDotMask) : reinterpret_cast<const uint8_t *>(MediumShallowDotMask),
    workingBuffer);
}

}
