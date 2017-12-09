#ifndef APPS_LOCK_VIEW_H
#define APPS_LOCK_VIEW_H

#include <escher.h>

class LockView final : public TransparentView {
public:
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(k_lockWidth, k_lockHeight);
  }
  constexpr static int k_lockHeight = 9;
  constexpr static int k_lockWidth = 7;
};

#endif
