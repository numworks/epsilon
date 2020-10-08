#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <kandinsky/coordinate.h>

namespace Shared {

class Dots {
public:
  static constexpr KDCoordinate TinyDotDiameter = 5;
  static const uint8_t TinyDotMask[TinyDotDiameter][TinyDotDiameter];
  static constexpr KDCoordinate SmallDotDiameter = 7;
  static const uint8_t SmallDotMask[SmallDotDiameter][SmallDotDiameter];
  static constexpr KDCoordinate LargeDotDiameter = 10;
  static const uint8_t LargeDotMask[LargeDotDiameter][LargeDotDiameter];
};

}

#endif
