#ifndef CALCULATION_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/old/context.h>

namespace Calculation {

namespace ScientificNotationHelper {

Poincare::Layout ScientificLayout(
    const Poincare::UserExpression approximateExpression,
    Poincare::Context* context,
    const Poincare::Preferences::CalculationPreferences calculationPreferences);

}  // namespace ScientificNotationHelper

}  // namespace Calculation

#endif
