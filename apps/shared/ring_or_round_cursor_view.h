#ifndef SHARED_RING_OR_ROUND_CURSOR_VIEW_H
#define SHARED_RING_OR_ROUND_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

class RingOrRoundCursorView : public MemoizedCursorView {
public:
  void setIsRing(bool isRing);
private:
  void drawCursor(KDContext * ctx, KDRect rect) const override;
  KDCoordinate size() const override { return Dots::LargeRingDiameter; }
  KDColor * underneathPixelBuffer() const override { return m_underneathPixelBuffer; }
  mutable KDColor m_underneathPixelBuffer[Dots::LargeRingDiameter * Dots::LargeRingDiameter];
  bool m_isRing;
};

}

#endif
