#pragma once

#include <poincare/expression.h>

namespace Poincare::PolynomialHelpers {

bool HasNonNullCoefficients(
    SystemExpression e, const char* symbol,
    OMG::Troolean* highestDegreeCoefficientIsPositive = nullptr);

}
