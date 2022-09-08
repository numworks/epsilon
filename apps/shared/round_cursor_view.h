#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

class RoundCursorView : public MemoizedCursorView {
  void drawCursor(KDContext * ctx, KDRect rect) const override;
  KDCoordinate size() const override { return Dots::LargeDotDiameter; }
  KDColor * underneathPixelBuffer() const override { return m_underneathPixelBuffer; }
  mutable KDColor m_underneathPixelBuffer[Dots::LargeDotDiameter * Dots::LargeDotDiameter];
};

}

#endif
