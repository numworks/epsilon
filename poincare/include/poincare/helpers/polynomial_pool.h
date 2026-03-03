#pragma once

#include <poincare/system_expression.h>

namespace Poincare::PolynomialHelpers {

bool HasNonNullCoefficients(
    const SystemExpression& e, const char* symbol,
    OMG::Troolean* highestDegreeCoefficientIsPositive = nullptr);

}
