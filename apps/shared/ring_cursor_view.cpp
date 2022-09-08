#include "ring_cursor_view.h"

#include <kandinsky/ion_context.h>

namespace Shared {

void RingCursorView::drawCursor(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  KDColor cursorWorkingBuffer[Dots::LargeRingDiameter*Dots::LargeRingDiameter];
  ctx->blendRectWithMask(r, m_color, (const uint8_t *)Dots::LargeRingMask, cursorWorkingBuffer);
}

}
