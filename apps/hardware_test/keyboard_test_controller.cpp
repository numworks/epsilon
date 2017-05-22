#include "keyboard_test_controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}
#include <poincare.h>

using namespace Poincare;

namespace HardwareTest {

constexpr KDColor KeyboardTestController::k_LEDColors[k_numberOfColors];

KeyboardTestController::KeyboardTestController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(),
  m_LEDColorIndex(0),
  m_screenTestController(nullptr)
{
}

View * KeyboardTestController::view() {
  return &m_view;
}

bool KeyboardTestController::handleEvent(Ion::Events::Event event) {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  setLEDColor(LEDColorAtIndex(m_LEDColorIndex++));
  if (m_LEDColorIndex == k_numberOfColors) {
    m_LEDColorIndex = 0;
  }
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  int shift = (uint8_t)Ion::Keyboard::ValidKeys[m_view.keyboardView()->testedKeyIndex()];
  Ion::Keyboard::State onlyKeyDown = (uint64_t)1 << shift;
  if (state == onlyKeyDown) {
    m_view.keyboardView()->setTestedKeyIndex(m_view.keyboardView()->testedKeyIndex()+1);
    if (m_view.keyboardView()->testedKeyIndex() == Ion::Keyboard::NumberOfValidKeys) {
      setLEDColor(KDColorBlack);
      ModalViewController * modal = (ModalViewController *)parentResponder();
      modal->displayModalViewController(&m_screenTestController, 0.0f, 0.0f);
    }
  }
  return true;
}

void KeyboardTestController::viewWillAppear() {
  m_LEDColorIndex = 0;
  setLEDColor(LEDColorAtIndex(m_LEDColorIndex++));
  updateBatteryState(Ion::Battery::voltage(), Ion::Battery::isCharging());
  m_view.keyboardView()->setTestedKeyIndex(0);
}

void KeyboardTestController::updateBatteryState(float batteryLevel, bool batteryCharging) {
  char bufferLevel[ContentView::k_maxNumberOfCharacters + Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "Battery level: ";
  int legendLength = strlen(legend);
  strlcpy(bufferLevel, legend, legendLength+1);
  Complex::convertFloatToText(batteryLevel, bufferLevel+legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_view.batteryLevelTextView()->setText(bufferLevel);

  char bufferCharging[ContentView::k_maxNumberOfCharacters + Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
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

void KeyboardTestController::setLEDColor(KDColor color) {
  Ion::LED::setColor(color);

  char ledLevel[ContentView::k_maxNumberOfCharacters];
  const char * legend = "LED color: ";
  int legendLength = strlen(legend);
  int numberOfChar = legendLength;
  strlcpy(ledLevel, legend, legendLength+1);
  legend = "Off";
  if (color == KDColorWhite) {
    legend = "White";
  }
  if (color == KDColorRed) {
    legend = "Red";
  }
  if (color == KDColorBlue) {
    legend = "Blue";
  }
  if (color == KDColorGreen) {
    legend = "Green";
  }
  legendLength = strlen(legend);
  strlcpy(ledLevel+numberOfChar, legend, legendLength+1);
  m_view.LEDStateTextView()->setText(ledLevel);
}

KDColor KeyboardTestController::LEDColorAtIndex(int i) {
  assert(i >= 0 && i < k_numberOfColors);
  return k_LEDColors[i];
}

KeyboardTestController::ContentView::ContentView() :
  m_keyboardView(),
  m_batteryLevelView(KDText::FontSize::Small),
  m_batteryChargingView(KDText::FontSize::Small),
  m_ledStateView(KDText::FontSize::Small)
{
}

KeyboardView * KeyboardTestController::ContentView::keyboardView() {
  return &m_keyboardView;
}

BufferTextView * KeyboardTestController::ContentView::batteryLevelTextView() {
  return &m_batteryLevelView;
}

BufferTextView * KeyboardTestController::ContentView::batteryChargingTextView() {
  return &m_batteryChargingView;
}

BufferTextView * KeyboardTestController::ContentView::LEDStateTextView() {
  return &m_ledStateView;
}

void KeyboardTestController::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
}

void KeyboardTestController::ContentView::layoutSubviews() {
  m_keyboardView.setFrame(KDRect(0, 0, 130, bounds().height()));
  KDSize textSize = KDText::stringSize(" ", KDText::FontSize::Small);
  m_batteryLevelView.setFrame(KDRect(130, k_margin, bounds().width()-130, textSize.height()));
  m_batteryChargingView.setFrame(KDRect(130, k_margin+2*textSize.height(), bounds().width()-130, textSize.height()));
  m_ledStateView.setFrame(KDRect(130, k_margin+5*textSize.height(), bounds().width()-130, textSize.height()));
}

int KeyboardTestController::ContentView::numberOfSubviews() const {
  return 4;
}

View * KeyboardTestController::ContentView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_keyboardView;
  }
  if (index == 1) {
    return &m_batteryLevelView;
  }
  if (index == 2) {
    return &m_batteryChargingView;
  }
  return &m_ledStateView;
}

}

