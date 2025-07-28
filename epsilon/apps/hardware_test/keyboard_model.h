#ifndef HARDWARE_TEST_KEYBOARD_MODEL_H
#define HARDWARE_TEST_KEYBOARD_MODEL_H

#include <ion/keyboard.h>

#include <array>

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
  int indexInTestedKeys(Ion::Keyboard::Key key) const;

  constexpr static Ion::Keyboard::Key TestedKeys[] = {
#if HWTEST_ALL_KEYS
      Ion::Keyboard::Key::Left,
      Ion::Keyboard::Key::Up,
      Ion::Keyboard::Key::Down,
      Ion::Keyboard::Key::Right,
      Ion::Keyboard::Key::Home,
      Ion::Keyboard::Key::OK,
      Ion::Keyboard::Key::Back,
      Ion::Keyboard::Key::Shift,
      Ion::Keyboard::Key::Alpha,
      Ion::Keyboard::Key::XNT,
      Ion::Keyboard::Key::Var,
      Ion::Keyboard::Key::Toolbox,
      Ion::Keyboard::Key::Backspace,
      Ion::Keyboard::Key::Exp,
      Ion::Keyboard::Key::Ln,
      Ion::Keyboard::Key::Log,
      Ion::Keyboard::Key::Imaginary,
      Ion::Keyboard::Key::Comma,
      Ion::Keyboard::Key::Power,
      Ion::Keyboard::Key::Sine,
      Ion::Keyboard::Key::Cosine,
      Ion::Keyboard::Key::Tangent,
      Ion::Keyboard::Key::Pi,
      Ion::Keyboard::Key::Sqrt,
      Ion::Keyboard::Key::Square,
      Ion::Keyboard::Key::Seven,
      Ion::Keyboard::Key::Eight,
      Ion::Keyboard::Key::Nine,
      Ion::Keyboard::Key::LeftParenthesis,
      Ion::Keyboard::Key::RightParenthesis,
      Ion::Keyboard::Key::Four,
      Ion::Keyboard::Key::Five,
      Ion::Keyboard::Key::Six,
      Ion::Keyboard::Key::Multiplication,
      Ion::Keyboard::Key::Division,
      Ion::Keyboard::Key::One,
      Ion::Keyboard::Key::Two,
      Ion::Keyboard::Key::Three,
      Ion::Keyboard::Key::Plus,
      Ion::Keyboard::Key::Minus,
      Ion::Keyboard::Key::Zero,
      Ion::Keyboard::Key::Dot,
      Ion::Keyboard::Key::EE,
      Ion::Keyboard::Key::Ans,
      Ion::Keyboard::Key::EXE,
#else
      Ion::Keyboard::Key::Left,      Ion::Keyboard::Key::Up,
      Ion::Keyboard::Key::Down,      Ion::Keyboard::Key::Right,
      Ion::Keyboard::Key::OK,        Ion::Keyboard::Key::Back,
      Ion::Keyboard::Key::Home,      Ion::Keyboard::Key::Shift,
      Ion::Keyboard::Key::Alpha,     Ion::Keyboard::Key::XNT,
      Ion::Keyboard::Key::Var,       Ion::Keyboard::Key::Toolbox,
      Ion::Keyboard::Key::Backspace, Ion::Keyboard::Key::Power,
      Ion::Keyboard::Key::Square,    Ion::Keyboard::Key::RightParenthesis,
      Ion::Keyboard::Key::Division,  Ion::Keyboard::Key::Minus,
      Ion::Keyboard::Key::EXE,
#endif
  };
  constexpr static int NumberOfTestedKeys = std::size(TestedKeys);

 private:
  int m_testedKeyIndex;
};
}  // namespace HardwareTest

#endif
