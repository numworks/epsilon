#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_addition_evaluate) {
  Complex a[1] = {Complex::Float(3.0f)};
  assert_parsed_expression_evaluate_to("1+2", a, 1);

  Complex b[1] = {Complex::Cartesian(6.0f, 2.0f)};
  assert_parsed_expression_evaluate_to("2+I+4+I", b, 1);

#if MATRICES_ARE_DEFINED
  Complex c[6] = {Complex::Float(4.0f), Complex::Float(5.0f), Complex::Float(6.0f), Complex::Float(7.0f), Complex::Float(8.0f), Complex::Float(9.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]+3", c, 6);

  Complex d[6] = {Complex::Cartesian(4.0f, 1.0f), Complex::Cartesian(5.0f, 2.0f), Complex::Cartesian(6.0f, 1.0f), Complex::Cartesian(7.0f, 1.0f), Complex::Cartesian(8.0f, 1.0f), Complex::Cartesian(9.0f, 1.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]+3+I", d, 6);

  assert_parsed_expression_evaluate_to("3+[[1,2][3,4][5,6]]", c, 6);

  assert_parsed_expression_evaluate_to("3+I+[[1,2+I][3,4][5,6]]", d, 6);

  Complex e[6] = {Complex::Float(2.0f), Complex::Float(4.0f), Complex::Float(6.0f), Complex::Float(8.0f), Complex::Float(10.0f), Complex::Float(12.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", e, 6);

  Complex f[6] = {Complex::Cartesian(2.0f, 0.0f), Complex::Cartesian(4.0f, 2.0f), Complex::Cartesian(6.0f, 0.0f), Complex::Cartesian(8.0f, 0.0f), Complex::Cartesian(10.0f, 0.0f), Complex::Cartesian(12.0f, 0.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]+[[1,2+I][3,4][5,6]]", f, 6);
#endif
}
