#include "helper.h"
#include <apps/shared/global_context.h>
#include <poincare/solver2.h>

using namespace Poincare;

void assert_root_is(const char * expression, double tMin, double tMax, double expectedRoot, double precision = Float<double>::Epsilon()) {
  Shared::GlobalContext context;
  Expression e = parse_expression(expression, &context, false);
  ExpressionNode::ComputationContext computationContext(&context, Real, Radian);
  Solver2<double> solver(&computationContext, tMin, tMax, "x", precision);
  double observedRoot = solver.nextRoot(e).x1();
  quiz_assert_print_if_failure(std::isfinite(expectedRoot) == std::isfinite(observedRoot), expression);
  quiz_assert_print_if_failure(!std::isfinite(observedRoot) || Helpers::RelativelyEqual(observedRoot, expectedRoot, precision), expression);
}

QUIZ_CASE(poincare_solver_2) {
  assert_root_is("1", -10., 10., NAN);
  assert_root_is("x", -10., 10., 0.);
  assert_root_is("x*(x-1)", 0., 10., 1.);
  assert_root_is("cos(x)", -10., 10., -7.8539816339744828);
  assert_root_is("x^2+2x+1", -10., 10., -1., 1e-8); // FIXME Insufficient precision
  assert_root_is("1/x", -10., 10., NAN);
  assert_root_is("(x-100)/(x-101)", 0., 200., 100.);
  assert_root_is("x^2/((x-1)(x+1))", -10., 10., 0.);
  assert_root_is("(x-5)^2/((x-6)(x-4))", -10., 10., 5.);
  assert_root_is("(x+1)^2/(x^2*(x+2))", -10., 10., -1.);
  assert_root_is("(x+1)ln(x)", 0., -10., NAN);
  // assert_root_is("x^(1/x)", -123., 123., NAN); // FIXME Infinite loop
  assert_root_is("x^x", -1e-2, 1e2, NAN);
}
