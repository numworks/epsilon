#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

extern "C" {
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
}

#include <ion/keyboard/layout_keyboard.h>

namespace Ion {
namespace Keyboard {

constexpr Key ValidKeys[] = {
  Key::Left, Key::Up, Key::Down, Key::Right, Key::OK, Key::Back,
  Key::Home, Key::OnOff,
  Key::Shift, Key::Alpha, Key::XNT, Key::Var, Key::Toolbox, Key::Backspace,
  Key::Exp, Key::Ln, Key::Log, Key::Imaginary, Key::Comma, Key::Power,
  Key::Sine, Key::Cosine, Key::Tangent, Key::Pi, Key::Sqrt, Key::Square,
  Key::Seven, Key::Eight, Key::Nine, Key::LeftParenthesis, Key::RightParenthesis,
  Key::Four, Key::Five, Key::Six, Key::Multiplication, Key::Division,
  Key::One, Key::Two, Key::Three, Key::Plus, Key::Minus,
  Key::Zero, Key::Dot, Key::EE, Key::Ans, Key::EXE,
};

constexpr int NumberOfKeys  = 54;
constexpr int NumberOfValidKeys  = 46;

class State {
public:
  constexpr State(uint64_t s = 0) :
    m_bitField(s)
  {}
  /* "Shift behavior is undefined if the right operand is negative, or greater
   * than or equal to the length in bits of the promoted left operand" according
   * to C++ spec but k is always in [0:52]. */
  explicit constexpr State(Key k) :
    m_bitField((uint64_t)1 << (uint8_t)k)
  {}
  inline bool keyDown(Key k) const {
    assert((uint8_t)k < 64);
    return (m_bitField>>(uint8_t)k) & 1;
  }
  operator uint64_t() const { return m_bitField; }
  void setKey(Key k) {
    if (k != Key::None) {
      m_bitField |= (uint64_t)1 << (uint8_t)k;
    }
  }
private:
  uint64_t m_bitField;
};

State scan();
State scanForInterruptionAndPopState();
State popState();

static_assert(sizeof(State)*8>NumberOfKeys, "Ion::Keyboard::State cannot hold a keyboard snapshot");

}
}

#endif
