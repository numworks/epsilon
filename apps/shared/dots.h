#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <kandinsky/coordinate.h>

namespace Shared {

class Dots {
public:
  constexpr static KDCoordinate TinyDotDiameter = 5;
  constexpr static float TinyDotRadius = 2.4675f;
  static const uint8_t (&TinyDotMask)[TinyDotDiameter][TinyDotDiameter];

  constexpr static KDCoordinate SmallDotDiameter = 6;
  constexpr static float SmallDotRadius = 3.f;
  static const uint8_t (&SmallDotMask)[SmallDotDiameter][SmallDotDiameter];

  constexpr static KDCoordinate MediumDotDiameter = 7;
  constexpr static float MediumDotRadius = 3.85f;
  static const uint8_t (&MediumDotMask)[MediumDotDiameter][MediumDotDiameter];

  constexpr static KDCoordinate LargeDotDiameter = 11;
  constexpr static float LargeDotRadius = 4.f;
  static const uint8_t (&LargeDotMask)[LargeDotDiameter][LargeDotDiameter];

  constexpr static float LargeRingInternalRadius = 4.25f;
  constexpr static float LargeRingExternalRadius = 5.5f;
  static const uint8_t (&LargeRingMask)[LargeDotDiameter][LargeDotDiameter];
};

}

#endif
