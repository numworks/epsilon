#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/test/helper.h>

#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_approximation_lists_functions) {
  assert_expression_approximates_to<double>("sort({-1,i,8,-0})", "{-1,i,8,0}");
  assert_expression_approximates_to<double>("sort({(8,1),(5,i),(5,-3)})",
                                            "{(8,1),(5,undef),(5,-3)}");
  assert_expression_approximates_to<double>("sort({(undef,1),(6,1),(5,-3)})",
                                            "{(undef,1),(6,1),(5,-3)}");
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
