#ifndef LCD_DATA_TEST_CONTROLLER_H
#define LCD_DATA_TEST_CONTROLLER_H

#include <escher.h>
#include <assert.h>

namespace HardwareTest {

class LCDDataTestController : public ViewController {

/* There are three types of tests, where a pattern is pushed to the screen and
 * the number of invalid pixels then counted.
 *   - Test 1: Tile the screen with color patches. Tiling increases the number
 *   of border mistakes.
 *   - Test 2: Push one color to the whole screen in one step. It shows errors
 *   that appear on large and fast pushes.
 *   - Test 3: Color the screen by alterning one pixel black and one pixel
 *   white (maximal data difference), at maximal data writing speed.
 * Tests 1 and 2 are done for a few different colors. */

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

  ContentView m_view;
};

}

#endif

