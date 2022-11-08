#ifndef CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_HELPER_H
#define CALCULATION_ADDITIONAL_OUTPUTS_SCIENTIFIC_NOTATION_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(Poincare::Expression approximateExpression);
Poincare::Layout ScientificLayout(Poincare::Expression approximateExpression);

}

}

#endif
