#include "battery_test_controller.h"
#include <apps/shared/post_and_hardware_tests.h>
#include "app.h"
#include <apps/shared/poincare_helpers.h>
extern "C" {
#include <assert.h>
}
#include <poincare/print_float.h>
#include <poincare/preferences.h>

using namespace Poincare;
using namespace Shared;

namespace HardwareTest {

BatteryTestController::BatteryTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view()
{
}

View * BatteryTestController::view() {
  return &m_view;
}

bool BatteryTestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    if (strcmp(m_view.batteryStateTextView()->text(), k_batteryOKText) == 0) {
      // Handled in WizardViewController
      return false;
    }
  }
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  return true;
}

void BatteryTestController::viewWillAppear() {
  bool batteryOK = Shared::POSTAndHardwareTests::BatteryOK();
  const char * text = batteryOK ? k_batteryOKText : k_batteryNeedChargingText;
  KDColor color = batteryOK ? KDColorGreen : KDColorRed;
  m_view.setColor(color);
  m_view.batteryStateTextView()->setText(text);
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
}

void BatteryTestController::updateBatteryState(float batteryLevel, bool batteryCharging) {
  constexpr int precision = Preferences::LargeNumberOfSignificantDigits;
  constexpr int sizeForPrecision = PrintFloat::charSizeForFloatsWithPrecision(precision);
  constexpr size_t bufferLevelSize = ContentView::k_maxNumberOfCharacters + sizeForPrecision;
  char bufferLevel[bufferLevelSize];
  const char * legend = "Battery level: ";
  int legendLength = strlcpy(bufferLevel, legend, bufferLevelSize);
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<float>(batteryLevel, bufferLevel+legendLength, sizeForPrecision, precision, Preferences::PrintFloatMode::Decimal);
  m_view.batteryLevelTextView()->setText(bufferLevel);

  constexpr size_t bufferChargingSize = ContentView::k_maxNumberOfCharacters + sizeForPrecision;
  char bufferCharging[bufferChargingSize];
  int numberOfChars = 0;
  legend = "Battery charging: ";
  numberOfChars += strlcpy(bufferCharging, legend, bufferChargingSize);
  legend = "no";
  if (batteryCharging) {
    legend = "yes";
  }
  numberOfChars += strlcpy(bufferCharging+numberOfChars, legend, bufferChargingSize);
  bufferCharging[numberOfChars] = 0;
  m_view.batteryChargingTextView()->setText(bufferCharging);
}

BatteryTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_batteryStateView(KDFont::LargeFont),
  m_batteryLevelView(KDFont::SmallFont),
  m_batteryChargingView(KDFont::SmallFont)
{
}

BufferTextView * BatteryTestController::ContentView::batteryStateTextView() {
  return &m_batteryStateView;
}

BufferTextView * BatteryTestController::ContentView::batteryLevelTextView() {
  return &m_batteryLevelView;
}

BufferTextView * BatteryTestController::ContentView::batteryChargingTextView() {
  return &m_batteryChargingView;
}

void BatteryTestController::ContentView::setColor(KDColor color) {
  SolidColorView::setColor(color);
  m_batteryStateView.setBackgroundColor(color);
  m_batteryLevelView.setBackgroundColor(color);
  m_batteryChargingView.setBackgroundColor(color);
}

void BatteryTestController::ContentView::layoutSubviews(bool force) {
  m_batteryStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height/2), force);
  KDSize textSize = KDFont::SmallFont->glyphSize();
  m_batteryLevelView.setFrame(KDRect(0, Ion::Display::Height-2*textSize.height(), Ion::Display::Width, textSize.height()), force);
  m_batteryChargingView.setFrame(KDRect(0, Ion::Display::Height-textSize.height(), Ion::Display::Width, textSize.height()), force);
}

int BatteryTestController::ContentView::numberOfSubviews() const {
  return 3;
}

View * BatteryTestController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_batteryStateView;
  }
  if (index == 1) {
    return &m_batteryLevelView;
  }
  return &m_batteryChargingView;
}

}

