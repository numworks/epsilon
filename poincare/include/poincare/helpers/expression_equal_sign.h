#pragma once

#include <poincare/src/expression/projection.h>
#include <poincare/user_expression.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    const Internal::ProjectionContext& ctx = {});

}  // namespace Poincare
