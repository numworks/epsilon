#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_complex_evaluate) {
  GlobalContext globalContext;
  Expression * a = new Complex<float>(Complex<float>::Float(123.456f));
  Expression * m = a->approximate<double>(globalContext);
  assert(m->type() == Expression::Type::Complex);
  Complex<double> * mc = static_cast<Complex<double> *>(m);
  assert(std::fabs(mc->a() - 123.456) < 0.00001);
  assert(mc->b() == 0.0);
  delete m;

  Expression * n = a->approximate<float>(globalContext);
  assert(n->type() == Expression::Type::Complex);
  Complex<float> * nc = static_cast<Complex<float> *>(n);
  assert(nc->a() == 123.456f);
  assert(nc->b() == 0.0f);
  delete n;

  delete a;
}

QUIZ_CASE(poincare_complex_constructor) {
  Complex<float> * a = new Complex<float>(Complex<float>::Cartesian(2.0f, 3.0f));
  assert(std::fabs(a->a() - 2.0f) < 0.00001f && std::fabs(a->b()-3.0f) < 0.00001f);
  assert(a->r() == 3.60555124f && a->th() == 0.982793748f);
  delete a;

  a = new Complex<float>(Complex<float>::Polar(3.60555124f, 0.982793748f));
  assert(std::fabs(a->a() - 2.0f) < 0.00001f && std::fabs(a->b()-3.0f) < 0.00001f);
  delete a;

  Complex<double> * b = new Complex<double>(Complex<double>::Cartesian(1.0, 12.0));
  assert(std::fabs(b->a() - 1.0) < 0.0000000001 && std::fabs(b->b()-12.0) < 0.0000000001);
  delete b;

  b = new Complex<double>(Complex<double>::Polar(12.04159457879229548012824103, 1.4876550949));
  assert(std::fabs(b->a() - 1.0) < 0.0000000001 && std::fabs(b->b()-12.0) < 0.0000000001);
  delete b;

  Complex<float> * c = new Complex<float>(Complex<float>::Cartesian(-2.0e20f, 2.0e20f));
  assert(c->a() == -2.0e20f && c->b() == 2.0e20f);
  assert(c->r() == 2.0e20f*(float)M_SQRT2 && c->th() == 3*(float)M_PI_4);
  delete c;

  Complex<double> * d = new Complex<double>(Complex<double>::Cartesian(1.0e155, -1.0e155));
  assert(d->a() == 1.0e155 && d->b() == -1.0e155);
  assert(d->r() == 1.0e155*M_SQRT2 && d->th() == -M_PI_4);
  delete d;
}

QUIZ_CASE(poincare_complex_simplify) {
  assert_parsed_expression_simplify_to("I", "I");
  assert_parsed_expression_simplify_to("R(-33)", "R(33)*I");
  assert_parsed_expression_simplify_to("I^(3/5)", "(R(2)*R(5-R(5))+I+R(5)*I)/4");
  assert_parsed_expression_simplify_to("IIII", "1");
  assert_parsed_expression_simplify_to("R(-I)", "R(-I)");
  assert_parsed_expression_simplify_to("Acos(9)IIln(2)", "-cos(9)*ln(2)*A");
  assert_parsed_expression_simplify_to("(R(2)+R(2)*I)/2(R(2)+R(2)*I)/2(R(2)+R(2)*I)/2", "(R(2)-R(2)*I)/32");
  assert_parsed_expression_simplify_to("root(5^(-I)3^9,I)", "undef");
  assert_parsed_expression_simplify_to("I^I", "undef");
}
