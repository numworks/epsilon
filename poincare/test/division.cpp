#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_division_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1/2", "0.5");
  assert_parsed_expression_evaluates_to<double>("(3+I)/(4+I)", "7.6470588235294E-1+5.8823529411765E-2*I");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+I][3,4][5,6]]/(1+I)", "[[0.5-0.5*I,1.5-0.5*I][1.5-1.5*I,2-2*I][2.5-2.5*I,3-3*I]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]/2", "[[0.5,1][1.5,2][2.5,3]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2][3,4]]/[[3,4][6,9]]", "[[-1,6.6666666666667E-1][1,0]]");
  assert_parsed_expression_evaluates_to<double>("3/[[3,4][5,6]]", "[[-9,6][7.5,-4.5]]");
  // TODO: add this test when inverse of complex matrix is implemented
  //assert_parsed_expression_evaluates_to<double>("(3+4i)/[[1,2+i][3,4][5,6]]", "[[(-9)-12*I,6+8*I][7/5+10*I,(-4.5)-6*I]]");
#endif
  assert_parsed_expression_evaluates_to<float>("1E20/(1E20+1E20I)", "0.5-0.5*I");
  assert_parsed_expression_evaluates_to<double>("1E155/(1E155+1E155I)", "0.5-0.5*I");
}
