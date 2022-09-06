#ifndef SHARED_RING_CURSOR_VIEW_H
#define SHARED_RING_CURSOR_VIEW_H

#include "round_cursor_view.h"
#include <escher/palette.h>

namespace Shared {

class RingCursorView : public RoundCursorView {
public:
  RingCursorView() : RoundCursorView(Escher::Palette::YellowDark) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
