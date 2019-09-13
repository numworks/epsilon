#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_subtraction_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1-2", "-1");
  assert_parsed_expression_evaluates_to<double>("3+𝐢-(4+𝐢)", "-1");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]-3", "[[-2,-1][0,1][2,3]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+𝐢][3,4][5,6]]-(4+𝐢)", "[[-3-𝐢,-2][-1-𝐢,-𝐢][1-𝐢,2-𝐢]]");
  assert_parsed_expression_evaluates_to<float>("3-[[1,2][3,4][5,6]]", "[[2,1][0,-1][-2,-3]]");
  assert_parsed_expression_evaluates_to<double>("3+𝐢-[[1,2+𝐢][3,4][5,6]]", "[[2+𝐢,1][𝐢,-1+𝐢][-2+𝐢,-3+𝐢]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]", "[[-5,-3][-1,1][3,5]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+𝐢][3,4][5,6]]-[[1,2+𝐢][3,4][5,6]]", "[[0,0][0,0][0,0]]");
#endif
}
