#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

namespace Ion {
namespace Keyboard {

enum class Key {
  A1 =  0, A2 =  1, A3 =  2, A4 =  3, A5 =  4,
  B1 =  5, B2 =  6, B3 =  7, B4 =  8, B5 =  9,
  C1 = 10, C2 = 11, C3 = 12, C4 = 13, C5 = 14,
  D1, D2, D3, D4, D5,
  E1, E2, E3, E4, E5,
  F1, F2, F3, F4, F5,
  G1, G2, G3, G4, G5,
  H1, H2, H3, H4, H5,
  I1, I2, I3, I4, I5,
  J1, J2, J3, J4, J5
};

constexpr int NumberOfKeys  = 50;

bool keyDown(Key k);

}
}

#endif
