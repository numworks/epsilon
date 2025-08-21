#pragma once

#include <stdint.h>

namespace Poincare {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
  Gradian,
  None,
  NUnits = None
};

enum class ComplexFormat : uint8_t {
  Real = 0,
  Cartesian,
  Polar,
  None,
  NFormats = None
};

}  // namespace Poincare
