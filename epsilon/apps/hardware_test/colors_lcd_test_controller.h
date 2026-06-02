#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class ColorsLCDTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool runTest() override;
  const char* testName() const override { return "COLORS LCD:"; }

 private:
  constexpr static int k_numberOfAcceptablesGlyphErrors = 1;
};

}  // namespace HardwareTest
