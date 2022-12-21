#ifndef HARDWARE_TEST_VBLANK_TEST_CONTROLLER_H
#define HARDWARE_TEST_VBLANK_TEST_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <assert.h>

namespace HardwareTest {

class VBlankTestController : public Escher::ViewController {
public:
  using Escher::ViewController::ViewController;
  Escher::View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public Escher::SolidColorView {
  public:
    ContentView();
    Escher::BufferTextView * vBlankStateTextView() { return &m_vBlankStateView; }
    void setColor(KDColor color) override;
  private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_vBlankStateView;
    }
    Escher::BufferTextView m_vBlankStateView;
  };
  constexpr static const char * k_vBlankOKText = "VBLANK: OK";
  constexpr static const char * k_vBlankFailTest = "VBLANK: FAIL";
  ContentView m_view;
};

}

#endif

