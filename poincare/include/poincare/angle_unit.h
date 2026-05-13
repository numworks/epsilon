#pragma once

#include "stdint.h"

namespace Poincare {

enum class AngleUnit : uint8_t {
  Radian = 0,
  Degree,
#if !POINCARE_UNIT_ONLY_ANGLES
  Gradian,
#endif
  None,
  NUnits = None
};

}  // namespace Poincare
