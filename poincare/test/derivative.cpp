#include "helper.h"
#include <ion/storage.h>
#include <poincare/derivative.h>
#include <poincare/undefined.h>
#include <poincare/nonreal.h>

using namespace Poincare;

void assert_reduces_to_formal_expression(const char * expression, const char * result, Preferences::AngleUnit angleUnit = Radian, Preferences::ComplexFormat complexFormat = Cartesian) {
  assert_parsed_expression_simplify_to(expression, result, User, angleUnit, MetricUnitFormat, complexFormat, ReplaceAllDefinedSymbolsWithDefinition);
}

QUIZ_CASE(poincare_derivative_formal) {
  assert_reduces_to_formal_expression("diff(undef,x,x)", Undefined::Name());
  assert_reduces_to_formal_expression("diff(nonreal,x,x)", Nonreal::Name());
  assert_reduces_to_formal_expression("diff(inf,x,x)", Undefined::Name());

  assert_reduces_to_formal_expression("diff(1,x,x)", "0");
  assert_reduces_to_formal_expression("diff(π,x,x)", "0");
  assert_reduces_to_formal_expression("diff(y,x,x)", "dep\u0014(0,[[y]])");
  assert_reduces_to_formal_expression("diff(x,x,x)", "dep\u0014(1,[[x]])");
  assert_reduces_to_formal_expression("diff(x^2,x,x)", "dep\u0014(2×x,[[x^2]])");
  assert_reduces_to_formal_expression("diff((x-1)(x-2)(x-3),x,x)", "dep\u0014(3×x^2-12×x+11,[[x^3-6×x^2+11×x-6]])");
  assert_reduces_to_formal_expression("diff(√(x),x,x)", "dep\u0014(1/\u00122×√(x)\u0013,[[√(x)]])");
  assert_reduces_to_formal_expression("diff(1/x,x,x)", "dep\u0014(-1/x^2,[[1/x]])");

  assert_reduces_to_formal_expression("diff(ℯ^x,x,x)", "dep\u0014(ℯ^\u0012x\u0013,[[ℯ^\U00000012x\U00000013]])");
  assert_reduces_to_formal_expression("diff(2^x,x,x)", "dep\u0014(2^x×ln(2),[[2^x]])");
  assert_reduces_to_formal_expression("diff(ln(x),x,x)", "dep\u0014(1/x,[[ln(x)]])");
  assert_reduces_to_formal_expression("diff(log(x),x,x)", "dep\u0014(1/\u0012x×ln(5)+x×ln(2)\u0013,[[log(x)]])");

  assert_reduces_to_formal_expression("diff(sin(x),x,x)", "dep\u0014(cos(x),[[sin(x)]])");
  assert_reduces_to_formal_expression("diff(sin(x),x,x)", "dep\u0014(\u0012π×cos(x)\u0013/180,[[sin(x)]])", Degree);
  assert_reduces_to_formal_expression("diff(cos(x),x,x)", "dep\u0014(-sin(x),[[cos(x)]])");
  assert_reduces_to_formal_expression("diff(cos(x),x,x)", "dep\u0014(-\u0012π×sin(x)\u0013/200,[[cos(x)]])", Gradian);
  assert_reduces_to_formal_expression("diff(tan(x),x,x)", "dep\u0014(1/cos(x)^2,[[tan(x)]])");
  assert_reduces_to_formal_expression("diff(tan(x),x,x)", "dep\u0014(π/\u0012180×cos(x)^2\u0013,[[tan(x)]])", Degree);

  assert_reduces_to_formal_expression("diff(asin(x),x,x)", "dep\u0014(conj(1/√(-x^2+1)),[[asin(x)]])");
  assert_reduces_to_formal_expression("diff(asin(x),x,x)", "dep\u0014(conj(180/\u0012π×√(-x^2+1)\u0013),[[asin(x)]])", Degree);
  assert_reduces_to_formal_expression("diff(acos(x),x,x)", "dep\u0014(conj(-1/√(-x^2+1)),[[acos(x)]])");
  assert_reduces_to_formal_expression("diff(acos(x),x,x)", "dep\u0014(conj(-180/\u0012π×√(-x^2+1)\u0013),[[acos(x)]])", Degree);
  assert_reduces_to_formal_expression("diff(atan(x),x,x)", "dep\u0014(1/\u0012x^2+1\u0013,[[atan(x)]])");
  assert_reduces_to_formal_expression("diff(atan(x),x,x)", "dep\u0014(180/\u0012π×(x^2+1)\u0013,[[atan(x)]])", Degree);
  assert_reduces_to_formal_expression("diff(asec(x),x,x)", "dep\u0014(-conj(-1/√(\u0012x^2-1\u0013/x^2))/x^2,[[acos(1/x)]])");
  assert_reduces_to_formal_expression("diff(acsc(x),x,x)", "dep\u0014(-conj(1/√(\u0012x^2-1\u0013/x^2))/x^2,[[asin(1/x)]])");
  assert_reduces_to_formal_expression("diff(acot(x),x,x)", "dep\u0014(-1/\u0012x^2+1\u0013,[[\u0012π×sign(x)-2×atan(x)\u0013/2]])");

  assert_reduces_to_formal_expression("diff(sinh(x),x,x)", "dep\u0014(cosh(x),[[sinh(x)]])");
  assert_reduces_to_formal_expression("diff(cosh(x),x,x)", "dep\u0014(sinh(x),[[cosh(x)]])");
  assert_reduces_to_formal_expression("diff(tanh(x),x,x)", "dep\u0014(1/cosh(x)^2,[[tanh(x)]])");
  assert_reduces_to_formal_expression("diff(sin(x)^2,x,x)", "dep\u0014(2×sin(x)×cos(x),[[sin(x)^2]])");

  assert_reduces_to_formal_expression("diff(diff(x^3,x,x),x,x)", "dep\u0014(6×x,[[x^3,3×x^2]])");
  assert_reduces_to_formal_expression("diff(sinh(sin(y)),x,x)", "dep\u0014(0,[[sinh(sin(y))]])");

  assert_reduce("2→a");
  assert_reduce("-1→b");
  assert_reduce("3→c");
  assert_reduce("x/2→f(x)");

  assert_reduces_to_formal_expression("diff(a×x^2+b×x+c,x,x)", "dep\u0014(4×x-1,[[2×x^2-x+3]])");
  assert_reduces_to_formal_expression("diff(f(x),x,x)", "dep\u0014(1/2,[[x,x/2]])");
  assert_reduces_to_formal_expression("diff(a^2,a,x)", "dep\u0014(2×x,[[x^2]])");
  assert_reduces_to_formal_expression("diff(a^2,a,a)", "4");
  assert_reduces_to_formal_expression("diff(b^2,b,2)", "4", Radian, Real);

  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("c.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

}

void assert_reduces_for_approximation(const char * expression, const char * result, Preferences::AngleUnit angleUnit = Radian, Poincare::Preferences::ComplexFormat complexFormat = Real) {
  assert_parsed_expression_simplify_to(expression, result, SystemForApproximation, angleUnit, MetricUnitFormat, complexFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined);
}

QUIZ_CASE(poincare_derivative_reduced_approximation) {
  assert(Ion::Storage::sharedStorage()->recordNamed("x.exp").isNull());
  assert_reduces_for_approximation("diff(ln(x),x,1)", "1");
  assert_reduces_for_approximation("diff(ln(x),x,2.2)", "5/11");
  assert_reduces_for_approximation("diff(ln(x),x,0)", Undefined::Name());
  assert_reduces_for_approximation("diff(ln(x),x,-3.1)", Nonreal::Name());
  assert_reduces_for_approximation("diff(log(x),x,-10)", Nonreal::Name());

  assert_reduces_for_approximation("diff(abs(x),x,123)", "1");
  assert_reduces_for_approximation("diff(abs(x),x,-2.34)", "-1");
  assert_reduces_for_approximation("diff(abs(x),x,0)", Undefined::Name());

  assert_reduces_for_approximation("diff(√(x),x,-1)", "-𝐢/2", Radian, Cartesian);
  assert_reduces_for_approximation("diff(1/x,x,-2)", "-1/4");
  assert_reduces_for_approximation("diff(x^3+5*x^2,x,0)", "0");
  assert_reduces_for_approximation("diff(5^(sin(x)),x,3)", "5^sin(3)×cos(3)×ln(5)");
  assert_reduces_for_approximation("diff((-1)^(4-2*2),x,3)", "0");
  assert_reduce("0→a");
  assert_reduces_for_approximation("diff((-1)^(a*x),x,3)", "0");
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

void assert_approximate_to(const char * expression, const char * result, Preferences::AngleUnit angleUnit = Radian) {
  /* Reduce significant numbers to 3 to handle platforms discrepancies when
   * computing floats. This allows to expect the same results from both double
   * and float approximations. */
  assert_expression_approximates_to<float>(expression, result, angleUnit, MetricUnitFormat, Real, 3);
  assert_expression_approximates_to<double>(expression, result, angleUnit, MetricUnitFormat, Real, 3);
}

QUIZ_CASE(poincare_derivative_approximation) {
  assert_approximate_to("diff(2×x, x, 2)", "2");
  assert_approximate_to("diff(2×TO^2, TO, 7)", "28");
  assert_approximate_to("diff(ln(x),x,1)", "1");
  assert_approximate_to("diff(ln(x),x,2.2)", "0.455");
  assert_approximate_to("diff(ln(x),x,0)", Undefined::Name());
  assert_approximate_to("diff(ln(x),x,-3.1)", Nonreal::Name());
  assert_approximate_to("diff(log(x),x,-10)", Nonreal::Name());
  assert_approximate_to("diff(abs(x),x,123)", "1");
  assert_approximate_to("diff(abs(x),x,-2.34)", "-1");
  assert_approximate_to("diff(1/x,x,-2)", "-0.25");
  assert_approximate_to("diff(x^3+5*x^2,x,0)", "0");
  assert_approximate_to("diff(abs(x),x,0)", "0"); // Undefined::Name());
  // FIXME error too big on floats
  // assert_expression_approximates_to<float>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
  assert_expression_approximates_to<double>("diff(-1/3×x^3+6x^2-11x-50,x,11)", "0");
}
