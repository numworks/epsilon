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
    m_view.setPrefix("NEED RECHARGE");
  }
  constexpr int maxNumberOfCharacters = 20;
  constexpr int precision =
      Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int sizeForPrecision =
      Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  constexpr size_t bufferSize = maxNumberOfCharacters + sizeForPrecision;
  char buffer[bufferSize];

  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "Level: %*.*ef", batteryLevel,
      Poincare::Preferences::PrintFloatMode::Decimal, precision);
  m_view.setSuffix1(buffer);

  Poincare::Print::CustomPrintf(buffer, bufferSize, "Charging: %s",
                                batteryCharging ? "yes" : "no");
  m_view.setSuffix2(buffer);
}

}  // namespace HardwareTest
