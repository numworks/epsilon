#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_power_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(8.0f)};
  assert_parsed_expression_evaluates_to("2^3", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(28.0, 96.0)};
  assert_parsed_expression_evaluates_to("(3+I)^4", b);

  Complex<float> c[1] = {Complex<float>::Cartesian(11.7412464f, 62.9137754f)};
  assert_parsed_expression_evaluates_to("4^(3+I)", c);

#if MATRICES_ARE_DEFINED
  Complex<double> d[4] = {Complex<double>::Float(37.0), Complex<double>::Float(54.0), Complex<double>::Float(81.0), Complex<double>::Float(118.0)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4]]^3", d, 2, 2);
#endif

  Complex<float> e[1] = {Complex<float>::Float(0.0f)};
  assert_parsed_expression_evaluates_to("0^2", e);

  Complex<double> f[1] = {Complex<double>::Float(std::exp(-M_PI_2))};
  assert_parsed_expression_evaluates_to("I^I", f);
}

QUIZ_CASE(poincare_power_simplify) {
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("1256^(1/3)*x", "2*root(157,3)*x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "1/(2*root(157,3))");
  assert_parsed_expression_simplify_to("32^(-1/5)", "1/2");
  assert_parsed_expression_simplify_to("(2+3-4)^(x)", "1");
  assert_parsed_expression_simplify_to("1^x", "1");
  assert_parsed_expression_simplify_to("x^1", "x");
  assert_parsed_expression_simplify_to("0^3", "0");
  assert_parsed_expression_simplify_to("0^0", "undef");
  assert_parsed_expression_simplify_to("0^(-3)", "undef");
  assert_parsed_expression_simplify_to("4^0.5", "2");
  assert_parsed_expression_simplify_to("8^0.5", "2*R(2)");
  assert_parsed_expression_simplify_to("(12^4*3)^(0.5)", "144*R(3)");
  assert_parsed_expression_simplify_to("(2^A)^B", "2^(A*B)");
  assert_parsed_expression_simplify_to("(2*A)^B", "2^B*A^B");
  assert_parsed_expression_simplify_to("(12^4*x)^(0.5)", "144*R(x)");
  assert_parsed_expression_simplify_to("R(32)", "4*R(2)");
  assert_parsed_expression_simplify_to("R(3^2)", "3");
  assert_parsed_expression_simplify_to("2^(2+P)", "4*2^P");
  assert_parsed_expression_simplify_to("R(5513219850886344455940081)", "2348024669991");
  assert_parsed_expression_simplify_to("R(154355776)", "12424");
  assert_parsed_expression_simplify_to("R(P)^2", "P");
  assert_parsed_expression_simplify_to("R(P^2)", "P");
  assert_parsed_expression_simplify_to("R((-P)^2)", "P");
  assert_parsed_expression_simplify_to("R(x*144)", "12*R(x)");
  assert_parsed_expression_simplify_to("R(x*144*P^2)", "12*R(x)*P");
  assert_parsed_expression_simplify_to("R(x*144*P)", "12*R(x)*R(P)");
  assert_parsed_expression_simplify_to("x^(1/2)", "R(x)");
  assert_parsed_expression_simplify_to("x^(-1/2)", "1/R(x)");
  assert_parsed_expression_simplify_to("x^(1/7)", "root(x,7)");
  assert_parsed_expression_simplify_to("x^(-1/7)", "1/root(x,7)");
  assert_parsed_expression_simplify_to("1/(3R(2))", "R(2)/6");
  assert_parsed_expression_simplify_to("X^ln(3)", "3");
  assert_parsed_expression_simplify_to("X^ln(R(3))", "R(3)");
  assert_parsed_expression_simplify_to("P^log(R(3),P)", "R(3)");
  assert_parsed_expression_simplify_to("10^log(P)", "P");
  assert_parsed_expression_simplify_to("X^ln(65)", "65");
  assert_parsed_expression_simplify_to("X^ln(PX)", "P*X");
  assert_parsed_expression_simplify_to("X^log(PX)", "X^(log(P)+log(X))");
  assert_parsed_expression_simplify_to("R(X^2)", "X");
  assert_parsed_expression_simplify_to("999^(10000/3)", "999^(10000/3)");
  /* This does not reduce but should not as the integer is above
   * k_maxNumberOfPrimeFactors and thus it prime decomposition might overflow
   * 32 factors. */
  assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "root(1881676377434183981909562699940347954480361860897069,3)");
  /* This does not reduce but should not as the prime decomposition involves
   * factors above k_maxNumberOfPrimeFactors. */
  assert_parsed_expression_simplify_to("1002101470343^(1/3)", "root(1002101470343,3)");
  assert_parsed_expression_simplify_to("(x+P)^(3)", "x^3+3*x^2*P+3*x*P^2+P^3");
  assert_parsed_expression_simplify_to("(5+R(2))^(-8)", "(1446241-1003320*R(2))/78310985281");
  assert_parsed_expression_simplify_to("(5*P+R(2))^(-5)", "1/(4*R(2)+100*P+500*R(2)*P^2+2500*P^3+3125*R(2)*P^4+3125*P^5)");
  assert_parsed_expression_simplify_to("(1+R(2)+R(3))^5", "296+224*R(2)+184*R(3)+120*R(6)");
  assert_parsed_expression_simplify_to("(P+R(2)+R(3)+x)^(-3)", "1/(11*R(2)+9*R(3)+15*x+6*R(6)*x+3*R(2)*x^2+3*R(3)*x^2+x^3+15*P+6*R(6)*P+6*R(2)*x*P+6*R(3)*x*P+3*x^2*P+3*R(2)*P^2+3*R(3)*P^2+3*x*P^2+P^3)");
}
