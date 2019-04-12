#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

extern "C" {
#include <stdint.h>
#include <assert.h>
}

#include <keyboard_layout.h>

namespace Ion {
namespace Keyboard {

constexpr Key ValidKeys[] = {
  Key::A1, Key::A2, Key::A3, Key::A4, Key::A5, Key::A6, Key::B1, PowerKey,
  Key::C1, Key::C2, Key::C3, Key::C4, Key::C5, Key::C6, Key::D1, Key::D2,
  Key::D3, Key::D4, Key::D5, Key::D6, Key::E1, Key::E2, Key::E3, Key::E4,
  Key::E5, Key::E6, Key::F1, Key::F2, Key::F3, Key::F4, Key::F5, Key::G1,
  Key::G2, Key::G3, Key::G4, Key::G5, Key::H1, Key::H2, Key::H3, Key::H4,
  Key::H5, Key::I1, Key::I2, Key::I3, Key::I4, Key::I5,
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
    m_bitField |= (uint64_t)1 << (uint8_t)k;
  }
  void clearKey(Key k) {
    m_bitField &= ~((uint64_t)1 << (uint8_t)k);
  }
private:
  uint64_t m_bitField;
};

State scan();

static_assert(sizeof(State)*8>NumberOfKeys, "Ion::Keyboard::State cannot hold a keyboard snapshot");


}
}

#endif
