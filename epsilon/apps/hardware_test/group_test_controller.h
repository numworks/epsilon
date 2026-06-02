#pragma once

#include <array>

#include "focused_view_controller.h"

namespace HardwareTest {

/* Runs a group of test without stopping between each test. Each test is a
 * FocusedViewController */
template <size_t NTests>
class GroupTestController : public FocusedViewController {
 public:
  GroupTestController(Escher::Responder* parentResponder,
                      std::array<FocusedViewController*, NTests> tests)
      : FocusedViewController(parentResponder), m_tests{tests} {}

  bool runTest() override;

 private:
  // Test name can become more generic if needed
  const char* testName() const override { return "DISPLAY:"; }

  std::array<FocusedViewController*, NTests> m_tests;
};

}  // namespace HardwareTest
