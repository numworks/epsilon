#ifndef HARDWARE_TEST_USB_TEST_CONTROLLER_H
#define HARDWARE_TEST_USB_TEST_CONTROLLER_H

#include <escher.h>
#include "reset_controller.h"

namespace HardwareTest {

class USBTestController : public ViewController {
public:
  USBTestController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public SolidColorView {
  public:
    ContentView();
    BufferTextView * USBTextView();
  private:
    void layoutSubviews() override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    BufferTextView m_USBTextView;
  };
  constexpr static const char * k_USBPlugText = "PLUG USB";
  constexpr static const char * k_USBUnplugText = "OK, UNPLUG USB";
  ContentView m_view;
  ResetController m_resetController;
};

}

#endif

