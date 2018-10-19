#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_logarithm_evaluate) {
  assert_parsed_expression_evaluates_to<float>("log(2,64)", "0.1666667");
  assert_parsed_expression_evaluates_to<double>("log(6,7)", "0.9207822211616");
  assert_parsed_expression_evaluates_to<float>("log(5)", "0.69897");
  assert_parsed_expression_evaluates_to<double>("ln(5)", "1.6094379124341");
  assert_parsed_expression_evaluates_to<float>("log(2+5*I,64)", "0.4048317+0.2862042*I");
  assert_parsed_expression_evaluates_to<double>("log(6,7+4*I)", "8.0843880717528E-1-2.0108238082167E-1*I");
  assert_parsed_expression_evaluates_to<float>("log(5+2*I)", "0.731199+0.1652518*I");
  assert_parsed_expression_evaluates_to<double>("ln(5+2*I)", "1.6836479149932+3.8050637711236E-1*I");
  assert_parsed_expression_evaluates_to<double>("log(0,0)", "undef");
  assert_parsed_expression_evaluates_to<double>("log(0)", "-inf");
  assert_parsed_expression_evaluates_to<double>("log(2,0)", "0");

  // WARNING: evaluate on branch cut can be multivalued
  assert_parsed_expression_evaluates_to<double>("ln(-4)", "1.3862943611199+3.1415926535898*I");
}

QUIZ_CASE(poincare_logarithm_simplify) {
  assert_parsed_expression_simplify_to("log(0,0)", "undef");
  assert_parsed_expression_simplify_to("log(0,1)", "undef");
  assert_parsed_expression_simplify_to("log(1,0)", "0");
  assert_parsed_expression_simplify_to("log(2,0)", "0");
  assert_parsed_expression_simplify_to("log(0,14)", "-inf");
  assert_parsed_expression_simplify_to("log(0,0.14)", "inf");
  assert_parsed_expression_simplify_to("log(0,0.14+I)", "undef");
  assert_parsed_expression_simplify_to("log(2,1)", "undef");
  assert_parsed_expression_simplify_to("log(x,1)", "undef");
  assert_parsed_expression_simplify_to("log(12925)", "2*log(5)+log(11)+log(47)");
  assert_parsed_expression_simplify_to("ln(12925)", "2*ln(5)+ln(11)+ln(47)");
  assert_parsed_expression_simplify_to("log(1742279/12925, 6)", "(-2*log(5,6))+log(7,6)+3*log(11,6)+log(17,6)-log(47,6)");
  assert_parsed_expression_simplify_to("ln(2/3)", "ln(2)-ln(3)");
  assert_parsed_expression_simplify_to("log(1742279/12925, -6)", "log(158389/1175,-6)");
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

  assert_parsed_expression_simplify_to("log(100)", "2");
  assert_parsed_expression_simplify_to("log(1000000)", "6");
  assert_parsed_expression_simplify_to("log(70992768,14)", "5+2*log(2,14)+log(3,14)+log(11,14)");
  assert_parsed_expression_simplify_to("log(1/6991712,14)", "(-5)-log(13,14)");
  assert_parsed_expression_simplify_to("log(4,10)", "2*log(2)");
}
