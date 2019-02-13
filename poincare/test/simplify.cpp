#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;
QUIZ_CASE(poincare_reduction_target) {
  assert_parsed_expression_simplify_to("1/P+1/x", "1/x+1/P", System);
  assert_parsed_expression_simplify_to("1/P+1/x", "(x+P)/(P*x)", User);

  assert_parsed_expression_simplify_to("1/(1+I)", "1/(I+1)", System);
  assert_parsed_expression_simplify_to("1/(1+I)", "1/2-1/2*I", User);

  assert_parsed_expression_simplify_to("sin(x)/(cos(x)*cos(x))", "sin(x)/cos(x)^2", System);
  assert_parsed_expression_simplify_to("sin(x)/(cos(x)*cos(x))", "tan(x)/cos(x)", User);

  assert_parsed_expression_simplify_to("x^0", "x^0", System);
  assert_parsed_expression_simplify_to("x^0", "1", User);

  assert_parsed_expression_simplify_to("x^(2/3)", "root(x,3)^2", System);
  assert_parsed_expression_simplify_to("x^(2/3)", "x^(2/3)", User);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", User);

  assert_parsed_expression_simplify_to("1/(R(2)+R(3))", "1/(R(3)+R(2))", System);
  assert_parsed_expression_simplify_to("1/(R(2)+R(3))", "R(3)-R(2)", User);

  assert_parsed_expression_simplify_to("sign(abs(x))", "sign(abs(x))", System);
  assert_parsed_expression_simplify_to("sign(abs(x))", "1", User);

  assert_parsed_expression_simplify_to("atan(1/x)", "atan(1/x)", System);
  assert_parsed_expression_simplify_to("atan(1/x)", "(P*sign(x)-2*atan(x))/2", User);

  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "(x+1)^0", System);
  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "1", User);
}

QUIZ_CASE(poincare_simplify_mix) {
  // Root at denominator
  assert_parsed_expression_simplify_to("1/(R(2)+R(3))", "R(3)-R(2)");
  assert_parsed_expression_simplify_to("1/(5+R(3))", "(-R(3)+5)/22");
  assert_parsed_expression_simplify_to("1/(R(2)+4)", "(-R(2)+4)/14");
  assert_parsed_expression_simplify_to("1/(2R(2)-4)", "(-R(2)-2)/4");
  assert_parsed_expression_simplify_to("1/(-2R(2)+4)", "(R(2)+2)/4");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1/(R(2)ln(3))", "R(2)/(2*ln(3))");
  assert_parsed_expression_simplify_to("R(3/2)", "R(6)/2");

  // Common operation mix
  assert_parsed_expression_simplify_to("(R(2)*P + R(2)*X)/R(2)", "X+P");
  assert_parsed_expression_simplify_to("P+(3R(2)-2R(3))/25", "(25*P-2*R(3)+3*R(2))/25");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to("3*A*B*C+4*cos(2)-2*A*B*C+A*B*C+ln(3)+4*A*B-5*A*B*C+cos(3)*ln(5)+cos(2)-45*cos(2)", "cos(3)*ln(5)+ln(3)-40*cos(2)+4*A*B-3*A*B*C");
  assert_parsed_expression_simplify_to("2*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1+0", "6*ln(5)+3*cos(2)+7*A+4");
  assert_parsed_expression_simplify_to("2.3*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1.2+0.235", "(1200*ln(5)+600*cos(2)+1460*A+887)/200");
  assert_parsed_expression_simplify_to("2*A*B*C+2.3*A*B+3*A^2+5.2*A*B*C+4*A^2", "(70*A^2+23*A*B+72*A*B*C)/10");
  assert_parsed_expression_simplify_to("(A*B)^2*A+4*A^3", "4*A^3+A^3*B^2");
  assert_parsed_expression_simplify_to("(A*3)^2*A+4*A^3", "13*A^3");
  assert_parsed_expression_simplify_to("A^2^2*A+4*A^3", "A^5+4*A^3");
  assert_parsed_expression_simplify_to("0.5+4*A*B-2.3+2*A*B-2*B*A^C-cos(4)+2*A^C*B+A*B*C*D", "(-5*cos(4)+30*A*B+5*A*B*C*D-9)/5");
  assert_parsed_expression_simplify_to("(1+R(2))/5", "(R(2)+1)/5");
  assert_parsed_expression_simplify_to("(2+R(6))^2", "4*R(6)+10");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+P", "tan(3)*ln(2)+P");

  // Complex
  assert_parsed_expression_simplify_to("I", "I");
  assert_parsed_expression_simplify_to("R(-33)", "R(33)*I");
  assert_parsed_expression_simplify_to("I^(3/5)", "(R(2)*R(-R(5)+5))/4+(R(5)+1)/4*I");
  assert_parsed_expression_simplify_to("IIII", "1");
  assert_parsed_expression_simplify_to("R(-I)", "R(2)/2-R(2)/2*I");
  assert_parsed_expression_simplify_to("A*cos(9)IIln(2)", "-A*cos(9)*ln(2)");
  assert_parsed_expression_simplify_to("(R(2)+R(2)*I)/2(R(2)+R(2)*I)/2(R(2)+R(2)*I)/2", "R(2)/32-R(2)/32*I");
  assert_parsed_expression_simplify_to("root(5^((-I)3^9),I)", "1/X^atan(tan(19683*ln(5)))");
  assert_parsed_expression_simplify_to("I^I", "1/X^(P/2)");
  assert_parsed_expression_simplify_to("I/(1+I*R(x))", "I/(R(x)*I+1)");
  assert_parsed_expression_simplify_to("x+I/(1+I*R(x))", "(x^(3/2)*I+I+x)/(R(x)*I+1)");

  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), cos(9))", "ln(2)+ln(3)"); // TODO: for this to work, we must know the sign of cos(9)
  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), 9)", "ln(6)*log(cos(9), 9)"); // TODO: for this to work, we must know the sign of cos(9)
  assert_parsed_expression_simplify_to("(((R(6)-R(2))/4)/((R(6)+R(2))/4))+1", "-R(3)+3");
  //assert_parsed_expression_simplify_to("1/R(I) * (R(2)-I*R(2))", "-2I"); // TODO: get rid of complex at denominator?

}
