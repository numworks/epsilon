#ifndef POINCARE_EXPRESSION_ALIAS_H
#define POINCARE_EXPRESSION_ALIAS_H

// clang-format off
#include <poincare/old/old_expression.h>
#include <poincare/old/junior_expression.h>
// clang-format on

namespace Poincare {
using Expression = JuniorExpression;
// Can be applied to different types of Expressions
using NewExpression = Expression;
// Can be layoutted (Not projected)
using UserExpression = NewExpression;
// Must have been projected
using ProjectedExpression = NewExpression;
// Must have been systematic simplified
using SystemExpression = ProjectedExpression;
// Must have been systematic simplified and can depend on a Variable
using SystemFunction = SystemExpression;
// Same with Scalar approximation
using SystemCartesianFunction = SystemFunction;
// Same with Point approximation
using SystemParametricFunction = SystemFunction;
}  // namespace Poincare

#endif
