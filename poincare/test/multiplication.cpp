#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_multiplication_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(2.0f)};
  assert_parsed_expression_evaluates_to("1*2", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(11.0, 7.0)};
  assert_parsed_expression_evaluates_to("(3+I)*(4+I)", b);

#if MATRICES_ARE_DEFINED
  Complex<float> c[6] = {Complex<float>::Float(2.0f), Complex<float>::Float(4.0f), Complex<float>::Float(6.0f), Complex<float>::Float(8.0f), Complex<float>::Float(10.0f), Complex<float>::Float(12.0f)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4][5,6]]*2", c, 3, 2);

  Complex<double> d[6] = {Complex<double>::Cartesian(3.0, 1.0), Complex<double>::Cartesian(5.0, 5.0), Complex<double>::Cartesian(9.0, 3.0), Complex<double>::Cartesian(12.0, 4.0), Complex<double>::Cartesian(15.0, 5.0), Complex<double>::Cartesian(18.0, 6.0)};
  assert_parsed_expression_evaluates_to("[[1,2+I][3,4][5,6]]*(3+I)", d, 3, 2);

  assert_parsed_expression_evaluates_to("2*[[1,2][3,4][5,6]]", c, 3, 2);

  assert_parsed_expression_evaluates_to("(3+I)*[[1,2+I][3,4][5,6]]", d, 3, 2);

  Complex<float> e[12] = {Complex<float>::Float(11.0f), Complex<float>::Float(14.0f), Complex<float>::Float(17.0f), Complex<float>::Float(20.0f), Complex<float>::Float(23.0f), Complex<float>::Float(30.0f), Complex<float>::Float(37.0f), Complex<float>::Float(44.0f), Complex<float>::Float(35.0f), Complex<float>::Float(46.0f), Complex<float>::Float(57.0f), Complex<float>::Float(68.0f)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]", e, 3, 4);

  Complex<double> f[12] = {Complex<double>::Cartesian(11.0, 5.0), Complex<double>::Cartesian(13.0, 9.0), Complex<double>::Cartesian(17.0, 7.0), Complex<double>::Cartesian(20.0, 8.0), Complex<double>::Cartesian(23.0, 0.0), Complex<double>::Cartesian(30.0, 7.0), Complex<double>::Cartesian(37.0, 0.0), Complex<double>::Cartesian(44.0, 0.0), Complex<double>::Cartesian(35.0, 0.0), Complex<double>::Cartesian(46.0, 11.0), Complex<double>::Cartesian(57.0, 0.0), Complex<double>::Cartesian(68.0, 0.0)};
  assert_parsed_expression_evaluates_to("[[1,2+I][3,4][5,6]]*[[1,2+I,3,4][5,6+I,7,8]]", f, 3, 4);
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
