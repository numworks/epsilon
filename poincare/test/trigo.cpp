#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_trigo) {
  {
    Expression * e = Expression::parse("sin(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Sine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("cos(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Cosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("tan(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Tangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("cosh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("sinh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("tanh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicTangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("acos(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("asin(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("atan(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcTangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("acosh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("asinh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("atanh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcTangent);
    delete e;
    delete e2;
  }
}

QUIZ_CASE(poincare_trigo_evaluate) {
  Complex<double> a[1] = {Complex<double>::Float(-0.4161468365)};
  assert_parsed_expression_evaluate_to("cos(2)", a, 1, Radian);
  Complex<float> a1[1] = {Complex<float>::Cartesian(-1.0086248134f, -0.8893951958f)};
  assert_parsed_expression_evaluate_to("cos(I-4)", a1, 1, Radian);
  Complex<double> b[1] = {Complex<double>::Float(0.9092974268)};
  assert_parsed_expression_evaluate_to("sin(2)", b, 1, Radian);
  Complex<float> b1[1] = {Complex<float>::Cartesian( 1.16780727488f, -0.768162763456f)};
  assert_parsed_expression_evaluate_to("sin(I-4)", b1, 1, Radian);
  Complex<double> c[1] = {Complex<double>::Float(-2.18503986326151899)};
  assert_parsed_expression_evaluate_to("tan(2)", c, 1, Radian);
  Complex<float> c1[1] = {Complex<float>::Cartesian(-0.27355308280730f, 1.002810507583504f)};
  assert_parsed_expression_evaluate_to("tan(I-4)", c1, 1, Radian);
  Complex<double> a2[1] = {Complex<double>::Float(3.762195691)};
  assert_parsed_expression_evaluate_to("cosh(2)", a2, 1, Radian);
  Complex<float> a3[1] = {Complex<float>::Cartesian(14.754701170483756280f,-22.96367349919304059f)};
  assert_parsed_expression_evaluate_to("cosh(I-4)", a3, 1, Radian);
  Complex<double> b2[1] = {Complex<double>::Float(3.62686040784701876)};
  assert_parsed_expression_evaluate_to("sinh(2)", b2, 1, Radian);
  Complex<float> b3[1] = {Complex<float>::Cartesian(-14.744805188558725031023f, 22.979085577886129555168f)};
  assert_parsed_expression_evaluate_to("sinh(I-4)", b3, 1, Radian);
  Complex<double> c2[1] = {Complex<double>::Float(0.9640275800758168839464)};
  assert_parsed_expression_evaluate_to("tanh(2)", c2, 1, Radian);
  Complex<float> c3[1] = {Complex<float>::Cartesian(-1.00027905623446556836909f, 0.000610240921376259f)};
  assert_parsed_expression_evaluate_to("tanh(I-4)", c3, 1, Radian);
}
