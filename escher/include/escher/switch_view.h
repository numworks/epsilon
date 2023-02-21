#ifndef ESCHER_SWITCH_VIEW_H
#define ESCHER_SWITCH_VIEW_H

#include <escher/palette.h>
#include <escher/toggleable_view.h>

namespace Escher {

class SwitchView final : public ToggleableView {
 public:
  using ToggleableView::ToggleableView;
  /* k_switchHeight and k_switchWidth are the dimensions of the switch
   * (including the outline of the switch). */
  constexpr static KDCoordinate k_onOffSize = 12;
  constexpr static KDCoordinate k_switchHeight = 12;
  constexpr static KDCoordinate k_switchWidth = 22;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(k_switchWidth, k_switchHeight);
  }

 private:
  void drawRect(KDContext* ctx, KDRect rect) const override;
};

}  // namespace Escher
#endif
