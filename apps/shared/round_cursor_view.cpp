#include "round_cursor_view.h"

namespace Shared {

static constexpr KDCoordinate cursorSize = 10;
static const uint8_t cursorMask[cursorSize][cursorSize] = {
  {0xFF, 0xFF, 0xFF, 0xED, 0xB6, 0xB6, 0xED, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xFF, 0x7C, 0x06, 0x00, 0x00, 0x06, 0x7C, 0xFF, 0xFF},
  {0xFF, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFF},
  {0xED, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE5},
  {0xB6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB6},
  {0xB6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB6},
  {0xED, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE5},
  {0xFF, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xFF},
  {0xFF, 0xFF, 0x7C, 0x06, 0x00, 0x00, 0x06, 0x7C, 0xFF, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xED, 0xB6, 0xB6, 0xED, 0xFF, 0xFF, 0xFF},
};

static KDColor s_cursorWorkingBuffer[cursorSize*cursorSize];

void RoundCursorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect r = bounds();
  ctx->getPixels(r, m_underneathPixelBuffer);
  m_underneathPixelBufferLoaded = true;
  ctx->blendRectWithMask(r, m_color, (const uint8_t *)cursorMask, s_cursorWorkingBuffer);
}

KDSize RoundCursorView::minimalSizeForOptimalDisplay() const {
  return KDSize(cursorSize, cursorSize);
}

void RoundCursorView::setColor(KDColor color) {
  m_color = color;
  markRectAsDirty(bounds());
}

#define GRAPH_CURSOR_SPEEDUP 1

void RoundCursorView::setCursorFrame(KDRect f) {
#if GRAPH_CURSOR_SPEEDUP
  /* TODO This is quite dirty (we are out of the dirty tracking and we assume
   * the cursor is the upmost view) but it works well. */
  if (m_frame == f) {
    return;
  }
  const KDRect previousRelativeFrame = m_frame;
  /* We want to avoid drawing the curve just because the cursor has been
   * repositioned, as it is very slow for non cartesian curves.*/
  if (m_underneathPixelBufferLoaded && !previousRelativeFrame.isEmpty()) {
    const KDRect previousFrame = absoluteVisibleFrame();
    // Erase the cursor
    KDContext * ctx = KDIonContext::sharedContext();
    ctx->setOrigin(previousFrame.origin());
    ctx->setClippingRect(previousFrame);
    ctx->fillRectWithPixels(KDRect(0,0,k_cursorSize, k_cursorSize), m_underneathPixelBuffer, s_cursorWorkingBuffer);
    // Set the frame
    m_frame = f;
    markRectAsDirty(bounds());
    return;
  }
#endif
  CursorView::setCursorFrame(f);
}

}
