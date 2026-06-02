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
  const char* testName() const override {
    return m_failingTestIndex == k_noFail
               // Test name can become more generic if needed
               ? "DISPLAY:"
               : m_tests[m_failingTestIndex]->testName();
  }

 private:
  std::array<FocusedViewController*, NTests> m_tests;
  constexpr static int k_noFail = -1;
  int m_failingTestIndex = k_noFail;
};

}  // namespace HardwareTest
