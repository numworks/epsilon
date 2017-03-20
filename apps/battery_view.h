#ifndef APPS_BATTERY_VIEW_H
#define APPS_BATTERY_VIEW_H

#include <escher.h>

class BatteryView : public View {
public:
  BatteryView();
  void setChargeState(Ion::Battery::Charge chargeState);
  void setIsCharging(bool isCharging);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  constexpr static int k_flashHeight = 8;
  constexpr static int k_flashWidth = 4;
private:
  constexpr static KDCoordinate k_batteryHeight = 8;
  constexpr static KDCoordinate k_batteryWidth = 15;
  constexpr static KDCoordinate k_elementWidth = 1;
  constexpr static KDCoordinate k_capHeight = 4;
  constexpr static KDCoordinate k_separatorThickness = 1;
  Ion::Battery::Charge m_chargeState;
  bool m_isCharging;
};

#endif
