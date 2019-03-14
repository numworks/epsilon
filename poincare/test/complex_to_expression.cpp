#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_complex_to_expression) {
  assert_parsed_expression_evaluates_to<float>("0", "0");
  assert_parsed_expression_evaluates_to<float>("0", "0", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0", "0");
  assert_parsed_expression_evaluates_to<double>("0", "0", System, Radian, Polar);

  assert_parsed_expression_evaluates_to<float>("10", "10");
  assert_parsed_expression_evaluates_to<float>("-10", "-10");
  assert_parsed_expression_evaluates_to<float>("100", "100");
  assert_parsed_expression_evaluates_to<float>("0.1", "0.1");
  assert_parsed_expression_evaluates_to<float>("0.1234567", "0.1234567");
  assert_parsed_expression_evaluates_to<float>("0.12345678", "0.1234568");
  assert_parsed_expression_evaluates_to<float>("1+2*I", "1+2*I");
  assert_parsed_expression_evaluates_to<float>("1+I-I", "1");
  assert_parsed_expression_evaluates_to<float>("1+I-1", "I");
  assert_parsed_expression_evaluates_to<float>("1+I", "1+I");
  assert_parsed_expression_evaluates_to<float>("3+I", "3+I");
  assert_parsed_expression_evaluates_to<float>("3-I", "3-I");
  assert_parsed_expression_evaluates_to<float>("3-I-3", "-I");

  assert_parsed_expression_evaluates_to<float>("10", "10", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("-10", "10*X^(3.141593*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("100", "100", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.1", "0.1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.1234567", "0.1234567", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.12345678", "0.1234568", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+2*I", "2.236068*X^(1.107149*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+I-I", "1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+I-1", "X^(1.570796*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+I", "1.414214*X^(0.7853982*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3+I", "3.162278*X^(0.3217506*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3-I", "3.162278*X^(-0.3217506*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3-I-3", "X^(-1.570796*I)", System, Radian, Polar);

  assert_parsed_expression_evaluates_to<double>("10", "10");
  assert_parsed_expression_evaluates_to<double>("-10", "-10");
  assert_parsed_expression_evaluates_to<double>("100", "100");
  assert_parsed_expression_evaluates_to<double>("0.1", "0.1");
  assert_parsed_expression_evaluates_to<double>("0.12345678901234", "1.2345678901234E-1");
  assert_parsed_expression_evaluates_to<double>("0.123456789012345", "1.2345678901235E-1");
  assert_parsed_expression_evaluates_to<double>("1+2*I", "1+2*I");
  assert_parsed_expression_evaluates_to<double>("1+I-I", "1");
  assert_parsed_expression_evaluates_to<double>("1+I-1", "I");
  assert_parsed_expression_evaluates_to<double>("1+I", "1+I");
  assert_parsed_expression_evaluates_to<double>("3+I", "3+I");
  assert_parsed_expression_evaluates_to<double>("3-I", "3-I");
  assert_parsed_expression_evaluates_to<double>("3-I-3", "-I");

  assert_parsed_expression_evaluates_to<double>("10", "10", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("-10", "10*X^(3.1415926535898*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("100", "100", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.1", "0.1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.1234567", "0.1234567", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.12345678", "0.12345678", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1+2*I", "2.2360679775*X^(1.10714871779*I)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("1+I-I", "1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1+I-1", "X^(1.57079632679*I)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("1+I", "1.41421356237*X^(0.785398163397*I)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("3+I", "3.16227766017*X^(0.321750554397*I)", System, Radian, Polar,12);
  assert_parsed_expression_evaluates_to<double>("3-I", "3.16227766017*X^(-0.321750554397*I)", System, Radian, Polar,12);
  assert_parsed_expression_evaluates_to<double>("3-I-3", "X^(-1.57079632679*I)", System, Radian, Polar,12);

  assert_parsed_expression_evaluates_to<double>("2+3*I", "3.60555127546*X^(0.982793723247*I)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("3.60555127546*X^(0.982793723247*I)", "2+3*I", System, Radian, Cartesian, 12);
  assert_parsed_expression_evaluates_to<float>("12.04159457879229548012824103*X^(1.4876550949*I)", "1+12*I", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("-2E20+2E20*I", "-2E20+2E20*I");
  assert_parsed_expression_evaluates_to<float>("-2E20+2E20*I", "2.828427E20*X^(2.356194*I)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1E155-1E155*I", "1E155-1E155*I");
  assert_parsed_expression_evaluates_to<double>("1E155-1E155*I", "1.41421356237E155*X^(-0.785398163397*I)", System, Radian, Polar,12);

  assert_parsed_expression_evaluates_to<float>("-2E100+2E100*I", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("-2E360+2E360*I", Undefined::Name());
  assert_parsed_expression_evaluates_to<float>("-2E100+2E10*I", "-inf+2E10*I");
  assert_parsed_expression_evaluates_to<double>("-2E360+2*I", "-inf+2*I");
  assert_parsed_expression_evaluates_to<float>("undef+2E100*I", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("-2E360+undef*I", Undefined::Name());

  assert_parsed_expression_evaluates_to<double>("2*X^(I)", "2*X^I", System, Radian, Polar, 5);
  assert_parsed_expression_evaluates_to<double>("2*X^(-I)", "2*X^(-I)", System, Radian, Polar, 5);
}
