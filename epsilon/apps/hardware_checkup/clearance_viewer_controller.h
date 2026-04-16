#pragma once

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <kandinsky/basic_color.h>

namespace HardwareCheckup {

class ClearanceViewerController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::RawColorSolidColorView {
   public:
    constexpr static size_t k_bufferSize = 512;
    ContentView() : Escher::RawColorSolidColorView(BasicColor::White) {}
    Escher::AbstractBufferTextView* textView() { return &m_textView; }
    void setColor(KDColor color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_textView;
    }
    /* TODO: turn back to a normal OneLineBufferTextView when SolidColorView is
     * themed */
    Escher::RawColorBufferTextView<k_bufferSize> m_textView;
  };
  ContentView m_view;
};

}  // namespace HardwareCheckup
