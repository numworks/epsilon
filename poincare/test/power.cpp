#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_power_evaluate) {
  Complex a[1] = {Complex::Float(8.0f)};
  assert_parsed_expression_evaluate_to("2^3", a, 1);

  Complex b[1] = {Complex::Cartesian(28.0f, 96.0f)};
  assert_parsed_expression_evaluate_to("(3+I)^4", b, 1);

  Complex c[1] = {Complex::Cartesian(11.7412464f, 62.9137754f)};
  assert_parsed_expression_evaluate_to("4^(3+I)", c, 1);

#if MATRICES_ARE_DEFINED
  Complex d[4] = {Complex::Float(37.0f), Complex::Float(54.0f), Complex::Float(81.0f), Complex::Float(118.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4]]^3", d, 4);
#endif
}
