#ifndef SHARED_RING_OR_ROUND_CURSOR_VIEW_H
#define SHARED_RING_OR_ROUND_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

/* To avoid code duplication RingCursorView is indeed a RingOrRoundCursorView
 * that can't be set to round */
class RingCursorView : public MemoizedCursorView {
public:
  RingCursorView() : MemoizedCursorView(), m_isRing(true) {}
protected:
  bool m_isRing;
private:
  void drawCursor(KDContext * ctx, KDRect rect) const override;
  KDCoordinate size() const override { return Dots::LargeRingDiameter; }
  KDColor * underneathPixelBuffer() const override { return m_underneathPixelBuffer; }
  mutable KDColor m_underneathPixelBuffer[Dots::LargeRingDiameter * Dots::LargeRingDiameter];
};

class RingOrRoundCursorView : public RingCursorView {
public:
  void setIsRing(bool isRing);
};

}

#endif
