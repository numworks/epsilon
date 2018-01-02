#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;

QUIZ_CASE(poincare_simplify_mix) {

  // Root at denominator
  assert_parsed_expression_simplify_to("1/(R(2)+R(3))", "(-R(2))+R(3)");
  assert_parsed_expression_simplify_to("1/(5+R(3))", "(5-R(3))/22");
  assert_parsed_expression_simplify_to("1/(R(2)+4)", "(4-R(2))/14");
  assert_parsed_expression_simplify_to("1/(2R(2)-4)", "((-2)-R(2))/4");
  assert_parsed_expression_simplify_to("1/(-2R(2)+4)", "(2+R(2))/4");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1/(R(2)ln(3))", "R(2)/(2*ln(3))");
  assert_parsed_expression_simplify_to("R(3/2)", "R(6)/2");

  // Common operation mix
  assert_parsed_expression_simplify_to("(R(2)*P + R(2)*X)/R(2)", "P+X");
  assert_parsed_expression_simplify_to("P+(3R(2)-2R(3))/25", "(3*R(2)-2*R(3)+25*P)/25");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to("3*A*B*C+4*cos(2)-2*A*B*C+A*B*C+ln(3)+4*A*B-5*A*B*C+cos(3)*ln(5)+cos(2)-45*cos(2)", "(-40*cos(2))+ln(3)+cos(3)*ln(5)+4*A*B-3*A*B*C");
  assert_parsed_expression_simplify_to("2*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1+0", "4+3*cos(2)+6*ln(5)+7*A");
  assert_parsed_expression_simplify_to("2.3*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1.2+0.235", "(887+600*cos(2)+1200*ln(5)+1460*A)/200");
  assert_parsed_expression_simplify_to("2*A*B*C+2.3*A*B+3*A^2+5.2*A*B*C+4*A^2", "(70*A^2+23*A*B+72*A*B*C)/10");
  assert_parsed_expression_simplify_to("(A*B)^2*A+4*A^3", "4*A^3+A^3*B^2");
  assert_parsed_expression_simplify_to("(A*3)^2*A+4*A^3", "13*A^3");
  assert_parsed_expression_simplify_to("A^2^2*A+4*A^3", "4*A^3+A^5");
  assert_parsed_expression_simplify_to("0.5+4*A*B-2.3+2*A*B-2*B*A^C-cos(4)+2*A^C*B+A*B*C*D", "((-9)-5*cos(4)+30*A*B+5*A*B*C*D)/5");
  assert_parsed_expression_simplify_to("(1+R(2))/5", "(1+R(2))/5");
  assert_parsed_expression_simplify_to("(2+R(6))^2", "10+4*R(6)");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+P", "tan(3)*ln(2)+P");


  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), cos(9))", "ln(2)+ln(3)"); // TODO: for this to work, we must know the sign of cos(9)
  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), 9)", "ln(6)*log(cos(9), 9)"); // TODO: for this to work, we must know the sign of cos(9)
  assert_parsed_expression_simplify_to("(((R(6)-R(2))/4)/((R(6)+R(2))/4))+1", "3-R(3)");
  //assert_parsed_expression_simplify_to("1/R(I) * (R(2)-I*R(2))", "-2I"); // TODO: get rid of complex at denominator?

}
