#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_substraction_evaluate) {
  Complex a[1] = {Complex::Float(-1.0f)};
  assert_parsed_expression_evaluate_to("1-2", a, 1);

  Complex b[1] = {Complex::Cartesian(-1.0f, 0.0f)};
  assert_parsed_expression_evaluate_to("3+I-(4+I)", b, 1);

#if MATRICES_ARE_DEFINED
  Complex c[6] = {Complex::Float(-2.0f), Complex::Float(-1.0f), Complex::Float(0.0f), Complex::Float(1.0f), Complex::Float(2.0f), Complex::Float(3.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]-3", c, 6);

  Complex d[6] = {Complex::Cartesian(-3.0f, -1.0f), Complex::Cartesian(-2.0f, 0.0f), Complex::Cartesian(-1.0f, -1.0f), Complex::Cartesian(0.0f, -1.0f), Complex::Cartesian(1.0f, -1.0f), Complex::Cartesian(2.0f, -1.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]-(4+I)", d, 6);

  Complex e[6] = {Complex::Float(2.0f), Complex::Float(1.0f), Complex::Float(0.0f), Complex::Float(-1.0f), Complex::Float(-2.0f), Complex::Float(-3.0f)};
  assert_parsed_expression_evaluate_to("3-[[1,2][3,4][5,6]]", e, 6);

  Complex f[6] = {Complex::Cartesian(2.0f, 1.0f), Complex::Cartesian(1.0f, 0.0f), Complex::Cartesian(0.0f, 1.0f), Complex::Cartesian(-1.0f, 1.0f), Complex::Cartesian(-2.0f, 1.0f), Complex::Cartesian(-3.0f, 1.0f)};
  assert_parsed_expression_evaluate_to("3+I-[[1,2+I][3,4][5,6]]", f, 6);

  Complex g[6] = {Complex::Float(-5.0f), Complex::Float(-3.0f), Complex::Float(-1.0f), Complex::Float(1.0f), Complex::Float(3.0f), Complex::Float(5.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]", g, 6);

  Complex h[6] = {Complex::Float(0.0f), Complex::Float(0.0f), Complex::Float(0.0f), Complex::Float(0.0f), Complex::Float(0.0f), Complex::Float(0.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]-[[1,2+I][3,4][5,6]]", h, 6);
#endif
}
