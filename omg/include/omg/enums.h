#ifndef OMG_ENUMS_H
#define OMG_ENUMS_H

#include <stdint.h>

namespace OMG {

enum class Base : uint8_t {
  Binary = 2,
  Decimal = 10,
  Hexadecimal = 16
};

enum class Direction {
  Left,
  Up,
  Down,
  Right
};

enum class HorizontalDirection {
  Left,
  Right
};

enum class VerticalDirection {
  Up,
  Down
};

}

#endif
