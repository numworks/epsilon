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

void assert_parses_and_reduces_as(const char * expression, const char * derivative) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression eReduced = e.reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  Expression d = parse_expression(derivative, &globalContext, false).reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  quiz_assert_print_if_failure(eReduced.isIdenticalTo(d), expression);
}

QUIZ_CASE(poincare_differential_addition) {
  assert_parses_and_reduces_as("diff(1,x,1)", "0");
  assert_parses_and_reduces_as("diff(x,x,1)", "1");
  assert_parses_and_reduces_as("diff(1+2,x,1)", "0");
  assert_parses_and_reduces_as("diff(a,x,1)", "0");

  assert_parses_and_reduces_as("diff(1+x,x,1)", "1");
  assert_parses_and_reduces_as("diff(undef,x,1)", "undef");

  assert_parses_and_reduces_as("diff(x+x,x,4)", "2");
  assert_parses_and_reduces_as("diff(2*x,x,1)", "2");
  assert_parses_and_reduces_as("diff(a*x,x,2)", "a");
  assert_parses_and_reduces_as("diff(a*x+b,x,x)", "a");

  // assert_parses_and_reduces_as("diff(x*x,x,3)", "3");
}