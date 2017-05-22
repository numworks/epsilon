#ifndef HARDWARE_TEST_KEYBOARD_CONTROLLER_H
#define HARDWARE_TEST_KEYBOARD_CONTROLLER_H

#include <escher.h>
#include "keyboard_view.h"
#include "screen_test_controller.h"

namespace HardwareTest {

class KeyboardTestController : public ViewController {
public:
  KeyboardTestController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public View {
  public:
    ContentView();
    KeyboardView * keyboardView();
    BufferTextView * batteryLevelTextView();
    BufferTextView * batteryChargingTextView();
    BufferTextView * LEDStateTextView();
    constexpr static int k_maxNumberOfCharacters = 20;
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    void layoutSubviews() override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    constexpr static int k_margin = 4;
    KeyboardView m_keyboardView;
    BufferTextView m_batteryLevelView;
    BufferTextView m_batteryChargingView;
    BufferTextView m_ledStateView;
  };
  void updateBatteryState(float batteryLevel, bool batteryCharging);
  void setLEDColor(KDColor color);
  constexpr static int k_numberOfColors = 5;
  constexpr static KDColor k_LEDColors[k_numberOfColors] = {KDColorBlack, KDColorRed, KDColorBlue, KDColorGreen, KDColorWhite};
  static KDColor LEDColorAtIndex(int i);
  ContentView m_view;
  int m_LEDColorIndex;
  ScreenTestController m_screenTestController;
};

}

#endif

