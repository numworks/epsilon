#include "round_cursor_view.h"

#include <ion/display.h>

namespace Shared {

void AbstractRoundCursorView::drawCursor(KDContext* ctx, KDRect rect) const {
  KDColor cursorWorkingBuffer[k_maxSize * k_maxSize];
  /* The bounding rect may be larger than the mask to be stamped. We thus need
   * to compute a smaller, centered rect with the same width as the mask for the
   * blend to be correct. */
  KDCoordinate diff = size() - Dots::LargeDotDiameter;
  KDRect r = bounds();
  KDRect dotRect = m_isRing ? r
                            : KDRect(r.x() + diff / 2, r.y() + diff / 2,
                                     r.width() - diff, r.height() - diff);
  const uint8_t* mask = Dots::Mask(Dots::Size::Large, m_isRing);
  ctx->blendRectWithMask(dotRect, color(), mask, cursorWorkingBuffer);
}

void ToggleableRingRoundCursorView::setIsRing(bool isRing) {
  if (isRing != m_isRing) {
    m_isRing = isRing;
    markWholeFrameAsDirty();
  }
}

}  // namespace Shared
