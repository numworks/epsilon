#ifndef SHARED_RING_OR_ROUND_CURSOR_VIEW_H
#define SHARED_RING_OR_ROUND_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

class RingOrRoundCursorView : public MemoizedCursorView<std::max(Dots::LargeRingDiameter, Dots::LargeDotDiameter)> {
public:
  void drawCursor(KDContext * ctx, KDRect rect) const override;
  void setIsRing(bool isRing);
private:
  bool m_isRing;
};

}

#endif
