#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_matrix_evaluate) {
#if MATRICES_ARE_DEFINED
  Complex<float> a[6] = {Complex<float>::Float(1.0f), Complex<float>::Float(2.0f), Complex<float>::Float(3.0f), Complex<float>::Float(4.0f), Complex<float>::Float(5.0f), Complex<float>::Float(6.0f)};
  assert_parsed_expression_evaluates_to("[[1,2,3][4,5,6]]", a, 2, 3);

  Complex<double> b[6] = {Complex<double>::Float(1.0), Complex<double>::Float(2.0), Complex<double>::Float(3.0), Complex<double>::Float(4.0), Complex<double>::Float(5.0), Complex<double>::Float(6.0)};
  assert_parsed_expression_evaluates_to("[[1,2,3][4,5,6]]", b, 2, 3);
#endif
}
