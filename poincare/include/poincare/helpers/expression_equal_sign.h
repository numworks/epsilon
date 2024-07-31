#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

#include <poincare/expression.h>

namespace Poincare {
// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreStriclyEqual(
    SystemExpression exact, SystemExpression approximate);

}  // namespace Poincare

#endif
