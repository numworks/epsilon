#ifndef HARDWARE_TEST_KEYBOARD_MODEL_H
#define HARDWARE_TEST_KEYBOARD_MODEL_H

#include <ion/keyboard.h>

namespace HardwareTest {

class KeyboardModel {
 public:
  KeyboardModel() : m_testedKeyIndex(0) {}
  Ion::Keyboard::Key testedKey() const { return TestedKeys[m_testedKeyIndex]; }
  int testedKeyIndex() const { return m_testedKeyIndex; }
  void setTestedKeyIndex(int i) {
    assert(i >= 0 && i <= NumberOfTestedKeys);
    m_testedKeyIndex = i;
  }
  bool belongsToTestedKeysSubset(Ion::Keyboard::Key key) const;

  constexpr static int NumberOfTestedKeys = 19;
  constexpr static Ion::Keyboard::Key
      TestedKeys[KeyboardModel::NumberOfTestedKeys] = {
          Ion::Keyboard::Key::Left,      Ion::Keyboard::Key::Up,
          Ion::Keyboard::Key::Down,      Ion::Keyboard::Key::Right,
          Ion::Keyboard::Key::OK,        Ion::Keyboard::Key::Back,
          Ion::Keyboard::Key::Home,      Ion::Keyboard::Key::Shift,
          Ion::Keyboard::Key::Alpha,     Ion::Keyboard::Key::XNT,
          Ion::Keyboard::Key::Var,       Ion::Keyboard::Key::Toolbox,
          Ion::Keyboard::Key::Backspace, Ion::Keyboard::Key::Power,
          Ion::Keyboard::Key::Square,    Ion::Keyboard::Key::RightParenthesis,
          Ion::Keyboard::Key::Division,  Ion::Keyboard::Key::Minus,
          Ion::Keyboard::Key::EXE};

 private:
  int m_testedKeyIndex;
};
}  // namespace HardwareTest

#endif
