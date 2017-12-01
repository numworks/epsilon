#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

extern "C" {
#include <stdint.h>
#include <assert.h>
}

namespace Ion {
namespace Keyboard {

enum class Key : uint8_t {
  A1=0,  A2=1,  A3=2,  A4=3,  A5=4,  A6=5,
  B1=6,  B2=7, /*  B3=8,  B4=9,  B5=10, B6=11, */
  C1=12, C2=13, C3=14, C4=15, C5=16, C6=17,
  D1=18, D2=19, D3=20, D4=21, D5=22, D6=23,
  E1=24, E2=25, E3=26, E4=27, E5=28, E6=29,
  F1=30, F2=31, F3=32, F4=33, F5=34, // F6=35,
  G1=36, G2=37, G3=38, G4=39, G5=40, // G6=41,
  H1=42, H2=43, H3=44, H4=45, H5=46, // H6=47,
  I1=48, I2=49, I3=50, I4=51, I5=52, // I6=53,
};

constexpr Key ValidKeys[] = {
  Key::A1, Key::A2, Key::A3, Key::A4, Key::A5, Key::A6, Key::B1, Key::B2,
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
private:
  uint64_t m_bitField;
};

State scan();

static_assert(sizeof(State)*8>NumberOfKeys, "Ion::Keyboard::State cannot hold a keyboard snapshot");


}
}

#endif
