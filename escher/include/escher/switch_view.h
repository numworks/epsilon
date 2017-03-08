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
private:
  /* k_switchHeight and k_switchWidth are the dimensions of the switch
   * (including the outline of the switch). The outline thickness is
   * k_separatorThickness. The k_switchMargin is the margin between the right
   * extremity of the view and the right extremity of the switch. */
  constexpr static KDCoordinate k_switchHeight = 16;
  constexpr static KDCoordinate k_switchWidth = 20;
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_switchMargin = 10;
  bool m_state;
};

#endif
