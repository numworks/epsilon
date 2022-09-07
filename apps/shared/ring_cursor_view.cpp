#include "ring_cursor_view.h"

#include <kandinsky/ion_context.h>

namespace Shared {

void RingCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  KDColor cursorWorkingBuffer[Dots::LargeDotDiameter*Dots::LargeDotDiameter];
#ifdef GRAPH_CURSOR_SPEEDUP
  /* Beware that only the pixels of the intersection of rect with KDContext's
   * clipping rect are pulled. All other pixels are left unaltered. Indeed
   * nothing outside the clipping rect should be redrawn and hence was not
   * dirty.
   */
  ctx->getPixels(r, m_underneathPixelBuffer);
  m_underneathPixelBufferLoaded = true;
#endif
  constexpr KDCoordinate diff = Dots::LargeDotDiameter - Dots::TinyDotDiameter;
  KDRect dotRect(r.x() + diff/2, r.y() + diff/2, r.width() - diff, r.height() - diff);
  ctx->blendRectWithMask(r, m_color, (const uint8_t *)Dots::LargeRingMask, cursorWorkingBuffer);
}

}
