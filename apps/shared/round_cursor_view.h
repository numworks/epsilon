#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include "cursor_view.h"

namespace Shared {

#define GRAPH_CURSOR_SPEEDUP 1

class RoundCursorView : public CursorView {
public:
  RoundCursorView(KDColor color = KDColorBlack) : m_color(color), m_underneathPixelBufferLoaded(false) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color);
  void setCursorFrame(KDRect frame) override;
#ifdef GRAPH_CURSOR_SPEEDUP
  void resetMemoization() const { m_underneathPixelBufferLoaded = false; }
#endif
private:
#ifdef GRAPH_CURSOR_SPEEDUP
  bool eraseCursorIfPossible();
#endif
  constexpr static int k_cursorSize = 10;
  mutable KDColor m_underneathPixelBuffer[k_cursorSize*k_cursorSize];
  KDColor m_color;
  mutable bool m_underneathPixelBufferLoaded;
};

}

#endif
