#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_logarithm_evaluate) {
  Complex<float> a0[1] = {Complex<float>::Float(0.1666666666666666)};
  assert_parsed_expression_evaluates_to("log(2,64)", a0);
  Complex<double> a1[1] = {Complex<double>::Float(0.9207822211616017903187272451)};
  assert_parsed_expression_evaluates_to("log(6,7)", a1);
  Complex<float> a2[1] = {Complex<float>::Float(0.698970004336018804786261105275506973231810118537891458689)};
  assert_parsed_expression_evaluates_to("log(5)", a2);
  Complex<double> a3[1] = {Complex<double>::Float(1.609437912434100374600759333226187639525601354268517721912)};
  assert_parsed_expression_evaluates_to("ln(5)", a3);
}

QUIZ_CASE(poincare_logarithm_simplify) {
  assert_parsed_expression_simplify_to("log(12925)", "2*log(5)+log(11)+log(47)");
  assert_parsed_expression_simplify_to("ln(12925)", "2*ln(5)+ln(11)+ln(47)");
  assert_parsed_expression_simplify_to("log(1742279/12925, 6)", "(-2*log(5,6))+log(7,6)+3*log(11,6)+log(17,6)-log(47,6)");
  assert_parsed_expression_simplify_to("ln(2/3)", "ln(2)-ln(3)");
  assert_parsed_expression_simplify_to("log(1742279/12925, -6)", "undef");
  assert_parsed_expression_simplify_to("ln(R(2))", "ln(2)/2");
  assert_parsed_expression_simplify_to("ln(X^3)", "3");
  assert_parsed_expression_simplify_to("log(10)", "1");
  assert_parsed_expression_simplify_to("log(R(3),R(3))", "1");
  assert_parsed_expression_simplify_to("log(1/R(2))", "-log(2)/2");
  assert_parsed_expression_simplify_to("log(-I)", "log(-I)");
  assert_parsed_expression_simplify_to("ln(X^(IP/7))", "(P*I)/7");
  assert_parsed_expression_simplify_to("log(10^24)", "24");
  assert_parsed_expression_simplify_to("log((23P)^4,23P)", "4");
  assert_parsed_expression_simplify_to("log(10^(2+P))", "2+P");
  assert_parsed_expression_simplify_to("ln(1881676377434183981909562699940347954480361860897069)", "ln(1881676377434183981909562699940347954480361860897069)");
  /* log(1002101470343) does no reduce to 3*log(10007) because it involves
   * prime factors above k_biggestPrimeFactor */
  assert_parsed_expression_simplify_to("log(1002101470343)", "log(1002101470343)");
  assert_parsed_expression_simplify_to("log(64,2)", "6");
  assert_parsed_expression_simplify_to("log(2,64)", "log(2,64)");
  assert_parsed_expression_simplify_to("log(1476225,5)", "2+10*log(3,5)");
}
