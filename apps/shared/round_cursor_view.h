#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include "cursor_view.h"
#include "dots.h"

namespace Shared {

#define GRAPH_CURSOR_SPEEDUP

class RoundCursorView : public CursorView {
public:
  RoundCursorView(KDColor color = KDColorBlack) :
    m_color(color)
#ifdef GRAPH_CURSOR_SPEEDUP
  , m_underneathPixelBufferLoaded(false)
#endif
  {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void setColor(KDColor color);
  void setCursorFrame(KDRect frame, bool force) override;
#ifdef GRAPH_CURSOR_SPEEDUP
  void resetMemoization() const { m_underneathPixelBufferLoaded = false; }
#endif
protected:
  constexpr static int k_cursorSize = Dots::LargeDotDiameter;
  KDColor m_color;
#ifdef GRAPH_CURSOR_SPEEDUP
  mutable KDColor m_underneathPixelBuffer[k_cursorSize*k_cursorSize];
  mutable bool m_underneathPixelBufferLoaded;
#endif
private:
  void markRectAsDirty(KDRect rect) override;
#ifdef GRAPH_CURSOR_SPEEDUP
  bool eraseCursorIfPossible();
#endif
};

}

#endif
