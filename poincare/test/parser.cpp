#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parser) {
  assert_parsed_expression_evaluates_to<float>("-2-3", "-5");
  assert_parsed_expression_evaluates_to<float>("1.2*X^(1)", "3.261938");
  assert_parsed_expression_evaluates_to<float>("X^2*X^(1)", "20.0855", Radian, Cartesian, 6); // WARNING: the 7th significant digits is wrong on simulator
  assert_parsed_expression_evaluates_to<double>("X^2*X^(1)", "20.085536923188");
  assert_parsed_expression_evaluates_to<double>("2*3^4+2", "164");
  assert_parsed_expression_evaluates_to<float>("-2*3^4+2", "-160");
  assert_parsed_expression_evaluates_to<double>("-sin(3)*2-3", "-3.2822400161197", Radian);
  assert_parsed_expression_evaluates_to<float>("-.003", "-0.003");
  assert_parsed_expression_evaluates_to<double>(".02E2", "2");
  assert_parsed_expression_evaluates_to<float>("5-2/3", "4.333333");
  assert_parsed_expression_evaluates_to<double>("2/3-5", "-4.3333333333333");
  assert_parsed_expression_evaluates_to<float>("-2/3-5", "-5.666667");
  assert_parsed_expression_evaluates_to<double>("sin(3)2(4+2)", "1.6934400967184", Radian);
  assert_parsed_expression_evaluates_to<float>("4/2*(2+3)", "10");
  assert_parsed_expression_evaluates_to<double>("4/2*(2+3)", "10");
}
