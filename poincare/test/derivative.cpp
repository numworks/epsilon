#include <poincare/derivative.h>
#include <poincare/init.h>
#include <poincare/src/parsing/parser.h>
#include <poincare_nodes.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void assert_parses_and_reduces_as(const char * expression, const char * derivative) {
  Shared::GlobalContext globalContext;
  Expression d = parse_expression(derivative, &globalContext, false).reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  Expression e = parse_expression(expression, &globalContext, false);
  Expression eReduced = e.reduce(ExpressionNode::ReductionContext(&globalContext, Cartesian, Radian, User));
  quiz_assert_print_if_failure(eReduced.isIdenticalTo(d), expression);
}

QUIZ_CASE(poincare_derivative_literals) {
  assert_parses_and_reduces_as("diff(1,x,1)", "0");
  assert_parses_and_reduces_as("diff(1,x,y)", "0");
  assert_parses_and_reduces_as("diff(1,x,x)", "0");

  assert_parses_and_reduces_as("diff(a,x,1)", "0");
  assert_parses_and_reduces_as("diff(a,x,y)", "0");
  assert_parses_and_reduces_as("diff(a,x,x)", "0");

  assert_parses_and_reduces_as("diff(x,x,1)", "1");
  assert_parses_and_reduces_as("diff(x,x,y)", "1");
  assert_parses_and_reduces_as("diff(x,x,x)", "1");

  assert_parses_and_reduces_as("diff(undef,x,0)", "undef");
}

QUIZ_CASE(poincare_derivative_additions) {
  assert_parses_and_reduces_as("diff(1+x,x,1)", "1");
  assert_parses_and_reduces_as("diff(x+a,x,x)", "1");
  assert_parses_and_reduces_as("diff(a+b,x,y)", "0");
}

QUIZ_CASE(poincare_derivative_multiplications) {
  assert_parses_and_reduces_as("diff(2x,x,1)", "2");
  assert_parses_and_reduces_as("diff(x*a,x,y)", "a");
  assert_parses_and_reduces_as("diff(a*x+b,x,x)", "a");
  assert_parses_and_reduces_as("diff(a*b+c,x,1)", "0");
  assert_parses_and_reduces_as("diff(-x,x,y)", "-1");
  assert_parses_and_reduces_as("diff(2-5x,x,x)", "-5");
}

QUIZ_CASE(poincare_derivative_powers) {
  assert_parses_and_reduces_as("diff(x*x,x,1)", "2");
  assert_parses_and_reduces_as("diff(x^2,x,y)", "2y");
  assert_parses_and_reduces_as("diff(x^3/3,x,x)", "x^2");
  assert_parses_and_reduces_as("diff(1/x,x,1)", "-1");
  assert_parses_and_reduces_as("diff(2^x,x,y)", "ln(2)*2^y");
  assert_parses_and_reduces_as("diff(x^(-2),x,x)", "-2/(x^3)");
  assert_parses_and_reduces_as("diff(a^b,x,1)", "0");
  assert_parses_and_reduces_as("diff(x^a,x,y)", "a*y^(a-1)");
  assert_parses_and_reduces_as("diff(a*x^2+b*x+c,x,x)", "2a*x+b");
  assert_parses_and_reduces_as("diff((1+x)(2-x),x,1)", "-1");
  assert_parses_and_reduces_as("diff(diff(x^3/6,x,y),y,z)", "z");
}

QUIZ_CASE(poincare_derivative_functions) {
  assert_parses_and_reduces_as("diff(sin(x),x,x)", "cos(x)");
  assert_parses_and_reduces_as("diff(cos(x),x,x)", "-sin(x)");
  assert_parses_and_reduces_as("diff(tan(x),x,0)", "1");
  assert_parses_and_reduces_as("diff(sin(a)+cos(b)+tan(c),x,y)", "0");
  assert_parses_and_reduces_as("diff(sin(cos(x)),x,y)", "-sin(y)*cos(cos(y))");

  assert_parses_and_reduces_as("diff(ln(x),x,x)", "1/x");
  assert_parses_and_reduces_as("diff(ln(a*x),x,x)", "1/x");
  assert_parses_and_reduces_as("diff(log(x),x,x)", "(x*ln(10))^(-1)");
  assert_parses_and_reduces_as("diff(ln(cos(x)),x,x)", "-tan(x)");
  assert_parses_and_reduces_as("diff(diff(ln(x),x,1/tan(x)),x,x)", "1/(cos(x))^2");
  assert_parses_and_reduces_as("diff(ln(a),x,1)", "0");

  assert_parses_and_reduces_as("diff(sinh(x),x,x)", "cosh(x)");
  assert_parses_and_reduces_as("diff(cosh(x),x,x)", "sinh(x)");
  assert_parses_and_reduces_as("diff(tanh(x),x,0)", "1");
  assert_parses_and_reduces_as("diff(ln(cosh(x)),x,0)", "0");
}