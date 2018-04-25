#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_multiplication_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1*2", "2");
  assert_parsed_expression_evaluates_to<double>("(3+I)*(4+I)", "11+7*I");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]*2", "[[2,4][6,8][10,12]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+I][3,4][5,6]]*(3+I)", "[[3+I,5+5*I][9+3*I,12+4*I][15+5*I,18+6*I]]");
  assert_parsed_expression_evaluates_to<float>("2*[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_parsed_expression_evaluates_to<double>("(3+I)*[[1,2+I][3,4][5,6]]", "[[3+I,5+5*I][9+3*I,12+4*I][15+5*I,18+6*I]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]", "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+I][3,4][5,6]]*[[1,2+I,3,4][5,6+I,7,8]]", "[[11+5*I,13+9*I,17+7*I,20+8*I][23,30+7*I,37,44][35,46+11*I,57,68]]");
#endif
}

QUIZ_CASE(poincare_multiplication_simplify) {
  assert_parsed_expression_simplify_to("0*x+B", "B");
  assert_parsed_expression_simplify_to("0*x*0*32*cos(3)", "0");
  assert_parsed_expression_simplify_to("3*A^4*B^x*B^2*(A^2+2)*2*1.2", "(72*A^4*B^(2+x)+36*A^6*B^(2+x))/5");
  assert_parsed_expression_simplify_to("A*(B+C)*(D+3)", "3*A*B+3*A*C+A*B*D+A*C*D");
  assert_parsed_expression_simplify_to("A/B", "A/B");
  assert_parsed_expression_simplify_to("(A*B)^2", "A^2*B^2");
  assert_parsed_expression_simplify_to("(1/2)*A/B", "A/(2*B)");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6", "21");
  assert_parsed_expression_simplify_to("1-2+3-4+5-6", "-3");
  assert_parsed_expression_simplify_to("987654321123456789*998877665544332211", "986545842648570754445552922919330479");
  assert_parsed_expression_simplify_to("2/3", "2/3");
  assert_parsed_expression_simplify_to("9/17+5/4", "121/68");
  assert_parsed_expression_simplify_to("1/2*3/4", "3/8");
  assert_parsed_expression_simplify_to("0*2/3", "0");
  assert_parsed_expression_simplify_to("1+(1/(1+1/(1+1/(1+1))))", "8/5");
  assert_parsed_expression_simplify_to("1+2/(3+4/(5+6/(7+8)))", "155/101");
  assert_parsed_expression_simplify_to("3/4*16/12", "1");
  assert_parsed_expression_simplify_to("3/4*(8+8)/12", "1");
  assert_parsed_expression_simplify_to("916791/794976477", "305597/264992159");
  assert_parsed_expression_simplify_to("321654987123456789/112233445566778899", "3249040273974311/1133671167341201");
  assert_parsed_expression_simplify_to("0.1+0.2", "3/10");
  assert_parsed_expression_simplify_to("2^3", "8");
  assert_parsed_expression_simplify_to("(-1)*(-1)", "1");
  assert_parsed_expression_simplify_to("(-2)^2", "4");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("(1/2)^-1", "2");
  assert_parsed_expression_simplify_to("R(2)*R(3)", "R(6)");
  assert_parsed_expression_simplify_to("2*2^P", "2*2^P");
  assert_parsed_expression_simplify_to("A^3*B*A^(-3)", "B");
  assert_parsed_expression_simplify_to("A^3*A^(-3)", "1");
  assert_parsed_expression_simplify_to("2^P*(1/2)^P", "1");
  assert_parsed_expression_simplify_to("A^3*A^(-3)", "1");
  assert_parsed_expression_simplify_to("(x+1)*(x+2)", "2+3*x+x^2");
  assert_parsed_expression_simplify_to("(x+1)*(x-1)", "(-1)+x^2");
  assert_parsed_expression_simplify_to("11P/(22P+11P)", "1/3");
  assert_parsed_expression_simplify_to("11/(22P+11P)", "1/(3*P)");
  assert_parsed_expression_simplify_to("-11/(22P+11P)", "-1/(3*P)");
  assert_parsed_expression_simplify_to("A^2*BA^(-2)*B^(-2)", "1/B");
  assert_parsed_expression_simplify_to("A^(-1)*B^(-1)", "1/(A*B)");
  assert_parsed_expression_simplify_to("x+x", "2*x");
  assert_parsed_expression_simplify_to("2*x+x", "3*x");
  assert_parsed_expression_simplify_to("x*2+x", "3*x");
  assert_parsed_expression_simplify_to("2*x+2*x", "4*x");
  assert_parsed_expression_simplify_to("x*2+2*n", "2*n+2*x");
  assert_parsed_expression_simplify_to("x+x+n+n", "2*n+2*x");
  assert_parsed_expression_simplify_to("x-x-n+n", "0");
  assert_parsed_expression_simplify_to("x+n-x-n", "0");
  assert_parsed_expression_simplify_to("x-x", "0");
  assert_parsed_expression_simplify_to("P*3^(1/2)*(5P)^(1/2)*(4/5)^(1/2)", "2*R(3)*P^(3/2)");
  assert_parsed_expression_simplify_to("12^(1/4)*(P/6)*(12*P)^(1/4)", "(R(3)*P^(5/4))/3");
}
