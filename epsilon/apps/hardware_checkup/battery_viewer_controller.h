#pragma once

#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>
#include <kandinsky/basic_color.h>
namespace HardwareCheckup {

class BatteryViewerController : public Escher::ViewController {
 public:
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    constexpr static size_t k_bufferSize = 100;
    ContentView()
        : Escher::SolidColorView(Escher::ThemeComponent::BasicWhite) {}
    Escher::AbstractBufferTextView* textView() { return &m_textView; }
    void setColor(Escher::ThemeComponent color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_textView;
    }

    Escher::BufferTextView<k_bufferSize> m_textView;
  };
  ContentView m_view;
};
}  // namespace HardwareCheckup
