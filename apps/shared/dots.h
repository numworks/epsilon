#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <kandinsky/coordinate.h>

namespace Shared {

class Dots {
public:
  static constexpr KDCoordinate SmallDotDiameter = 5;
  static const uint8_t SmallDotMask[SmallDotDiameter][SmallDotDiameter];
  static constexpr KDCoordinate MediumDotDiameter = 7;
  static const uint8_t MediumDotMask[MediumDotDiameter][MediumDotDiameter];
  static constexpr KDCoordinate LargeDotDiameter = 10;
  static const uint8_t LargeDotMask[LargeDotDiameter][LargeDotDiameter];
};

}

#endif
