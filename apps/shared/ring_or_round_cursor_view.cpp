#include "ring_or_round_cursor_view.h"

namespace Shared {

void RingCursorView::drawCursor(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  if (m_isRing) {
    KDColor cursorWorkingBuffer[Dots::LargeRingDiameter*Dots::LargeRingDiameter];
    ctx->blendRectWithMask(r, m_color, (const uint8_t *)Dots::LargeRingMask, cursorWorkingBuffer);
  } else {
    static_assert(Dots::LargeRingDiameter >= Dots::LargeDotDiameter);
    constexpr KDCoordinate diff = Dots::LargeRingDiameter - Dots::LargeDotDiameter;
    KDRect dotRect(r.x() + diff/2, r.y() + diff/2, r.width() - diff, r.height() - diff);
    KDColor cursorWorkingBuffer[Dots::LargeDotDiameter*Dots::LargeDotDiameter];
    ctx->blendRectWithMask(dotRect, m_color, (const uint8_t *)Dots::LargeDotMask, cursorWorkingBuffer);
  }
}

void RingOrRoundCursorView::setIsRing(bool isRing) {
  if (isRing != m_isRing) {
    m_isRing = isRing;
    markRectAsDirty(bounds());
  }
}

}
