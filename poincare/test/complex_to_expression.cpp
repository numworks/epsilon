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
  assert_parsed_expression_evaluates_to<float>("1+2×𝐢", "1+2×𝐢");
  assert_parsed_expression_evaluates_to<float>("1+𝐢-𝐢", "1");
  assert_parsed_expression_evaluates_to<float>("1+𝐢-1", "𝐢");
  assert_parsed_expression_evaluates_to<float>("1+𝐢", "1+𝐢");
  assert_parsed_expression_evaluates_to<float>("3+𝐢", "3+𝐢");
  assert_parsed_expression_evaluates_to<float>("3-𝐢", "3-𝐢");
  assert_parsed_expression_evaluates_to<float>("3-𝐢-3", "-𝐢");

  assert_parsed_expression_evaluates_to<float>("10", "10", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("-10", "10×ℯ^(3.141593×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("100", "100", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.1", "0.1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.1234567", "0.1234567", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("0.12345678", "0.1234568", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+2×𝐢", "2.236068×ℯ^(1.107149×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+𝐢-𝐢", "1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+𝐢-1", "ℯ^(1.570796×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("1+𝐢", "1.414214×ℯ^(0.7853982×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3+𝐢", "3.162278×ℯ^(0.3217506×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3-𝐢", "3.162278×ℯ^(-0.3217506×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<float>("3-𝐢-3", "ℯ^(-1.570796×𝐢)", System, Radian, Polar);

  assert_parsed_expression_evaluates_to<double>("10", "10");
  assert_parsed_expression_evaluates_to<double>("-10", "-10");
  assert_parsed_expression_evaluates_to<double>("100", "100");
  assert_parsed_expression_evaluates_to<double>("0.1", "0.1");
  assert_parsed_expression_evaluates_to<double>("0.12345678901234", "1.2345678901234ᴇ-1");
  assert_parsed_expression_evaluates_to<double>("0.123456789012345", "1.2345678901235ᴇ-1");
  assert_parsed_expression_evaluates_to<double>("1+2×𝐢", "1+2×𝐢");
  assert_parsed_expression_evaluates_to<double>("1+𝐢-𝐢", "1");
  assert_parsed_expression_evaluates_to<double>("1+𝐢-1", "𝐢");
  assert_parsed_expression_evaluates_to<double>("1+𝐢", "1+𝐢");
  assert_parsed_expression_evaluates_to<double>("3+𝐢", "3+𝐢");
  assert_parsed_expression_evaluates_to<double>("3-𝐢", "3-𝐢");
  assert_parsed_expression_evaluates_to<double>("3-𝐢-3", "-𝐢");

  assert_parsed_expression_evaluates_to<double>("10", "10", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("-10", "10×ℯ^(3.1415926535898×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("100", "100", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.1", "0.1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.1234567", "0.1234567", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("0.12345678", "0.12345678", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1+2×𝐢", "2.2360679775×ℯ^(1.10714871779×𝐢)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("1+𝐢-𝐢", "1", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1+𝐢-1", "ℯ^(1.57079632679×𝐢)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("1+𝐢", "1.41421356237×ℯ^(0.785398163397×𝐢)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("3+𝐢", "3.16227766017×ℯ^(0.321750554397×𝐢)", System, Radian, Polar,12);
  assert_parsed_expression_evaluates_to<double>("3-𝐢", "3.16227766017×ℯ^(-0.321750554397×𝐢)", System, Radian, Polar,12);
  assert_parsed_expression_evaluates_to<double>("3-𝐢-3", "ℯ^(-1.57079632679×𝐢)", System, Radian, Polar,12);

  assert_parsed_expression_evaluates_to<double>("2+3×𝐢", "3.60555127546×ℯ^(0.982793723247×𝐢)", System, Radian, Polar, 12);
  assert_parsed_expression_evaluates_to<double>("3.60555127546×ℯ^(0.982793723247×𝐢)", "2+3×𝐢", System, Radian, Cartesian, 12);
  assert_parsed_expression_evaluates_to<float>("12.04159457879229548012824103×ℯ^(1.4876550949×𝐢)", "1+12×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("-2ᴇ20+2ᴇ20×𝐢", "-2ᴇ20+2ᴇ20×𝐢");
  assert_parsed_expression_evaluates_to<float>("-2ᴇ20+2ᴇ20×𝐢", "2.828427ᴇ20×ℯ^(2.356194×𝐢)", System, Radian, Polar);
  assert_parsed_expression_evaluates_to<double>("1ᴇ155-1ᴇ155×𝐢", "1ᴇ155-1ᴇ155×𝐢");
  assert_parsed_expression_evaluates_to<double>("1ᴇ155-1ᴇ155×𝐢", "1.41421356237ᴇ155×ℯ^(-0.785398163397×𝐢)", System, Radian, Polar,12);

  assert_parsed_expression_evaluates_to<float>("-2ᴇ100+2ᴇ100×𝐢", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("-2ᴇ360+2ᴇ360×𝐢", Undefined::Name());
  assert_parsed_expression_evaluates_to<float>("-2ᴇ100+2ᴇ10×𝐢", "-inf+2ᴇ10×𝐢");
  assert_parsed_expression_evaluates_to<double>("-2ᴇ360+2×𝐢", "-inf+2×𝐢");
  assert_parsed_expression_evaluates_to<float>("undef+2ᴇ100×𝐢", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("-2ᴇ360+undef×𝐢", Undefined::Name());

  assert_parsed_expression_evaluates_to<double>("2×ℯ^(𝐢)", "2×ℯ^𝐢", System, Radian, Polar, 5);
  assert_parsed_expression_evaluates_to<double>("2×ℯ^(-𝐢)", "2×ℯ^(-𝐢)", System, Radian, Polar, 5);
}
