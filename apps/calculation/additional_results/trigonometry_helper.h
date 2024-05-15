#ifndef CALCULATION_TRIGONOMETRY_HELPER_H
#define CALCULATION_TRIGONOMETRY_HELPER_H

#include <poincare/expression.h>

namespace Calculation {

namespace TrigonometryHelper {

Poincare::UserExpression ExtractExactAngleFromDirectTrigo(
    const Poincare::UserExpression input,
    const Poincare::UserExpression exactOutput, Poincare::Context* context,
    const Poincare::Preferences::CalculationPreferences calculationPreferences);

}

}  // namespace Calculation

#endif
