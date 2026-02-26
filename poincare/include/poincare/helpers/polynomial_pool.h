#pragma once

#include <poincare/analysis_expression.h>

namespace Poincare::PolynomialHelpers {

bool HasNonNullCoefficients(
    const AnalysisExpression& e, const char* symbol,
    OMG::Troolean* highestDegreeCoefficientIsPositive = nullptr);

}
