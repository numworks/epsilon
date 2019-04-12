#ifndef HARDWARE_TEST_CENTERED_SCREEN_TEST_CONTROLLER_H
#define HARDWARE_TEST_CENTERED_SCREEN_TEST_CONTROLLER_H

#include <escher.h>

namespace HardwareTest {

class CenteredScreenTestController : public ViewController {
public:
  CenteredScreenTestController(Responder * parentResponder) :
    ViewController(parentResponder),
    m_view()
  {}
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override {
    // This will be handled by the WizardViewController
    return false;
  }
private:
  class ContentView : public View {
  public:
    ContentView() {}
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    constexpr static KDCoordinate k_outlineThickness = 1;
  };

  ContentView m_view;
};

}

#endif

