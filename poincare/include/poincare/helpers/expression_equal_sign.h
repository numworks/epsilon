#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

#include <poincare/expression.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStrictlyEqual(
    UserExpression exact, UserExpression approximate,
    Internal::ProjectionContext* ctx);

}  // namespace Poincare

#endif
