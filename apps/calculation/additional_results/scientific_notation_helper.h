#ifndef CALCULATION_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/old/context.h>
#include <poincare/old/expression.h>
#include <poincare/old/layout.h>

namespace Calculation {

namespace ScientificNotationHelper {

Poincare::Layout ScientificLayout(
    const Poincare::Expression approximateExpression,
    Poincare::Context* context,
    const Poincare::Preferences::CalculationPreferences calculationPreferences);

}  // namespace ScientificNotationHelper

}  // namespace Calculation

#endif
