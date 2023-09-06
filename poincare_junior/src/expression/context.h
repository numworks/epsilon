#ifndef POINCARE_EXPRESSION_CONTEXT_H
#define POINCARE_EXPRESSION_CONTEXT_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "k_tree.h"

namespace PoincareJ {

enum class ComplexFormat { Real, Cartesian, Polar };
enum class AngleUnit : uint8_t { Radian = 0, Degree = 1, Gradian = 2 };
enum class Strategy { Default, NumbersToFloat, ApproximateToFloat };

}  // namespace PoincareJ

#endif
