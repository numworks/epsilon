#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_store_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(43.0f)};
  assert_parsed_expression_evaluates_to("1+42>A", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(0.123, 1.0)};
  assert_parsed_expression_evaluates_to("0.123+I>B", b);
}

QUIZ_CASE(poincare_store_simplify) {
  assert_parsed_expression_simplify_to("1+2>A", "A");
  assert_parsed_expression_simplify_to("1+2>x", "x");
}
