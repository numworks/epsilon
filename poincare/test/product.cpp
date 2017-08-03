#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_product_evaluate) {
  Complex a[1] = {Complex::Float(2.0f)};
  assert_parsed_expression_evaluate_to("1*2", a, 1);

  Complex b[1] = {Complex::Cartesian(11.0f, 7.0f)};
  assert_parsed_expression_evaluate_to("(3+I)*(4+I)", b, 1);

#if MATRICES_ARE_DEFINED
  Complex c[6] = {Complex::Float(2.0f), Complex::Float(4.0f), Complex::Float(6.0f), Complex::Float(8.0f), Complex::Float(10.0f), Complex::Float(12.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]*2", c, 6);

  Complex d[6] = {Complex::Cartesian(3.0f, 1.0f), Complex::Cartesian(5.0f, 5.0f), Complex::Cartesian(9.0f, 3.0f), Complex::Cartesian(12.0f, 4.0f), Complex::Cartesian(15.0f, 5.0f), Complex::Cartesian(18.0f, 6.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]*(3+I)", d, 6);

  assert_parsed_expression_evaluate_to("2*[[1,2][3,4][5,6]]", c, 6);

  assert_parsed_expression_evaluate_to("(3+I)*[[1,2+I][3,4][5,6]]", d, 6);

  Complex e[12] = {Complex::Float(11.0f), Complex::Float(14.0f), Complex::Float(17.0f), Complex::Float(20.0f), Complex::Float(23.0f), Complex::Float(30.0f), Complex::Float(37.0f), Complex::Float(44.0f), Complex::Float(35.0f), Complex::Float(46.0f), Complex::Float(57.0f), Complex::Float(68.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]", e, 12);

  Complex f[12] = {Complex::Cartesian(11.0f, 5.0f), Complex::Cartesian(13.0f, 9.0f), Complex::Cartesian(17.0f, 7.0f), Complex::Cartesian(20.0f, 8.0f), Complex::Cartesian(23.0f, 0.0f), Complex::Cartesian(30.0f, 7.0f), Complex::Cartesian(37.0f, 0.0f), Complex::Cartesian(44.0f, 0.0f), Complex::Cartesian(35.0f, 0.0f), Complex::Cartesian(46.0f, 11.0f), Complex::Cartesian(57.0f, 0.0f), Complex::Cartesian(68.0f, 0.0f)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]*[[1,2+I,3,4][5,6+I,7,8]]", f, 12);
#endif
}
