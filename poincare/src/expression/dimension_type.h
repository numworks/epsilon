#ifndef POINCARE_EXPRESSION_DIMENSION_TYPE_H
#define POINCARE_EXPRESSION_DIMENSION_TYPE_H

#include <stdint.h>

#include "dimension_vector.h"

namespace Poincare::Internal {

enum class DimensionType {
  Scalar,
  Matrix,
  Unit,
  Boolean,
  Point,
};

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

#endif
