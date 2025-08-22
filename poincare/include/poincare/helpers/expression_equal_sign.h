#pragma once

#include <poincare/expression.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStrictlyEqual(
    const UserExpression exact, const UserExpression approximate,
    const Internal::ProjectionContext& ctx = {});

}  // namespace Poincare
