#ifndef CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_TRIGONOMETRY_HELPER_H

#include <poincare/expression.h>

namespace Calculation {

namespace TrigonometryHelper {

Poincare::Expression ExtractExactAngleFromDirectTrigo(
    Poincare::Expression input, Poincare::Expression exactOutput,
    Poincare::Context* context);

}

}  // namespace Calculation

#endif
