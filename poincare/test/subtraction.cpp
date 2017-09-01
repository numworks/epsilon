#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_substraction_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(-1.0f)};
  assert_parsed_expression_evaluates_to("1-2", a);

  Complex<double> b[1] = {Complex<double>::Cartesian(-1.0, 0.0)};
  assert_parsed_expression_evaluates_to("3+I-(4+I)", b);

#if MATRICES_ARE_DEFINED
  Complex<float> c[6] = {Complex<float>::Float(-2.0f), Complex<float>::Float(-1.0f), Complex<float>::Float(0.0f), Complex<float>::Float(1.0f), Complex<float>::Float(2.0f), Complex<float>::Float(3.0f)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4][5,6]]-3", c, 3, 2);

  Complex<double> d[6] = {Complex<double>::Cartesian(-3.0, -1.0), Complex<double>::Cartesian(-2.0, 0.0), Complex<double>::Cartesian(-1.0, -1.0), Complex<double>::Cartesian(0.0, -1.0), Complex<double>::Cartesian(1.0, -1.0), Complex<double>::Cartesian(2.0, -1.0)};
  assert_parsed_expression_evaluates_to("[[1,2+I][3,4][5,6]]-(4+I)", d, 3, 2);

  Complex<float> e[6] = {Complex<float>::Float(2.0f), Complex<float>::Float(1.0f), Complex<float>::Float(0.0f), Complex<float>::Float(-1.0f), Complex<float>::Float(-2.0f), Complex<float>::Float(-3.0f)};
  assert_parsed_expression_evaluates_to("3-[[1,2][3,4][5,6]]", e, 3, 2);

  Complex<double> f[6] = {Complex<double>::Cartesian(2.0, 1.0), Complex<double>::Cartesian(1.0, 0.0), Complex<double>::Cartesian(0.0, 1.0), Complex<double>::Cartesian(-1.0, 1.0), Complex<double>::Cartesian(-2.0, 1.0), Complex<double>::Cartesian(-3.0, 1.0)};
  assert_parsed_expression_evaluates_to("3+I-[[1,2+I][3,4][5,6]]", f, 3, 2);

  Complex<float> g[6] = {Complex<float>::Float(-5.0f), Complex<float>::Float(-3.0f), Complex<float>::Float(-1.0f), Complex<float>::Float(1.0f), Complex<float>::Float(3.0f), Complex<float>::Float(5.0f)};
  assert_parsed_expression_evaluates_to("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]", g, 3, 2);

  Complex<double> h[6] = {Complex<double>::Float(0.0), Complex<double>::Float(0.0), Complex<double>::Float(0.0), Complex<double>::Float(0.0), Complex<double>::Float(0.0), Complex<double>::Float(0.0)};
  assert_parsed_expression_evaluates_to("[[1,2+I][3,4][5,6]]-[[1,2+I][3,4][5,6]]", h, 3, 2);
#endif
}
