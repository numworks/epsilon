#pragma once

#include <escher/arbitrary_shaped_view.h>

class LockView : public Escher::ArbitraryShapedView {
 public:
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static int k_lockHeight = 9;
  constexpr static int k_lockWidth = 7;
};
