#include "round_cursor_view.h"

#include <kandinsky/ion_context.h>

namespace Shared {

void AbstractRoundCursorView::setIsRing(bool isRing) {
  if (isRing != m_isRing) {
    m_isRing = isRing;
    markRectAsDirty(bounds());
  }
}

void AbstractRoundCursorView::drawCursor(KDContext * ctx, KDRect rect) const {
  KDColor cursorWorkingBuffer[k_maxSize * k_maxSize];
  KDCoordinate diff = size() - Dots::LargeDotDiameter;
  KDRect r = bounds();
  KDRect dotRect = m_isRing ? r : KDRect(r.x() + diff/2, r.y() + diff/2, r.width() - diff, r.height() - diff);
  const uint8_t * mask = m_isRing ? Dots::LargeRingMask : Dots::LargeDotMask;
  ctx->blendRectWithMask(dotRect, m_color, mask, cursorWorkingBuffer);
}

}
