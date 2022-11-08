#ifndef CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/context.h>

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(Poincare::Expression approximateExpression, Poincare::Context * context);
Poincare::Layout ScientificLayout(Poincare::Expression approximateExpression, Poincare::Context * context);

}

}

#endif
