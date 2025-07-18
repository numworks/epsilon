#ifndef POINCARE_FUNCTION_PROPERTIES_INTEGRAL_H
#define POINCARE_FUNCTION_PROPERTIES_INTEGRAL_H

#include <poincare/expression.h>

namespace Poincare {

// Returns the integral of a function between two bounds.
/* REFACTOR: expression and variableName could be grouped in a single "function"
 * object */
SystemExpression IntegralBetweenBounds(SystemExpression expression,
                                       const char* variableName,
                                       SystemExpression lowerBound,
                                       SystemExpression upperBound);

}  // namespace Poincare

#endif
