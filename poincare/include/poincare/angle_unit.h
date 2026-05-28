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
  LastUnit = None
};

}  // namespace Poincare
