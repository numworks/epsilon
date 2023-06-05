#include "keyboard_model.h"

namespace HardwareTest {

constexpr Ion::Keyboard::Key
    KeyboardModel::TestedKeys[KeyboardModel::NumberOfTestedKeys];

int KeyboardModel::indexInTestedKeys(Ion::Keyboard::Key key) const {
  for (int i = 0; i < NumberOfTestedKeys; i++) {
    if (TestedKeys[i] == key) {
      return i;
    }
  }
  return -1;
}

}  // namespace HardwareTest
