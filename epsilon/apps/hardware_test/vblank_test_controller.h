#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class VBlankTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool runTest() override;

 private:
  const char* testName() const override { return "VBLANK:"; }
};

}  // namespace HardwareTest
