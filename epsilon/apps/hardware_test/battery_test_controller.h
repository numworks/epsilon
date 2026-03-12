#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class BatteryTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  constexpr static const char* k_needRecharge = "NEED RECHARGE";
  constexpr static int k_maxNumberOfCharacters = 20;

  bool runTest() override;
  const char* testName() const override { return "BATTERY:"; }

  void updateBatteryState(float batteryLevel, bool batteryCharging);
};

}  // namespace HardwareTest
