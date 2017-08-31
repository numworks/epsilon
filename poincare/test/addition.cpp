#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_addition_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluate_to("1+2", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(6.0, 2.0)};
  assert_parsed_expression_evaluate_to("2+I+4+I", b);

#if MATRICES_ARE_DEFINED
  Complex<float> c[6] = {Complex<float>::Float(4.0f), Complex<float>::Float(5.0f), Complex<float>::Float(6.0f), Complex<float>::Float(7.0f), Complex<float>::Float(8.0f), Complex<float>::Float(9.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]+3", c, 3, 2);

  Complex<double> d[6] = {Complex<double>::Cartesian(4.0, 1.0), Complex<double>::Cartesian(5.0, 2.0), Complex<double>::Cartesian(6.0, 1.0), Complex<double>::Cartesian(7.0, 1.0), Complex<double>::Cartesian(8.0, 1.0), Complex<double>::Cartesian(9.0, 1.0)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]+3+I", d, 3, 2);

  assert_parsed_expression_evaluate_to("3+[[1,2][3,4][5,6]]", c, 3, 2);

  assert_parsed_expression_evaluate_to("3+I+[[1,2+I][3,4][5,6]]", d, 3, 2);

  Complex<float> e[6] = {Complex<float>::Float(2.0f), Complex<float>::Float(4.0f), Complex<float>::Float(6.0f), Complex<float>::Float(8.0f), Complex<float>::Float(10.0f), Complex<float>::Float(12.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", e, 3, 2);

  Complex<double> f[6] = {Complex<double>::Cartesian(2.0, 0.0), Complex<double>::Cartesian(4.0, 2.0), Complex<double>::Cartesian(6.0, 0.0), Complex<double>::Cartesian(8.0, 0.0), Complex<double>::Cartesian(10.0, 0.0), Complex<double>::Cartesian(12.0, 0.0)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]+[[1,2+I][3,4][5,6]]", f, 3, 2);
#endif
}
