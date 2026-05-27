#include "battery_viewer_controller.h"

#include <ion/authentication.h>
#include <ion/battery.h>
#include <kandinsky/basic_color.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/print_float.h>

namespace HardwareCheckup {

bool BatteryViewerController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    viewWillAppear();
    return true;
  }
  return false;
}

void BatteryViewerController::ContentView::setColor(
    Escher::ThemeComponent color) {
  Escher::SolidColorView::setColor(color);
  m_textView.setBackgroundColor(color);
}

void BatteryViewerController::viewWillAppear() {
  float batteryLevel = Ion::Battery::voltage();
  bool batteryCharging = Ion::Battery::isCharging();
  Escher::ThemeComponent color;
  const char* levelText;
  switch (Ion::Battery::level()) {
    case Ion::Battery::Charge::EMPTY:
      levelText = "EMPTY";
      color = Escher::ThemeComponent::BasicBlue;
      break;
    case Ion::Battery::Charge::LOW:
      levelText = "LOW";
      color = Escher::ThemeComponent::BasicPurple;
      break;
    case Ion::Battery::Charge::P40:
      levelText = "P40";
      color = Escher::ThemeComponent::BasicRed;
      break;
    case Ion::Battery::Charge::P60:
      levelText = "P60";
      color = Escher::ThemeComponent::BasicOrange;
      break;
    case Ion::Battery::Charge::P80:
      levelText = "P80";
      color = Escher::ThemeComponent::BasicYellow;
      break;
    case Ion::Battery::Charge::FULL:
      levelText = "FULL";
      color = Escher::ThemeComponent::BasicGreen;
      break;
    default:
      OMG_UNREACHABLE;
  }

  char buffer[ContentView::k_bufferSize];

  Poincare::Print::CustomPrintf(
      buffer, ContentView::k_bufferSize,
      "Charge: %s\n\nLevel: %*.*ef\n\nCharging: %s\n\nPress Ok/EXE to refresh.",
      levelText, batteryLevel, Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::VeryLargeNumberOfSignificantDigits,
      batteryCharging ? "yes" : "no");

  m_view.setColor(color);
  m_view.textView()->setText(buffer);
}

void BatteryViewerController::ContentView::layoutSubviews(bool force) {
  setChildFrame(&m_textView,
                KDRect(0, 0, Ion::Display::Width, Ion::Display::Height / 2),
                force);
}

}  // namespace HardwareCheckup
