#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/test/helper.h>

#include "helper.h"

using namespace Poincare;

template <typename T>
void assert_float_approximates_to(Expression f, const char *result) {
#if 0  // TODO_PCJ
  Shared::GlobalContext globalContext;
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  char buffer[500];
  f.approximate<T>(ApproximationContext(&globalContext, Cartesian, Radian))
      .serialize(buffer, sizeof(buffer), DecimalMode, numberOfDigits);
  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
#endif
}

QUIZ_CASE(poincare_approximation_float) {
  assert_float_approximates_to<double>(
      Expression::Builder<double>(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(1.23456789E30), "1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_approximates_to<double>(Expression::Builder<double>(-1.2345E-3),
                                       "-0.0012345");
  assert_float_approximates_to<double>(Expression::Builder<double>(1.2345E-3),
                                       "0.0012345");
  assert_float_approximates_to<double>(Expression::Builder<double>(1.2345E3),
                                       "1234.5");
  assert_float_approximates_to<double>(Expression::Builder<double>(-1.2345E3),
                                       "-1234.5");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(0.99999999999995), "0.99999999999995");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(0.00000099999999999995),
      "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(
      Expression::Builder<double>(
          0.0000009999999999901200121020102010201201201021099995),
      "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(Expression::Builder<float>(1.2345E-1),
                                      "0.12345");
  assert_float_approximates_to<float>(Expression::Builder<float>(1), "1");
  assert_float_approximates_to<float>(
      Expression::Builder<float>(0.9999999999999995), "1");
  assert_float_approximates_to<float>(Expression::Builder<float>(1.2345E6),
                                      "1234500");
  assert_float_approximates_to<float>(Expression::Builder<float>(-1.2345E6),
                                      "-1234500");
  assert_float_approximates_to<float>(
      Expression::Builder<float>(0.0000009999999999999995), "1ᴇ-6");
  assert_float_approximates_to<float>(Expression::Builder<float>(-1.2345E-1),
                                      "-0.12345");

  assert_float_approximates_to<double>(Expression::Builder<double>(INFINITY),
                                       "∞");
  assert_float_approximates_to<float>(Expression::Builder<float>(0.0f), "0");
  assert_float_approximates_to<float>(Expression::Builder<float>(NAN), "undef");
}

template <typename T>
void assert_expression_approximation_is_bounded(const char *expression,
                                                T lowBound, T upBound,
                                                bool upBoundIncluded = false) {
  // TODO_PCJ
#if 0
  Shared::GlobalContext globalContext;
  Internal::Tree *e = parse_expression(expression, &globalContext, true);
  T result = Internal::Approximation::To<T>(
      e, Internal::Approximation::Parameters{.isRootAndCanHaveRandom = true, .projectLocalVariables=true},
      Internal::Approximation::Context(Radian, Cartesian, &globalContext));
  quiz_assert_print_if_failure(result >= lowBound, expression);
  quiz_assert_print_if_failure(
      result < upBound || (result == upBound && upBoundIncluded), expression);
  e->removeTree();
#endif
}

QUIZ_CASE(poincare_approximation_function) {
  assert_expression_approximation_is_bounded("random()", 0.0f, 1.0f);
  assert_expression_approximation_is_bounded("random()", 0.0, 1.0);

  assert_expression_approximation_is_bounded("randint(4,45)", 4.0f, 45.0f,
                                             true);
  assert_expression_approximation_is_bounded("randint(4,45)", 4.0, 45.0, true);
  assert_expression_approximation_is_bounded("randint(1, 10)", 1.0, 10.0, true);
  assert_expression_approximation_is_bounded(
      "randint(0, 2^31 - 1)", 0.0, static_cast<double>(0x7FFFFFFF), true);
  assert_expression_approximation_is_bounded(
      "randint(123456789876543, 123456789876543+10)", 123456789876543.0,
      123456789876553.0, true);
  assert_expression_approximation_is_bounded(
      "randint(-123456789876543, 123456789876543)", -123456789876543.0,
      123456789876543.0, true);
  assert_expression_approximates_to<double>("randint({1,1})", "{1,1}");
  assert_expression_approximates_to<double>("randint({1,2},{1,2})", "{1,2}");
  assert_expression_approximates_to<double>("1/randint(2,2)+1/2", "1");
  assert_expression_approximates_to<double>("randint(45,4)", "undef");
  assert_expression_approximates_to<double>("randint(1, inf)", "undef");
  assert_expression_approximates_to<double>("randint(-inf, 3)", "undef");
  assert_expression_approximates_to<double>("randint(4, 3)", "undef");
#if 0  // TODO_PCJ:
  assert_expression_approximates_to<double>("randint(2, 23345678909876545678)",
                                            "undef");
#endif
}

template <typename T>
void assert_no_duplicates_in_list(const char *expression) {
  Shared::GlobalContext globalContext;
  Internal::Tree *e = parse_expression(expression, &globalContext);
  // Sort list
  e->cloneNodeAtNode(Poincare::Internal::KListSort);
  e->moveTreeOverTree(Internal::Approximation::ToTree<T>(
      e,
      Internal::Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                          .projectLocalVariables = true},
      Internal::Approximation::Context(Radian, Cartesian)));
  // Find duplicates
  assert(e->isList());
  int n = e->numberOfChildren();
  bool bad = false;
  for (int i = 1; i < n; i++) {
    if (e->child(i)->treeIsIdenticalTo(e->child(i - 1))) {
      bad = true;
      break;
    }
  }
#if POINCARE_STRICT_TESTS
  quiz_assert_print_if_failure(!bad, expression);
#else
  quiz_tolerate_print_if_failure(!bad, expression, "no duplicates",
                                 "duplicates");
#endif
  e->removeTree();
}

QUIZ_CASE(poincare_approximation_unique_random) {
  assert_expression_approximates_to<double>("randintnorep(10,1,3)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,100)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,-1)", "undef");
  assert_expression_approximates_to<double>("randintnorep(1,10,0)", "{}");
  assert_expression_approximates_to<double>(
      "randintnorep(536427840,-2145711360,4)", "undef");

  assert_no_duplicates_in_list<float>("randintnorep(-100,99,200)");
  assert_no_duplicates_in_list<float>("randintnorep(1234,5678,20)");

  // Random lists can be sorted
  assert_expression_approximates_to<double>("sort(randintnorep(5,8,4))",
                                            "{5,6,7,8}");

  /* The simplification process should understand that the expression is not a
   * scalar if it encounters a randintnorep. */
  assert_expression_simplifies_approximates_to<double>(
      "rem(randintnorep(1,10,5),1)", "{0,0,0,0,0}");
}

QUIZ_CASE(poincare_approximation_lists_functions) {
  assert_expression_approximates_to<double>("sort({-1,i,8,-0})", "{-1,i,8,0}");
  assert_expression_approximates_to<double>("sort({(8,1),(5,i),(5,-3)})",
                                            "{(8,1),(5,undef),(5,-3)}");
  assert_expression_approximates_to<double>("sort({(undef,1),(6,1),(5,-3)})",
                                            "{(undef,1),(6,1),(5,-3)}");
}

template <typename T>
void assert_expression_approximates_with_value_for_symbol(
    const char *expression, T approximation, const char *symbol, T symbolValue,
    Preferences::AngleUnit angleUnit = Degree,
    Preferences::ComplexFormat complexFormat = Cartesian) {
  Shared::GlobalContext globalContext;
#if 0
  Internal::Tree *e = parse_expression(expression, &globalContext);
  ApproximationContext approximationContext(&globalContext, complexFormat,
                                            angleUnit);
  T result = e.approximateToRealScalarWithValueForSymbol<T>(symbol, symbolValue,
                                                        approximationContext);
  quiz_assert_print_if_failure(
      roughly_equal(result, approximation, OMG::Float::EpsilonLax<T>(),
                    true),
      expression);
#endif
}

QUIZ_CASE(poincare_approximation_floor_ceil_integer) {
  constexpr double upperBound = 1000.;
  for (double d = 0.; d < upperBound; d += 1.) {
    assert_expression_approximates_with_value_for_symbol(
        "floor(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
    assert_expression_approximates_with_value_for_symbol(
        "ceil(x * (x+1)^(-1) + x^2 * (x+1)^(-1))", d, "x", d);
  }
}

QUIZ_CASE(poincare_approximation_point) {
  assert_expression_approximates_to<double>("(undef,i)", "(undef,undef)");
  assert_expression_approximates_to<double>("(undef,i)", "(undef,undef)",
                                            Degree, MetricUnitFormat, Real);
}

QUIZ_CASE(poincare_approximation_keeping_symbols) {
  assert_expression_approximates_keeping_symbols_to("ln(10)+cos(10)+3x",
                                                    "3×x+3.287392846");
  assert_expression_approximates_keeping_symbols_to(
      "cos(4/3+ln(x-1/2))", "cos(ln(2×x-1)+0.6401861528)");
  assert_expression_approximates_keeping_symbols_to(
      "ln(ln(ln(10+10)))+ln(ln(ln(x+10)))", "ln(ln(ln(x+10)))+0.09275118142");
  assert_expression_approximates_keeping_symbols_to("int(x,x,0,2)+int(x,x,0,x)",
                                                    "int(x,x,0,x)+2");
  assert_expression_approximates_keeping_symbols_to(
      "[[x,cos(10)][1/2+x,cos(4/3+x)]]",
      "[[x,0.984807753][(2×x+1)/2,cos((3×x+4)/3)]]");
  assert_expression_approximates_keeping_symbols_to(
      "{x,undef,cos(10)+x,cos(10)}", "{x,undef,x+0.984807753,0.984807753}");
  assert_expression_approximates_keeping_symbols_to("cos(10)→x",
                                                    "0.984807753→x");
  assert_expression_approximates_keeping_symbols_to("4×kg×s^(-3)", "undef");
  // TODO_PCJ investigate why this one crashes on the CI only
  // assert_expression_approximates_keeping_symbols_to("piecewise(T×x<0)",
  // "undef");
  // Check that it still reduces
  assert_expression_approximates_keeping_symbols_to("x^2+x×x", "2×x^2");
}
