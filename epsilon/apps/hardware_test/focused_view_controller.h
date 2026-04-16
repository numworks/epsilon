#pragma once

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <ion/display.h>

namespace HardwareTest {

// This view will zoom on the screen for better text detection.
class FocusedViewController : public Escher::ViewController {
 private:
  constexpr static int k_zoomFactor = 2;

 public:
  FocusedViewController(Escher::Responder* parentResponder)
      : Escher::ViewController(parentResponder), m_success(false) {}
  Escher::View* view() override { return &m_view; }
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event event) override;

 protected:
  class ContentView : public Escher::RawColorSolidColorView {
   public:
    ContentView();
    Escher::View* subviewAtIndex(int index) override;
    void setTexts(const char* testName, const char* testResult);
    void setPrefix(const char* prefix);
    void setSuffix1(const char* suffix1);
    void setSuffix2(const char* suffix2);
    void setBackgroundColor(KDColor color);

   private:
    constexpr static KDCoordinate k_margin = 5;
    constexpr static KDCoordinate k_width = Ion::Display::Width / k_zoomFactor;
    constexpr static KDCoordinate k_height =
        Ion::Display::Height / k_zoomFactor;

    KDCoordinate layoutGlyphView(Escher::GlyphsView* view, KDCoordinate y,
                                 bool force);
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 5; }
    /* TODO: turn back to a normal OneLineBufferTextView when SolidColorView is
     * themed */
    Escher::OneLineRawColorBufferTextView<> m_prefixView;
    Escher::OneLineRawColorBufferTextView<> m_testNameView;
    Escher::OneLineRawColorBufferTextView<> m_testResultView;
    Escher::OneLineRawColorBufferTextView<> m_suffixView1;
    Escher::OneLineRawColorBufferTextView<> m_suffixView2;
  };
  ContentView m_view;
  bool m_success;

 private:
  virtual const char* testName() const = 0;
  virtual bool runTest() = 0;
};

}  // namespace HardwareTest
