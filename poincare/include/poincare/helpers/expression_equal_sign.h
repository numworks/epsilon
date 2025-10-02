#pragma once

#include <poincare/projection_context.h>
#include <poincare/user_expression.h>

namespace Poincare {
bool ExactAndApproximateLayoutsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    int numberOfSignificantDigits, const ProjectionContext& ctx = {});

}  // namespace Poincare
