#ifndef SHARED_RING_CURSOR_VIEW_H
#define SHARED_RING_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

class RingCursorView : public MemoizedCursorView<Dots::LargeRingDiameter> {
public:
  void drawCursor(KDContext * ctx, KDRect rect) const override;
};

}

#endif
