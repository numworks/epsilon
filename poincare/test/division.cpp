#include <quiz.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_division_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1/2", "0.5");
  assert_parsed_expression_evaluates_to<double>("(3+𝐢)/(4+𝐢)", "7.6470588235294ᴇ-1+5.8823529411765ᴇ-2×𝐢");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+𝐢][3,4][5,6]]/(1+𝐢)", "[[0.5-0.5×𝐢,1.5-0.5×𝐢][1.5-1.5×𝐢,2-2×𝐢][2.5-2.5×𝐢,3-3×𝐢]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", "[[-1,6.6666666666667ᴇ-1][1,0]]");
  assert_parsed_expression_evaluates_to<double>("3/[[3,4][5,6]]", "[[-9,6][7.5,-4.5]]");
  assert_parsed_expression_evaluates_to<double>("(3+4𝐢)/[[1,𝐢][3,4]]", "[[4×𝐢,1][-3×𝐢,𝐢]]");
  assert_parsed_expression_evaluates_to<float>("1ᴇ20/(1ᴇ20+1ᴇ20𝐢)", "0.5-0.5×𝐢");
  assert_parsed_expression_evaluates_to<double>("1ᴇ155/(1ᴇ155+1ᴇ155𝐢)", "0.5-0.5×𝐢");
}
