#ifndef ESCHER_SWITCH_VIEW_H
#define ESCHER_SWITCH_VIEW_H

#include <escher/view.h>

class SwitchView : public View {
public:
  SwitchView();
  bool state();
  void setState(bool state);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  /* k_switchHeight and k_switchWidth are the dimensions of the switch
   * (including the outline of the switch). */
  constexpr static KDCoordinate k_onOffSize = 12;
  constexpr static KDCoordinate k_switchHeight = 12;
  constexpr static KDCoordinate k_switchWidth = 22;
private:
  bool m_state;
};

#endif
