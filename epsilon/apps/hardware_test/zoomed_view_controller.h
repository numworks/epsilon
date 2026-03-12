#pragma once

#include <escher/view_controller.h>
#include <ion/display.h>

namespace HardwareTest {

/* This view will focus on the center of the display. ViewWillDisappear must be
 * called manually. We can't use viewDidDisappear before viewWillAppear of next
 * view is called before. */
class FocusedViewController : public Escher::ViewController {
 public:
  using Escher::ViewController::ViewController;
  void viewWillAppear() override {
    Ion::Display::setFocusRatio(2);
    Ion::Display::setFocusOrigin(
        KDPoint(Ion::Display::Width / 4, Ion::Display::Height / 4));
  }
  void viewWillDisappear() { Ion::Display::resetFocus(); }
};

}  // namespace HardwareTest
