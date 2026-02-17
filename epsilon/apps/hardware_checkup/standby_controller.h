#pragma once

#include <escher/pointer_text_view.h>
#include <escher/solid_color_view.h>
#include <escher/view_controller.h>

namespace HardwareCheckup {

class StandbyController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    ContentView();

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override;
    Escher::PointerTextView m_textView;
  };
  ContentView m_view;
};

}  // namespace HardwareCheckup
