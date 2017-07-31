#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <math.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_fraction_evaluate) {
  Complex a[1] = {Complex::Float(0.5f)};
  assert_parsed_expression_evaluate_to("1/2", a, 1);

  Complex b[1] = {Complex::Cartesian(13.0f/17.0f, 1.0f/17.0f)};
  assert_parsed_expression_evaluate_to("(3+I)/(4+I)", b, 1);

#if MATRICES_ARE_DEFINED
  Complex c[6] = {Complex::Float(0.5f), Complex::Float(1.0f), Complex::Float(1.5f), Complex::Float(2.0f), Complex::Float(2.5f), Complex::Float(3.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]/2", c, 6);

  Complex d[6] = {Complex::Cartesian(4.0f/17.0f, -1.0f/17.0f), Complex::Cartesian(9.0f/17.0f, 2.0/17.0f), Complex::Cartesian(12.0f/17.0f, -3.0f/17.0f), Complex::Cartesian(16.0f/17.0f, -4.0f/17.0f), Complex::Cartesian(20.0f/17.0f, -5.0f/17.0f), Complex::Cartesian(24.0f/17.0f, -6.0/17.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]/(4+I)", d, 6);

  Complex e[4] = {Complex::Float(2.0f), Complex::Float(-1.0f), Complex::Float(1.0f), Complex::Float(0.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4]]/[[3,4][5,6]]", e, 4);

  Complex f[4] = {Complex::Float(-9.0f), Complex::Float(6.0f), Complex::Float(15.0f/2.0f), Complex::Float(-9.0f/2.0f)};
  assert_parsed_expression_evaluate_to("3/[[3,4][5,6]]", f, 4);

  // TODO: add this test when inverse of complex matrix is implemented
  /*Complex g[4] = {Complex::Cartesian(-9.0f, -12.0f), Complex::Cartesian(6.0f, 8.0f), Complex::Cartesian(15.0f/2.0f, 10.0f), Complex::Cartesian(-9.0f/2.0f, -6.0f)};
  assert_parsed_expression_evaluate_to("(3+4i)/[[1,2+i][3,4][5,6]]", g, 4);*/

#endif
}
