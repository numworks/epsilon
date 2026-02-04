#pragma once

#include <stdint.h>

namespace Poincare {

enum class Interest : uint8_t {
  None,
  Root,
  Minimum,
  Maximum,
  Discontinuity,
  ReachedDiscontinuity,    // Discontinuity where f(x) is defined
  UnreachedDiscontinuity,  // Discontinuity where f(x) is undefined
  Intersection,
  YIntercept,
};

}  // namespace Poincare
