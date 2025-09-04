#pragma once

#include <poincare/projection_context.h>
#include <poincare/user_expression.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    const ProjectionContext& ctx = {});

}  // namespace Poincare
