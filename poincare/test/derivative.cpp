#include <poincare/derivative.h>
#include <poincare/init.h>
#include <poincare/src/parsing/parser.h>
#include <poincare_nodes.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void assert_differentiates_as(Expression expression, Expression derivative, const char * information) {
  Shared::GlobalContext globalContext;
  Expression expressionReduced = expression.reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  quiz_assert_print_if_failure(expressionReduced.isIdenticalTo(derivative), information);
}

QUIZ_CASE(poincare_differential_addition) {

}