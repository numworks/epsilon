#ifndef CALCULATION_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(const Poincare::Expression approximateExpression,
                          Poincare::Context* context);
Poincare::Layout ScientificLayout(
    const Poincare::Expression approximateExpression,
    Poincare::Context* context);

}  // namespace ScientificNotationHelper

}  // namespace Calculation

#endif
