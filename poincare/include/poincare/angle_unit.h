#pragma once

#include "stdint.h"

namespace Poincare {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
  Gradian,
  None,
  NUnits = None
};

}  // namespace Poincare
