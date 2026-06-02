#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class ColorsLCDTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool runTest() override;

 private:
  constexpr static int k_numberOfAcceptablesGlyphErrors = 1;
  const char* testName() const override { return "COLORS LCD:"; }
};

}  // namespace HardwareTest
