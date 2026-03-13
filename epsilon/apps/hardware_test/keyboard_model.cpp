#include "keyboard_model.h"

namespace HardwareTest {

int KeyboardModel::indexInTestedKeys(Ion::Keyboard::Key key) const {
  for (int i = 0; i < m_numberOfTestedKeys; i++) {
    if (m_testedKeys[i] == key) {
      return i;
    }
  }
  return -1;
}

}  // namespace HardwareTest
