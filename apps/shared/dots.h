#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <kandinsky/coordinate.h>

namespace Shared {
namespace Dots {

enum class Size : uint8_t {
  Tiny,
  Small,
  Medium,
  Large
};

constexpr static KDCoordinate TinyDotDiameter = 5;
constexpr static float TinyDotRadius = 2.4675f;
extern const uint8_t * const TinyDotMask;

constexpr static KDCoordinate SmallDotDiameter = 6;
constexpr static float SmallDotRadius = 3.f;
extern const uint8_t * const SmallDotMask;

constexpr static KDCoordinate MediumDotDiameter = 7;
constexpr static float MediumDotRadius = 3.85f;
extern const uint8_t * const MediumDotMask;

constexpr static KDCoordinate LargeDotDiameter = 9;
constexpr static float LargeDotRadius = 4.f;
extern const uint8_t * const LargeDotMask;

constexpr static KDCoordinate LargeRingDiameter = 11;
constexpr static float LargeRingInternalRadius = 4.25f;
constexpr static float LargeRingExternalRadius = 5.5f;
extern const uint8_t * const LargeRingMask;

}
}

#endif
