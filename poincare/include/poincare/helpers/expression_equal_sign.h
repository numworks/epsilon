#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

#include <poincare/expression.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    const Internal::ProjectionContext* ctx);

}  // namespace Poincare

#endif
