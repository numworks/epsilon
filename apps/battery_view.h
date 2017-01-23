#ifndef APPS_BATTERY_VIEW_H
#define APPS_BATTERY_VIEW_H

#include <escher.h>

class BatteryView : public View {
public:
  BatteryView();
  void setChargeState(Ion::Battery::Charge chargeState);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() override;
private:
  constexpr static KDCoordinate k_batteryHeight = 8;
  constexpr static KDCoordinate k_batteryWidth = 13;
  constexpr static KDCoordinate k_elementWidth = 1;
  constexpr static KDCoordinate k_capHeight = 4;
  constexpr static KDCoordinate k_separatorThickness = 1;
  Ion::Battery::Charge m_chargeState;
};

#endif
