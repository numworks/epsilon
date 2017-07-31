#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_matrix_evaluate) {
#if MATRICES_ARE_DEFINED
  Complex a[6] = {Complex::Float(1.0f), Complex::Float(2.0f), Complex::Float(3.0f), Complex::Float(4.0f), Complex::Float(5.0f), Complex::Float(6.0f)};
  assert_parsed_expression_evaluate_to("[[1,2,3][4,5,6]]", a, 6);
#endif
}
