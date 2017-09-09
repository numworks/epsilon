#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_power_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(8.0f)};
  assert_parsed_expression_evaluates_to("2^3", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(28.0, 96.0)};
  assert_parsed_expression_evaluates_to("(3+I)^4", b);

  Complex<float> c[1] = {Complex<float>::Cartesian(11.7412464f, 62.9137754f)};
  assert_parsed_expression_evaluates_to("4^(3+I)", c);

#if MATRICES_ARE_DEFINED
  Complex<double> d[4] = {Complex<double>::Float(37.0), Complex<double>::Float(54.0), Complex<double>::Float(81.0), Complex<double>::Float(118.0)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4]]^3", d, 2, 2);
#endif

  Complex<float> e[1] = {Complex<float>::Float(0.0f)};
  assert_parsed_expression_evaluates_to("0^2", e);

  Complex<double> f[1] = {Complex<double>::Float(std::exp(-M_PI_2))};
  assert_parsed_expression_evaluates_to("I^I", f);
}
