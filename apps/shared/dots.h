#ifndef SHARED_DOTS_H
#define SHARED_DOTS_H

#include <assert.h>
#include <kandinsky/coordinate.h>

#include <algorithm>

namespace Shared {
namespace Dots {

enum class Size : uint8_t { Tiny, Small, Medium, Large };

// Dots

constexpr static KDCoordinate TinyDotDiameter = 5;
constexpr static float TinyDotRadius = 2.4675f;
extern const uint8_t* const TinyDotMask;

constexpr static KDCoordinate SmallDotDiameter = 6;
constexpr static float SmallDotRadius = 3.f;
extern const uint8_t* const SmallDotMask;

constexpr static KDCoordinate MediumDotDiameter = 7;
constexpr static float MediumDotRadius = 3.85f;
extern const uint8_t* const MediumDotMask;

constexpr static KDCoordinate LargeDotDiameter = 9;
constexpr static float LargeDotRadius = 4.f;
extern const uint8_t* const LargeDotMask;

// Rings

constexpr static KDCoordinate LargeRingDiameter = 11;
constexpr static float LargeRingInternalRadius = 4.25f;
constexpr static float LargeRingExternalRadius = 5.5f;
extern const uint8_t* const LargeRingMask;

constexpr const uint8_t* Mask(Size size, bool ring) {
  if (ring) {
    assert(size == Size::Large);
    return LargeRingMask;
  }
  switch (size) {
    case Size::Tiny:
      return TinyDotMask;
    case Size::Small:
      return SmallDotMask;
    case Size::Medium:
      return MediumDotMask;
    default:
      assert(size == Size::Large);
      return LargeDotMask;
  }
}

constexpr KDCoordinate Diameter(Size size, bool ring) {
  if (ring) {
    assert(size == Size::Large);
    return LargeRingDiameter;
  }
  switch (size) {
    case Size::Tiny:
      return TinyDotDiameter;
    case Size::Small:
      return SmallDotDiameter;
    case Size::Medium:
      return MediumDotDiameter;
    default:
      assert(size == Size::Large);
      return LargeDotDiameter;
  }
}

}  // namespace Dots
}  // namespace Shared

#endif
