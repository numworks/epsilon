#ifndef ION_KEYBOARD_H
#define ION_KEYBOARD_H

extern "C" {
#include <stdint.h>
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

constexpr int NumberOfKeys  = 54;

bool keyDown(Key k);

}
}

#endif
