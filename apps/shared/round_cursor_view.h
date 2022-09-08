#ifndef SHARED_ROUND_CURSOR_VIEW_H
#define SHARED_ROUND_CURSOR_VIEW_H

#include "memoized_cursor_view.h"
#include "dots.h"

namespace Shared {

class RoundCursorView : public MemoizedCursorView<Dots::LargeDotDiameter> {
public:
  void drawCursor(KDContext * ctx, KDRect rect) const override;
};

}

#endif
