#pragma once

#include <assert.h>
#include <escher/buffer_text_view.h>
#include <escher/solid_color_view.h>

#include "zoomed_view_controller.h"

namespace HardwareTest {

class VBlankTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  Escher::View* view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

 private:
  class ContentView : public Escher::SolidColorView {
   public:
    ContentView();
    Escher::AbstractBufferTextView* vBlankStateTextView() {
      return &m_vBlankStateView;
    }
    void setColor(KDColor color) override;

   private:
    void layoutSubviews(bool force = false) override;
    int numberOfSubviews() const override { return 1; }
    Escher::View* subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_vBlankStateView;
    }
    Escher::OneLineBufferTextView<> m_vBlankStateView;
  };
  constexpr static const char* k_vBlankOKText = "VBLANK: OK";
  constexpr static const char* k_vBlankFailTest = "VBLANK: KO";
  ContentView m_view;
};

}  // namespace HardwareTest
