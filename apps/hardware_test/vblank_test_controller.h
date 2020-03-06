#ifndef HARDWARE_TEST_VBLANK_TEST_CONTROLLER_H
#define HARDWARE_TEST_VBLANK_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

class VBlankTestController : public ViewController {
public:
  VBlankTestController(Responder * parentResponder) :
    ViewController(parentResponder),
    m_view()
  {}
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public SolidColorView {
  public:
    ContentView();
    BufferTextView * vBlankStateTextView() { return &m_vBlankStateView; }
    void setColor(KDColor color) override;
  private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_vBlankStateView;
    }
    BufferTextView m_vBlankStateView;
  };
  constexpr static const char * k_vBlankOKText = "VBLANK: OK";
  constexpr static const char * k_vBlankFailTest = "VBLANK: FAIL";
  ContentView m_view;
};

}

#endif

