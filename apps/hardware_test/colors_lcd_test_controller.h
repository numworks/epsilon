#ifndef HARDWARE_TEST_COLORS_LCD_TEST_CONTROLLER_H
#define HARDWARE_TEST_COLORS_LCD_TEST_CONTROLLER_H

#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace HardwareTest {

class ColorsLCDTestController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    ContentView();
    Escher::BufferTextView* colorsLCDStateTextView() {
      return &m_colorsLCDStateView;
    }
    void setColor(KDColor color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_colorsLCDStateView;
    }
    Escher::BufferTextView m_colorsLCDStateView;
  };
  constexpr static int k_numberOfAcceptablesGlyphErrors = 1;
  constexpr static const char* k_colorsLCDOKText = "COLORS LCD: OK";
  constexpr static const char* k_colorsLCDFailTest = "COLORS LCD: FAIL";
  ContentView m_view;
};

}  // namespace HardwareTest

#endif
