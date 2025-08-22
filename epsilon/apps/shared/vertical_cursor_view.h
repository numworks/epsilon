#pragma once

#include "cursor_view.h"

namespace Shared {

class VerticalCursorView : public CursorView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
};

}  // namespace Shared
