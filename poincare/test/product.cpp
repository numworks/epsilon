#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_product_evaluate) {
  Complex<float> a[1] = {Complex<float>::Float(2.0f)};
  assert_parsed_expression_evaluate_to("1*2", a, 1);

  Complex<double> b[1] = {Complex<double>::Cartesian(11.0, 7.0)};
  assert_parsed_expression_evaluate_to("(3+I)*(4+I)", b, 1);

#if MATRICES_ARE_DEFINED
  Complex<float> c[6] = {Complex<float>::Float(2.0f), Complex<float>::Float(4.0f), Complex<float>::Float(6.0f), Complex<float>::Float(8.0f), Complex<float>::Float(10.0f), Complex<float>::Float(12.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]*2", c, 6);

  Complex<double> d[6] = {Complex<double>::Cartesian(3.0, 1.0), Complex<double>::Cartesian(5.0, 5.0), Complex<double>::Cartesian(9.0, 3.0), Complex<double>::Cartesian(12.0, 4.0), Complex<double>::Cartesian(15.0, 5.0), Complex<double>::Cartesian(18.0, 6.0)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]*(3+I)", d, 6);

  assert_parsed_expression_evaluate_to("2*[[1,2][3,4][5,6]]", c, 6);

  assert_parsed_expression_evaluate_to("(3+I)*[[1,2+I][3,4][5,6]]", d, 6);

  Complex<float> e[12] = {Complex<float>::Float(11.0f), Complex<float>::Float(14.0f), Complex<float>::Float(17.0f), Complex<float>::Float(20.0f), Complex<float>::Float(23.0f), Complex<float>::Float(30.0f), Complex<float>::Float(37.0f), Complex<float>::Float(44.0f), Complex<float>::Float(35.0f), Complex<float>::Float(46.0f), Complex<float>::Float(57.0f), Complex<float>::Float(68.0f)};
  assert_parsed_expression_evaluate_to("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]", e, 12);

  Complex<double> f[12] = {Complex<double>::Cartesian(11.0, 5.0), Complex<double>::Cartesian(13.0, 9.0), Complex<double>::Cartesian(17.0, 7.0), Complex<double>::Cartesian(20.0, 8.0), Complex<double>::Cartesian(23.0, 0.0), Complex<double>::Cartesian(30.0, 7.0), Complex<double>::Cartesian(37.0, 0.0), Complex<double>::Cartesian(44.0, 0.0), Complex<double>::Cartesian(35.0, 0.0), Complex<double>::Cartesian(46.0, 11.0), Complex<double>::Cartesian(57.0, 0.0), Complex<double>::Cartesian(68.0, 0.0)};
  assert_parsed_expression_evaluate_to("[[1,2+I][3,4][5,6]]*[[1,2+I,3,4][5,6+I,7,8]]", f, 12);
#endif
}
