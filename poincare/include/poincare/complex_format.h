#pragma once

#include "stdint.h"

namespace Poincare {

enum class ComplexFormat : uint8_t {
  Real = 0,
  Cartesian,
  Polar,
  None,
  NFormats = None
};

}  // namespace Poincare
