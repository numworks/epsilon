#ifndef SHARED_VERTICAL_CURSOR_VIEW_H
#define SHARED_VERTICAL_CURSOR_VIEW_H

#include "cursor_view.h"

namespace Shared {

class VerticalCursorView : public CursorView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
};

}  // namespace Shared

#endif
