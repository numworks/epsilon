#include "battery_test_controller.h"

#include <ion/battery.h>
#include <ion/post_and_hardware_tests.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/print_float.h>

namespace HardwareTest {

bool BatteryTestController::handleEvent(Ion::Events::Event event) {
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  return FocusedViewController::handleEvent(event);
}

bool BatteryTestController::runTest() {
  return Ion::POSTAndHardwareTests::BatteryOK();
}

void BatteryTestController::viewWillAppear() {
  FocusedViewController::viewWillAppear();
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
}

void BatteryTestController::updateBatteryState(float batteryLevel,
                                               bool batteryCharging) {
  if (!m_success) {
    m_view.setPrefix("NEED CHARGING");
  }
  constexpr int precision =
      Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int sizeForPrecision =
      Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  constexpr size_t bufferLevelSize = k_maxNumberOfCharacters + sizeForPrecision;
  char bufferLevel[bufferLevelSize];
  Poincare::Print::CustomPrintf(
      bufferLevel, bufferLevelSize, "Level: %*.*ef", batteryLevel,
      Poincare::Preferences::PrintFloatMode::Decimal, precision);
  m_view.setSuffix1(bufferLevel);

  constexpr size_t bufferChargingSize =
      k_maxNumberOfCharacters + sizeForPrecision;
  char bufferCharging[bufferChargingSize];
  Poincare::Print::CustomPrintf(bufferCharging, bufferChargingSize,
                                "Charging: %s", batteryCharging ? "yes" : "no");
  m_view.setSuffix2(bufferCharging);
}

}  // namespace HardwareTest
