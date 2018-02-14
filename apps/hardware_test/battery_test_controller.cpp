#include "battery_test_controller.h"
#include "../constant.h"
#include "app.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

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
      ModalViewController * modal = (ModalViewController *)parentResponder();
      App * a = (App *)app();
      modal->displayModalViewController(a->USBController(), 0.0f, 0.0f);
    }
  }
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  return true;
}

void BatteryTestController::viewWillAppear() {
  const char * text = Ion::Battery::voltage() < k_batteryThreshold ? k_batteryNeedChargingText : k_batteryOKText;
  KDColor color = Ion::Battery::voltage() < k_batteryThreshold ? KDColorRed : KDColorGreen;
  m_view.setColor(color);
  m_view.batteryStateTextView()->setText(text);
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
}

void BatteryTestController::updateBatteryState(float batteryLevel, bool batteryCharging) {
  char bufferLevel[ContentView::k_maxNumberOfCharacters + PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "Battery level: ";
  int legendLength = strlen(legend);
  strlcpy(bufferLevel, legend, legendLength+1);
  PrintFloat::convertFloatToText<float>(batteryLevel, bufferLevel+legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_view.batteryLevelTextView()->setText(bufferLevel);

  char bufferCharging[ContentView::k_maxNumberOfCharacters + PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  int numberOfChars = 0;
  legend = "Battery charging: ";
  legendLength = strlen(legend);
  strlcpy(bufferCharging, legend, legendLength+1);
  numberOfChars += legendLength;
  legend = "no";
  if (batteryCharging) {
    legend = "yes";
  }
  legendLength = strlen(legend);
  strlcpy(bufferCharging+numberOfChars, legend, legendLength+1);
  numberOfChars += legendLength;
  bufferCharging[numberOfChars] = 0;
  m_view.batteryChargingTextView()->setText(bufferCharging);
}

BatteryTestController::ContentView::ContentView() :
  SolidColorView(KDColorWhite),
  m_batteryStateView(KDText::FontSize::Large),
  m_batteryLevelView(KDText::FontSize::Small),
  m_batteryChargingView(KDText::FontSize::Small)
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

void BatteryTestController::ContentView::layoutSubviews() {
  m_batteryStateView.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height/2));
  KDSize textSize = KDText::charSize(KDText::FontSize::Small);
  m_batteryLevelView.setFrame(KDRect(0, Ion::Display::Height-2*textSize.height(), Ion::Display::Width, textSize.height()));
  m_batteryChargingView.setFrame(KDRect(0, Ion::Display::Height-textSize.height(), Ion::Display::Width, textSize.height()));
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

