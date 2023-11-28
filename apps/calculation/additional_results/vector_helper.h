#ifndef CALCULATION_VECTOR_HELPER_H
#define CALCULATION_VECTOR_HELPER_H

#include <poincare/expression.h>

namespace Calculation {

namespace VectorHelper {

Poincare::Expression BuildVectorNorm(
    const Poincare::Expression exactOutput, Poincare::Context* context,
    const Poincare::Preferences::ComplexFormat complexFormat,
    const Poincare::Preferences::AngleUnit angleUnit);

}

}  // namespace Calculation

#endif
