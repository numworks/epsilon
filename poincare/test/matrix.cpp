#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_matrix_evaluate) {
  Context context;
  Expression * f = Expression::parse("[[1,2,3][4,5,6]]");
  assert(f->operand(0)->approximate(context) == 1.0f);
  assert(f->operand(1)->approximate(context) == 2.0f);
  assert(f->operand(2)->approximate(context) == 3.0f);
  assert(f->operand(3)->approximate(context) == 4.0f);
  assert(f->operand(4)->approximate(context) == 5.0f);
  assert(f->operand(5)->approximate(context) == 6.0f);
  delete f;
}
