#include "round_cursor_view.h"

#include <kandinsky/ion_context.h>

namespace Shared {

void RoundCursorView::drawCursor(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  KDColor cursorWorkingBuffer[Dots::LargeDotDiameter*Dots::LargeDotDiameter];
  ctx->blendRectWithMask(r, m_color, (const uint8_t *)Dots::LargeDotMask, cursorWorkingBuffer);
}

}
