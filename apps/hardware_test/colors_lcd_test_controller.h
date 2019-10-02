#ifndef HARDWARE_TEST_COLORS_LCD_TEST_CONTROLLER_H
#define HARDWARE_TEST_COLORS_LCD_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

class ColorsLCDTestController : public ViewController {
public:
  ColorsLCDTestController(Responder * parentResponder) :
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
    BufferTextView * colorsLCDStateTextView() { return &m_colorsLCDStateView; }
    void setColor(KDColor color) override;
  private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_colorsLCDStateView;
    }
    BufferTextView m_colorsLCDStateView;
  };
  constexpr static int k_numberOfAcceptablesGlyphErrors = 1;
  constexpr static const char * k_colorsLCDOKText = "COLORS LCD: OK";
  constexpr static const char * k_colorsLCDFailTest = "COLORS LCD: FAIL";
  ContentView m_view;
};

}

#endif

