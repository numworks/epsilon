#ifndef LCD_TIMING_TEST_CONTROLLER_H
#define LCD_TIMING_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

//TODO Factorize with LCDData
class LCDTimingTestController : public ViewController {
public:
  LCDTimingTestController(Responder * parentResponder) :
    ViewController(parentResponder),
    m_testSuccessful(false),
    m_view()
  {}
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
private:
  class ContentView : public SolidColorView {
  public:
    ContentView();
    void setStatus(bool success, int numberOfErrors);
  private:
    constexpr static const char * k_lcdTimingPassTest = "LCD TIMING: OK";
    constexpr static const char * k_lcdTimingFailTest = "LCD TIMING: FAIL";
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override {
      assert(index >= 0 && index < 2);
      return index == 0 ? &m_lcdTimingStateView : &m_lcdNumberGlyphFailuresView;
    }
    BufferTextView m_lcdTimingStateView;
    BufferTextView m_lcdNumberGlyphFailuresView;
  };
  static constexpr int k_errorLimit = 0;
  void runTest();
  bool m_testSuccessful;
  ContentView m_view;
};

}

#endif

