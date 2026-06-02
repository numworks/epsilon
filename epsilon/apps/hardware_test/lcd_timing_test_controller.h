#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class LCDTimingTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  void viewWillAppear() override;
  bool runTest() override;

 private:
  constexpr static int k_errorLimit = 0;
  const char* testName() const override { return "LCD TIMING:"; }

  int m_pixelFailures = 0;
};

}  // namespace HardwareTest
