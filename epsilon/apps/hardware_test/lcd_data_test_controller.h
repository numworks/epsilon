#pragma once

#include "focused_view_controller.h"

namespace HardwareTest {

class LCDDataTestController : public FocusedViewController {
 public:
  using FocusedViewController::FocusedViewController;
  void viewWillAppear() override;

 private:
  constexpr static int k_errorLimit = 0;

  bool runTest() override;
  const char* testName() const override { return "LCD DATA:"; }

  int m_pixelFailures = 0;
};

}  // namespace HardwareTest
