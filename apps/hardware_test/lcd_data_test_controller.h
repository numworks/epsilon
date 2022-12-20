#ifndef LCD_DATA_TEST_CONTROLLER_H
#define LCD_DATA_TEST_CONTROLLER_H

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <assert.h>

namespace HardwareTest {

class LCDDataTestController : public Escher::ViewController {
public:
  LCDDataTestController(Escher::Responder * parentResponder) :
    Escher::ViewController(parentResponder),
    m_testSuccessful(false)
  {}
  Escher::View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event & event) override;
  void viewWillAppear() override;
private:
  class ContentView : public Escher::SolidColorView {
  public:
    ContentView();
    void setStatus(bool success, int numberOfErrors);
  private:
    constexpr static const char * k_lcdDataPassTest = "LCD DATA: OK";
    constexpr static const char * k_lcdDataFailTest = "LCD DATA: FAIL";
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 2; }
    Escher::View * subviewAtIndex(int index) override {
      assert(index >= 0 && index < 2);
      return index == 0 ? &m_lcdDataStateView : &m_lcdNumberPixelFailuresView;
    }
    Escher::BufferTextView m_lcdDataStateView;
    Escher::BufferTextView m_lcdNumberPixelFailuresView;
  };
  constexpr static int k_errorLimit = 0;
  void runTest();
  bool m_testSuccessful;
  ContentView m_view;
};

}

#endif

