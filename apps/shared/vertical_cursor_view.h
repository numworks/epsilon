#ifndef SHARED_VERTICAL_CURSOR_VIEW_H
#define SHARED_VERTICAL_CURSOR_VIEW_H

#include <escher.h>

namespace Shared {

class VerticalCursorView : public View {
public:
  using View::View;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
};

}

#endif
