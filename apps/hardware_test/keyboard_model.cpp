#include "keyboard_model.h"

namespace HardwareTest {

constexpr Ion::Keyboard::Key
    KeyboardModel::TestedKeys[KeyboardModel::NumberOfTestedKeys];

bool KeyboardModel::belongsToTestedKeysSubset(Ion::Keyboard::Key key) const {
  for (int i = 0; i < NumberOfTestedKeys; i++) {
    if (TestedKeys[i] == key) {
      return true;
    }
  }
  return false;
}

}  // namespace HardwareTest
