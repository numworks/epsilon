#ifndef CALCULATION_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>

namespace Calculation {

namespace ScientificNotationHelper {

Poincare::Layout ScientificLayout(
    const Poincare::Expression approximateExpression,
    Poincare::Context* context);

}  // namespace ScientificNotationHelper

}  // namespace Calculation

#endif
