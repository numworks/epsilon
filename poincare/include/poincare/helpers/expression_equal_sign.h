#pragma once

#include <poincare/simplification_parameters.h>
#include <poincare/user_expression.h>

namespace Poincare {
/* Checks if the exact and approximate will appear as strictly equal once they
 * are layoutted. */
bool IsCalculationOutputStrictEquality(
    const UserExpression exact, const UserExpression approximate,
    int numberOfSignificantDigits,
    const SimplificationParameters& simplificationParameters);

}  // namespace Poincare
