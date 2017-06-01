#ifndef HARDWARE_TEST_RESET_CONTROLLER_H
#define HARDWARE_TEST_RESET_CONTROLLER_H

#include <escher.h>

namespace HardwareTest {

class ResetController : public ViewController {
public:
  ResetController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  class ContentView : public View {
  public:
    ContentView();
    BufferTextView * resetTextView();
  private:
    void layoutSubviews() override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    BufferTextView m_resetTextView;
  };
  ContentView m_view;
};

}

#endif

