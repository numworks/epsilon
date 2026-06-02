#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class BatteryTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  bool runTest() override;

 private:
  const char* testName() const override { return "BATTERY:"; }

  void updateBatteryState(float batteryLevel, bool batteryCharging);
};

}  // namespace HardwareTest
