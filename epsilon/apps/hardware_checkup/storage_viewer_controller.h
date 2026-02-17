#pragma once

#include <escher/buffer_text_view.h>
#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace HardwareCheckup {

class StorageViewerController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    constexpr static size_t k_bufferSize = 512;
    ContentView() : SolidColorView(KDColorWhite) {}
    Escher::AbstractBufferTextView* textView() { return &m_textView; }
    void setColor(KDColor color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override;
    Escher::BufferTextView<k_bufferSize> m_textView;
  };
  ContentView m_view;
};

}  // namespace HardwareCheckup
