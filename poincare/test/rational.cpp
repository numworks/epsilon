#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_rational_sign) {
  assert(Rational(-2).sign() == Expression::Sign::Negative);
  assert(Rational(-2, 3).sign() == Expression::Sign::Negative);
  assert(Rational(2, 3).sign() == Expression::Sign::Positive);
}

QUIZ_CASE(poincare_rational_compare) {
  assert(Rational::NaturalOrder(Rational(123, 234),Rational(456, 567)) < 0);
  assert(Rational::NaturalOrder(Rational(-123, 234),Rational(456, 567)) < 0);
  assert(Rational::NaturalOrder(Rational(123, 234),Rational(-456, 567)) > 0);
  assert(Rational::NaturalOrder(Rational(123, 234),Rational("123456789123456789", "12345678912345678910")) > 0);
}

QUIZ_CASE(poincare_rational_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(0.333333333f)};
  assert_parsed_expression_evaluates_to("1/3", a);
  Complex<float> b[1] = {Complex<float>::Float(0.099999)};
  assert_parsed_expression_evaluates_to("123456/1234567", b);
}

QUIZ_CASE(poincare_rational_simplify) {
  assert_parsed_expression_simplify_to("-1/3", "-1/3");
  assert_parsed_expression_simplify_to("22355/45325", "4471/9065");
  assert_parsed_expression_simplify_to("0000.000000", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  assert_parsed_expression_simplify_to("0.1234567", "1234567/10000000");
  assert_parsed_expression_simplify_to("123.4567", "1234567/10000");
  assert_parsed_expression_simplify_to("0.1234", "617/5000");
  assert_parsed_expression_simplify_to("0.1234000", "617/5000");
  assert_parsed_expression_simplify_to("001234000", "1234000");
  assert_parsed_expression_simplify_to("001.234000E3", "1234");
  assert_parsed_expression_simplify_to("001234000E-4", "617/5");
  assert_parsed_expression_simplify_to("3/4+5/4-12+1/567", "-5669/567");
  assert_parsed_expression_simplify_to("34/78+67^(-1)", "1178/2613");
  assert_parsed_expression_simplify_to("12348/34564", "3087/8641");
  assert_parsed_expression_simplify_to("1-0.3-0.7", "0");
  assert_parsed_expression_simplify_to("123456789123456789+112233445566778899", "235690234690235688");
  assert_parsed_expression_simplify_to("56^56", "79164324866862966607842406018063254671922245312646690223362402918484170424104310169552592050323456");
  assert_parsed_expression_simplify_to("999^999", "999^999");
  assert_parsed_expression_simplify_to("999^-999", "1/999^999");
  assert_parsed_expression_simplify_to("0^0", "undef");
  assert_parsed_expression_simplify_to("x^0", "1");
  assert_parsed_expression_simplify_to("P^0", "1");
  assert_parsed_expression_simplify_to("A^0", "1");
  assert_parsed_expression_simplify_to("(-3)^0", "1");
}
