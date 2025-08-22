#pragma once

#include <poincare/expression.h>

namespace Calculation {

namespace VectorHelper {

Poincare::UserExpression BuildVectorNorm(
    const Poincare::UserExpression exactOutput,
    const Poincare::Context& context,
    const Poincare::Preferences::CalculationPreferences calculationPreferences);

}

}  // namespace Calculation
