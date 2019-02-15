#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_power_evaluate) {
  assert_parsed_expression_evaluates_to<float>("2^3", "8");
  assert_parsed_expression_evaluates_to<double>("(3+I)^4", "28+96*I");
  assert_parsed_expression_evaluates_to<float>("4^(3+I)", "11.74125+62.91378*I");
  assert_parsed_expression_evaluates_to<double>("(3+I)^(3+I)", "-11.898191759852+19.592921596609*I");

  assert_parsed_expression_evaluates_to<double>("0^0", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("0^2", "0");
  assert_parsed_expression_evaluates_to<double>("0^(-2)", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("(-2)^4.2", "14.8690638497+10.8030072384*I", System, Radian, Cartesian, 12);
  assert_parsed_expression_evaluates_to<double>("(-0.1)^4", "0.0001", System, Radian, Cartesian, 12);
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4]]^(-3)", "[[-14.75,6.75][10.125,-4.625]]", System, Degree, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("[[1,2][3,4]]^3", "[[37,54][81,118]]");
#endif
  assert_parsed_expression_evaluates_to<float>("0^2", "0");
  assert_parsed_expression_evaluates_to<double>("I^I", "2.0787957635076E-1");
  assert_parsed_expression_evaluates_to<float>("1.0066666666667^60", "1.48985", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("1.0066666666667^60", "1.4898457083046");
  assert_parsed_expression_evaluates_to<float>("X^(I*P)", "-1");
  assert_parsed_expression_evaluates_to<double>("X^(I*P)", "-1");
  assert_parsed_expression_evaluates_to<float>("X^(I*P+2)", "-7.38906", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("X^(I*P+2)", "-7.3890560989307");
  assert_parsed_expression_evaluates_to<float>("(-1)^(1/3)", "0.5+0.8660254*I");
  assert_parsed_expression_evaluates_to<double>("(-1)^(1/3)", "0.5+8.6602540378444E-1*I");
  assert_parsed_expression_evaluates_to<float>("X^(I*P/3)", "0.5+0.8660254*I");
  assert_parsed_expression_evaluates_to<double>("X^(I*P/3)", "0.5+8.6602540378444E-1*I");
  assert_parsed_expression_evaluates_to<float>("I^(2/3)", "0.5+0.8660254*I");
  assert_parsed_expression_evaluates_to<double>("I^(2/3)", "0.5+8.6602540378444E-1*I");
}

QUIZ_CASE(poincare_power_simplify) {
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("1256^(1/3)*x", "2*root(157,3)*x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "1/(2*root(157,3))");
  assert_parsed_expression_simplify_to("32^(-1/5)", "1/2");
  assert_parsed_expression_simplify_to("(2+3-4)^(x)", "1");
  assert_parsed_expression_simplify_to("1^x", "1");
  assert_parsed_expression_simplify_to("x^1", "x");
  assert_parsed_expression_simplify_to("0^3", "0");
  assert_parsed_expression_simplify_to("0^0", Undefined::Name());
  assert_parsed_expression_simplify_to("0^(-3)", Undefined::Name());
  assert_parsed_expression_simplify_to("4^0.5", "2");
  assert_parsed_expression_simplify_to("8^0.5", "2*R(2)");
  assert_parsed_expression_simplify_to("(12^4*3)^(0.5)", "144*R(3)");
  assert_parsed_expression_simplify_to("(2^A)^B", "2^(A*B)");
  assert_parsed_expression_simplify_to("(2*A)^B", "2^B*A^B");
  assert_parsed_expression_simplify_to("(12^4*x)^(0.5)", "144*R(x)");
  assert_parsed_expression_simplify_to("R(32)", "4*R(2)");
  assert_parsed_expression_simplify_to("R(-1)", "I");
  assert_parsed_expression_simplify_to("R(-1*R(-1))", "R(2)/2-R(2)/2*I");
  assert_parsed_expression_simplify_to("R(3^2)", "3");
  assert_parsed_expression_simplify_to("2^(2+P)", "4*2^P");
  assert_parsed_expression_simplify_to("R(5513219850886344455940081)", "2348024669991");
  assert_parsed_expression_simplify_to("R(154355776)", "12424");
  assert_parsed_expression_simplify_to("R(P)^2", "P");
  assert_parsed_expression_simplify_to("R(P^2)", "P");
  assert_parsed_expression_simplify_to("R((-P)^2)", "P");
  assert_parsed_expression_simplify_to("R(x*144)", "12*R(x)");
  assert_parsed_expression_simplify_to("R(x*144*P^2)", "12*P*R(x)");
  assert_parsed_expression_simplify_to("R(x*144*P)", "12*R(P)*R(x)");
  assert_parsed_expression_simplify_to("(-1)*(2+(-4*R(2)))", "4*R(2)-2");
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
  assert_parsed_expression_simplify_to("X^log(PX)", "X^(log(X)+log(P))");
  assert_parsed_expression_simplify_to("R(X^2)", "X");
  assert_parsed_expression_simplify_to("999^(10000/3)", "999^(10000/3)");
  /* This does not reduce but should not as the integer is above
   * k_maxNumberOfPrimeFactors and thus it prime decomposition might overflow
   * 32 factors. */
  assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "root(1881676377434183981909562699940347954480361860897069,3)");
  /* This does not reduce but should not as the prime decomposition involves
   * factors above k_maxNumberOfPrimeFactors. */
  assert_parsed_expression_simplify_to("1002101470343^(1/3)", "root(1002101470343,3)");
  assert_parsed_expression_simplify_to("P*P*P", "P^3");
  assert_parsed_expression_simplify_to("(x+P)^(3)", "x^3+3*P*x^2+3*P^2*x+P^3");
  assert_parsed_expression_simplify_to("(5+R(2))^(-8)", "(-1003320*R(2)+1446241)/78310985281");
  assert_parsed_expression_simplify_to("(5*P+R(2))^(-5)", "1/(3125*P^5+3125*R(2)*P^4+2500*P^3+500*R(2)*P^2+100*P+4*R(2))");
  assert_parsed_expression_simplify_to("(1+R(2)+R(3))^5", "120*R(6)+184*R(3)+224*R(2)+296");
  assert_parsed_expression_simplify_to("(P+R(2)+R(3)+x)^(-3)", "1/(x^3+3*P*x^2+3*R(3)*x^2+3*R(2)*x^2+3*P^2*x+6*R(3)*P*x+6*R(2)*P*x+6*R(6)*x+15*x+P^3+3*R(3)*P^2+3*R(2)*P^2+6*R(6)*P+15*P+9*R(3)+11*R(2))");
  assert_parsed_expression_simplify_to("1.0066666666667^60", "(10066666666667/10000000000000)^60");
  assert_parsed_expression_simplify_to("2^(6+P+x)", "64*2^(x+P)");
  assert_parsed_expression_simplify_to("I^(2/3)", "1/2+R(3)/2*I");
  assert_parsed_expression_simplify_to("X^(I*P/3)", "1/2+R(3)/2*I");
  assert_parsed_expression_simplify_to("(-1)^(1/3)", "1/2+R(3)/2*I");
  assert_parsed_expression_simplify_to("R(-x)", "R(-x)");
  assert_parsed_expression_simplify_to("R(x)^2", "x", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("R(x)^2", "R(x)^2", User, Radian, Real);
}
