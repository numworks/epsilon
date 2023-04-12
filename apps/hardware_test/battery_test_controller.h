#ifndef HARDWARE_TEST_BATTERY_TEST_CONTROLLER_H
#define HARDWARE_TEST_BATTERY_TEST_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace HardwareTest {

class BatteryTestController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    ContentView();
    Escher::AbstractBufferTextView* batteryStateTextView();
    Escher::AbstractBufferTextView* batteryLevelTextView();
    Escher::AbstractBufferTextView* batteryChargingTextView();
    constexpr static int k_maxNumberOfCharacters = 20;
    void setColor(KDColor color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override;
    Escher::View* subviewAtIndex(int index) override;
    constexpr static int k_margin = 4;
    Escher::OneLineBufferTextView<> m_batteryStateView;
    Escher::OneLineBufferTextView<> m_batteryLevelView;
    Escher::OneLineBufferTextView<> m_batteryChargingView;
  };
  constexpr static const char* k_batteryOKText = "BATTERY: OK";
  constexpr static const char* k_batteryNeedChargingText =
      "BATTERY: NEED RECHARGE";
  void updateBatteryState(float batteryLevel, bool batteryCharging);
  ContentView m_view;
};

}  // namespace HardwareTest

#endif
