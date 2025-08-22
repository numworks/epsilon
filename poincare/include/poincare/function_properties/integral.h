#pragma once

#include <poincare/expression.h>

namespace Poincare {

// Returns the integral of a function between two bounds.
/* REFACTOR: expression and variableName could be grouped in a single "function"
 * object */
SystemExpression IntegralBetweenBounds(const SystemExpression& expression,
                                       const char* variableName,
                                       const SystemExpression& lowerBound,
                                       const SystemExpression& upperBound);

// Returns the scalar approximation of the integral
template <typename T>
T ApproximateIntegralBetweenBounds(const SystemExpression& expression,
                                   const char* variableName,
                                   const SystemExpression& lowerBound,
                                   const SystemExpression& upperBound);
}  // namespace Poincare
