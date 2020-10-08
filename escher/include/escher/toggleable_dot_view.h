#ifndef ESCHER_TOGGLEABLE_DOT_VIEW_H
#define ESCHER_TOGGLEABLE_DOT_VIEW_H

#include <escher/toggleable_view.h>

class ToggleableDotView final : public ToggleableView {
public:
  using ToggleableView::ToggleableView;
  /* k_dotSize is the dimensions of the toggle dot */
  constexpr static KDCoordinate k_dotSize = 8;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_dotSize, k_dotSize); }
private:
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

#endif