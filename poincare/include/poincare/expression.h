#ifndef POINCARE_EXPRESSION_ALIAS_H
#define POINCARE_EXPRESSION_ALIAS_H

// clang-format off
// Force the include order
#include <poincare/old/old_expression.h>
#include <poincare/old/junior_expression.h>
// clang-format on

namespace Poincare {
using Expression = JuniorExpression;
}  // namespace Poincare

#endif
