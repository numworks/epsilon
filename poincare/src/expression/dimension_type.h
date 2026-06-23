#pragma once

#include <stdint.h>

#include "dimension_vector.h"

namespace Poincare::Internal {

enum class DimensionType : uint8_t {
  Scalar,
  Matrix,
  Unit,
  Boolean,
  Point,
  None,
};
// Note: AllowedDimensions should be updated if DimensionType changes

struct MatrixDimension {
  uint8_t rows;
  uint8_t cols;
};

namespace Units {
class Representative;
}

struct UnitDimension {
  Units::SIVector vector;
  // Only one representative is needed for now.
  const Units::Representative* representative;
};

}  // namespace Poincare::Internal
