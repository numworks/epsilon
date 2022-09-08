#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <kandinsky/coordinate.h>

namespace Shared {

class Dots {
public:
  constexpr static KDCoordinate TinyDotDiameter = 5;
  static const uint8_t TinyDotMask[TinyDotDiameter][TinyDotDiameter];
  constexpr static KDCoordinate SmallDotDiameter = 6;
  static const uint8_t SmallDotMask[SmallDotDiameter][SmallDotDiameter];
  constexpr static KDCoordinate MediumDotDiameter = 7;
  static const uint8_t MediumDotMask[MediumDotDiameter][MediumDotDiameter];
  constexpr static KDCoordinate LargeDotDiameter = 11;
  static const uint8_t (&LargeDotMask)[LargeDotDiameter][LargeDotDiameter];
  static const uint8_t (&LargeRingMask)[LargeDotDiameter][LargeDotDiameter];
};

}

#endif
