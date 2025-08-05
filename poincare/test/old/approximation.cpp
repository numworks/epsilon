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
