#pragma once

#include <poincare/system_expression.h>

namespace Poincare::Roots {

SystemExpression Linear(const SystemExpression& a, const SystemExpression& b);

SystemExpression Quadratic(const SystemExpression& a, const SystemExpression& b,
                           const SystemExpression& c);

}  // namespace Poincare::Roots
