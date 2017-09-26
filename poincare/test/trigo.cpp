#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_trigo) {
  assert_parsed_expression_type("sin(0)", Expression::Type::Sine);
  assert_parsed_expression_type("cos(0)", Expression::Type::Cosine);
  assert_parsed_expression_type("tan(0)", Expression::Type::Tangent);
  assert_parsed_expression_type("cosh(0)", Expression::Type::HyperbolicCosine);
  assert_parsed_expression_type("sinh(0)", Expression::Type::HyperbolicSine);
  assert_parsed_expression_type("tanh(0)", Expression::Type::HyperbolicTangent);
  assert_parsed_expression_type("acos(0)", Expression::Type::ArcCosine);
  assert_parsed_expression_type("asin(0)", Expression::Type::ArcSine);
  assert_parsed_expression_type("atan(0)", Expression::Type::ArcTangent);
  assert_parsed_expression_type("acosh(0)", Expression::Type::HyperbolicArcCosine);
  assert_parsed_expression_type("asinh(0)", Expression::Type::HyperbolicArcSine);
  assert_parsed_expression_type("atanh(0)", Expression::Type::HyperbolicArcTangent);
}

QUIZ_CASE(poincare_trigo_evaluate) {
  Complex<double> a[1] = {Complex<double>::Float(-0.4161468365)};
  assert_parsed_expression_evaluates_to("cos(2)", a, Radian);
  Complex<float> a1[1] = {Complex<float>::Cartesian(-1.0086248134f, -0.8893951958f)};
  assert_parsed_expression_evaluates_to("cos(I-4)", a1, Radian);
  Complex<double> b[1] = {Complex<double>::Float(0.9092974268)};
  assert_parsed_expression_evaluates_to("sin(2)", b, Radian);
  Complex<float> b1[1] = {Complex<float>::Cartesian( 1.16780727488f, -0.768162763456f)};
  assert_parsed_expression_evaluates_to("sin(I-4)", b1, Radian);
  Complex<double> c[1] = {Complex<double>::Float(-2.18503986326151899)};
  assert_parsed_expression_evaluates_to("tan(2)", c, Radian);
  Complex<float> c1[1] = {Complex<float>::Cartesian(-0.27355308280730f, 1.002810507583504f)};
  assert_parsed_expression_evaluates_to("tan(I-4)", c1, Radian);
  Complex<double> a2[1] = {Complex<double>::Float(3.762195691)};
  assert_parsed_expression_evaluates_to("cosh(2)", a2, Radian);
  Complex<float> a3[1] = {Complex<float>::Cartesian(14.754701170483756280f,-22.96367349919304059f)};
  assert_parsed_expression_evaluates_to("cosh(I-4)", a3, Radian);
  Complex<double> b2[1] = {Complex<double>::Float(3.62686040784701876)};
  assert_parsed_expression_evaluates_to("sinh(2)", b2, Radian);
  Complex<float> b3[1] = {Complex<float>::Cartesian(-14.744805188558725031023f, 22.979085577886129555168f)};
  assert_parsed_expression_evaluates_to("sinh(I-4)", b3, Radian);
  Complex<double> c2[1] = {Complex<double>::Float(0.9640275800758168839464)};
  assert_parsed_expression_evaluates_to("tanh(2)", c2, Radian);
  Complex<float> c3[1] = {Complex<float>::Cartesian(-1.00027905623446556836909f, 0.000610240921376259f)};
  assert_parsed_expression_evaluates_to("tanh(I-4)", c3, Radian);
}
