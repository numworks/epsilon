#include <poincare/derivative.h>
#include <poincare/init.h>
#include <poincare/src/parsing/parser.h>
#include <poincare_nodes.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void assert_parses_and_reduces_as(const char * expression, const char * derivative) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression eReduced = e.reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  Expression d = parse_expression(derivative, &globalContext, false).reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  quiz_assert_print_if_failure(eReduced.isIdenticalTo(d), expression);
}

QUIZ_CASE(poincare_differential_operations) {
  assert_parses_and_reduces_as("diff(1,x,1)", "0");
  assert_parses_and_reduces_as("diff(x,x,1)", "1");
  assert_parses_and_reduces_as("diff(1+2,x,1)", "0");
  assert_parses_and_reduces_as("diff(a,x,1)", "0");
  assert_parses_and_reduces_as("diff(diff(x^2,x,y),y,1)","2");

  assert_parses_and_reduces_as("diff(1+x,x,1)", "1");
  assert_parses_and_reduces_as("diff(undef,x,1)", "undef");

  assert_parses_and_reduces_as("diff(x+x,x,4)", "2");
  assert_parses_and_reduces_as("diff(2*x,x,1)", "2");
  assert_parses_and_reduces_as("diff(-x,x,1)", "-1");
  assert_parses_and_reduces_as("diff(3-x,x,1)", "-1");
  assert_parses_and_reduces_as("diff(a*x,x,2)", "a");
  assert_parses_and_reduces_as("diff(a*x+b,x,x)", "a");

  assert_parses_and_reduces_as("diff(x*x,x,3)", "6");
  assert_parses_and_reduces_as("diff(x^2,x,2)", "4");
  assert_parses_and_reduces_as("diff(2^x,x,0)", "ln(2)");
  assert_parses_and_reduces_as("diff(x^2,x,x)", "2*x");
  assert_parses_and_reduces_as("diff(a*x^2+b*x+c,x,x)", "2*a*x+b");
  assert_parses_and_reduces_as("diff(1/x,x,1)", "-1");
}

QUIZ_CASE(poicare_differential_unary_functions) {
  assert_parses_and_reduces_as("diff(sin(x),x,π)","-1");
  assert_parses_and_reduces_as("diff(sin(2y),y,π/12)","√(3)");
  assert_parses_and_reduces_as("diff(sin(2x)+sin(3x),x,π/6)","1");

  assert_parses_and_reduces_as("diff(cos(x),x,π/2)","-1");
}