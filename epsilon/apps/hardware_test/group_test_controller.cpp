#include "group_test_controller.h"

#include "app.h"

namespace HardwareTest {

template <size_t NTests>
bool GroupTestController<NTests>::runTest() {
  for (size_t testIndex = 0; testIndex < m_tests.size(); testIndex++) {
    if (!m_tests[testIndex]->runTest()) {
      m_failingTestIndex = testIndex;
      return false;
    }
  }
  m_failingTestIndex = k_noFail;
  return true;
}

template class GroupTestController<App::k_numberOfGroupedTests>;

}  // namespace HardwareTest
