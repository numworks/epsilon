#include <escher/toggleable_dot_view.h>
#include <escher/palette.h>
#include <kandinsky/dot.h>

namespace Escher {

static constexpr KDCoordinate Size = ToggleableDotView::k_dotSize;

static constexpr const KDMask<Size> mediumDot = KDMakeDot<Size>(ToggleableDotView::k_dotRadius);
const uint8_t (&MediumDotMask)[Size][Size] = mediumDot.m_mask;

static constexpr const KDMask<Size> mediumShallowDot = KDMakeRing<Size>(ToggleableDotView::k_dotRadius - ToggleableDotView::k_shallowDotThickness, ToggleableDotView::k_dotRadius);
const uint8_t (&MediumShallowDotMask)[Size][Size] = mediumShallowDot.m_mask;

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
