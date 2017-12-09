#ifndef HARDWARE_TEST_BATTERY_TEST_CONTROLLER_H
#define HARDWARE_TEST_BATTERY_TEST_CONTROLLER_H

#include <escher.h>

namespace HardwareTest {

class BatteryTestController final : public ViewController {
public:
  BatteryTestController(Responder * parentResponder) : ViewController(parentResponder), m_view() {}
  View * view() override {
    return &m_view;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView final : public SolidColorView {
  public:
    ContentView();
    BufferTextView * batteryStateTextView() {
      return &m_batteryStateView;
    }
    BufferTextView * batteryLevelTextView() {
      return &m_batteryLevelView;
    }
    BufferTextView * batteryChargingTextView() {
      return &m_batteryChargingView;
    }
    constexpr static int k_maxNumberOfCharacters = 20;
    void setColor(KDColor color) override;
  private:
    void layoutSubviews() override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    constexpr static int k_margin = 4;
    BufferTextView m_batteryStateView;
    BufferTextView m_batteryLevelView;
    BufferTextView m_batteryChargingView;
  };
  constexpr static const char * k_batteryOKText = "BATTERY: OK";
  constexpr static const char * k_batteryNeedChargingText = "BATTERY: NEED RECHARGE";
  void updateBatteryState(float batteryLevel, bool batteryCharging);
  ContentView m_view;
};

}

#endif

