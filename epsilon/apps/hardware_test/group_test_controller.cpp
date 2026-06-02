#include "group_test_controller.h"

#include "app.h"

namespace HardwareTest {

template <size_t NTests>
bool GroupTestController<NTests>::runTest() {
  for (FocusedViewController* test : m_tests) {
    if (!test->runTest()) {
      return false;
    }
  }
  return true;
}

template class GroupTestController<App::k_numberOfGroupedTests>;

}  // namespace HardwareTest
