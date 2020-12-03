#include <poincare/derivative.h>
#include "helper.h"

using namespace Poincare;

void assert_reduces_to_formal_expression(const char * expression, const char * result, Preferences::AngleUnit angleUnit = Radian) {
  assert_parsed_expression_simplify_to(expression, result, User, angleUnit, Metric, Cartesian, ReplaceAllDefinedSymbolsWithDefinition);
}

QUIZ_CASE(poincare_derivative_formal) {
  assert_reduces_to_formal_expression("diff(undef,x,x)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(unreal,x,x)", Unreal::Name());
  assert_reduces_to_formal_expression("diff(inf,x,x)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(1,x,x)", "0");
  assert_reduces_to_formal_expression("diff(π,x,x)", "0");
  assert_reduces_to_formal_expression("diff(y,x,x)", "0");
  assert_reduces_to_formal_expression("diff(x,x,x)", "1");
  assert_reduces_to_formal_expression("diff(x^2,x,x)", "2×x");
  assert_reduces_to_formal_expression("diff((x-1)(x-2)(x-3),x,x)", "3×x^2-12×x+11");
  assert_reduces_to_formal_expression("diff(√(x),x,x)", "1/\u00122×√(x)\u0013");
  assert_reduces_to_formal_expression("diff(1/x,x,x)", "-1/x^2");
  assert_reduces_to_formal_expression("diff(ℯ^x,x,x)", "ℯ^x");
  assert_reduces_to_formal_expression("diff(2^x,x,x)", "2^x×ln(2)");
  assert_reduces_to_formal_expression("diff(ln(x),x,x)", "1/x");
  assert_reduces_to_formal_expression("diff(log(x),x,x)", "1/\u0012x×ln(5)+x×ln(2)\u0013");
  assert_reduces_to_formal_expression("diff(sin(x),x,x)", "cos(x)");
  assert_reduces_to_formal_expression("diff(sin(x),x,x)", "\u0012π×cos(x)\u0013/180", Degree);
  assert_reduces_to_formal_expression("diff(cos(x),x,x)", "-sin(x)");
  assert_reduces_to_formal_expression("diff(cos(x),x,x)", "-\u0012π×sin(x)\u0013/200", Gradian);
  assert_reduces_to_formal_expression("diff(tan(x),x,x)", "1/cos(x)^2");
  assert_reduces_to_formal_expression("diff(tan(x),x,x)", "π/\u0012180×cos(x)^2\u0013", Degree);
  assert_reduces_to_formal_expression("diff(sinh(x),x,x)", "cosh(x)");
  assert_reduces_to_formal_expression("diff(cosh(x),x,x)", "sinh(x)");
  assert_reduces_to_formal_expression("diff(tanh(x),x,x)", "1/cosh(x)^2");
  assert_reduces_to_formal_expression("diff(sin(x)^2,x,x)", "2×sin(x)×cos(x)");
  assert_reduces_to_formal_expression("diff(diff(x^3,x,x),x,x)", "6×x");
  assert_reduces_to_formal_expression("diff(sinh(sin(y)),x,x)", "0");

  assert_reduce("2→a");
  assert_reduce("-1→b");
  assert_reduce("3→c");
  assert_reduce("x/2→f");

  assert_reduces_to_formal_expression("diff(a×x^2+b×x+c,x,x)", "4×x-1");
  assert_reduces_to_formal_expression("diff(f,x,x)", "1/2");
  assert_reduces_to_formal_expression("diff(a^2,a,x)", "2×x");
  assert_reduces_to_formal_expression("diff(a^2,a,a)", "4");

  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("c.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.exp").destroy();

}

void assert_reduces_for_approximation(const char * expression, const char * result, Preferences::AngleUnit angleUnit = Radian) {
  assert_parsed_expression_simplify_to(expression, result, SystemForApproximation, angleUnit, Metric, Real, ReplaceAllSymbolsWithDefinitionsOrUndefined);
}

QUIZ_CASE(poincare_derivative_approximation) {
  assert_reduces_for_approximation("diff(ln(x),x,1)", "1");
  assert_reduces_for_approximation("diff(ln(x),x,2.2)", "5/11");
  assert_reduces_for_approximation("diff(ln(x),x,0)", Undefined::Name());
  assert_reduces_for_approximation("diff(ln(x),x,-3.1)", Unreal::Name());
  assert_reduces_for_approximation("diff(log(x),x,-10)", Unreal::Name());

  assert_reduces_for_approximation("diff(abs(x),x,123)", "1");
  assert_reduces_for_approximation("diff(abs(x),x,-2.34)", "-1");
  assert_reduces_for_approximation("diff(abs(x),x,0)", Undefined::Name());

  assert_reduces_for_approximation("diff(1/x,x,-2)", "-1/4");
}
