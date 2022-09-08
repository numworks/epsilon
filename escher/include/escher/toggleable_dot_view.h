#ifndef ESCHER_TOGGLEABLE_DOT_VIEW_H
#define ESCHER_TOGGLEABLE_DOT_VIEW_H

#include <escher/toggleable_view.h>

namespace Escher {

class ToggleableDotView final : public ToggleableView {
public:
  using ToggleableView::ToggleableView;
  /* k_dotSize is the dimensions of the toggle dot */
  constexpr static KDCoordinate k_dotSize = 9;
  constexpr static float k_dotRadius = 4.485;
  constexpr static float k_shallowDotThickness = 1.0;
  KDSize minimalSizeForOptimalDisplay() const override { return KDSize(k_dotSize, k_dotSize); }
private:
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}
#endif
