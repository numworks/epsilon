#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class ColorsLCDTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;

 private:
  constexpr static int k_numberOfAcceptablesGlyphErrors = 1;

  bool runTest() override;
  const char* testName() const override { return "COLORS LCD:"; }
};

}  // namespace HardwareTest
