#ifndef LCD_DATA_TEST_CONTROLLER_H
#define LCD_DATA_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

class LCDDataTestController : public ViewController {
public:
  LCDDataTestController(Responder * parentResponder) :
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
    constexpr static const char * k_lcdDataPassTest = "LCD DATA: OK";
    constexpr static const char * k_lcdDataFailTest = "LCD DATA: FAIL";
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 2; }
    View * subviewAtIndex(int index) override {
      assert(index >= 0 && index < 2);
      return index == 0 ? &m_lcdDataStateView : &m_lcdNumberPixelFailuresView;
    }
    BufferTextView m_lcdDataStateView;
    BufferTextView m_lcdNumberPixelFailuresView;
  };
  static constexpr int k_errorLimit = 0;
  void runTest();
  bool m_testSuccessful;
  ContentView m_view;
};

}

#endif

