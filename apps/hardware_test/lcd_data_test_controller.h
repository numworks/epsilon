#ifndef LCD_DATA_TEST_CONTROLLER_H
#define LCD_DATA_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

class LCDDataTestController : public ViewController {

/* We want to test that:
 * - Command/data switching is OK,
 * - Data is correctly sent,
 * - There are no short-circuits between the data wires.
 * We thus send a tiled pattern (to test command/data switching), where each
 * tile is a checker of a color and its contrary (to tests that Data is sent
 * OK). To test each of the 16 data wires for short-circuits, we use 16 colors:
 * 2**k with 0 <= k < 16. */

public:
  LCDDataTestController(Responder * parentResponder) :
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
    BufferTextView * lcdDataStateTextView() { return &m_lcdDataStateView; }
    void setColor(KDColor color) override;
  private:
    void layoutSubviews() override;
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_lcdDataStateView;
    }
    BufferTextView m_lcdDataStateView;
  };
  constexpr static const char * k_lcdDataOKText = "LCD DATA: OK";
  constexpr static const char * k_lcdDataFailTest = "LCD DATA: FAIL";
  constexpr static int k_LCDTestIterationsCount = 20;

  ContentView m_view;
};

}

#endif

