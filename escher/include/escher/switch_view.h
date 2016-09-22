#ifndef ESCHER_SWITCH_VIEW_H
#define ESCHER_SWITCH_VIEW_H

#include <escher/childless_view.h>

class SwitchView : public ChildlessView {
public:
  SwitchView();
  bool state();
  void setState(bool state);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_switchHeight = 10;
  constexpr static KDCoordinate k_switchWidth = 20;
  constexpr static KDCoordinate k_separatorThickness = 1;
  bool m_state;
};

#endif
