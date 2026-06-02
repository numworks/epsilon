#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class LCDTimingTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  bool runTest() override;
  const char* testName() const override { return "LCD TIMING:"; }

  int prefixData() const override { return m_pixelFailures; }

 private:
  constexpr static int k_errorLimit = 0;

  int m_pixelFailures = 0;
};

}  // namespace HardwareTest
