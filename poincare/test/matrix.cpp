#include <quiz.h>
#include <poincare.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_matrix_evaluate) {
#if MATRICES_ARE_DEFINED
  GlobalContext globalContext;
  Expression * f = Expression::parse("[[1,2,3][4,5,6]]");
  assert(f->operand(0)->approximate(globalContext) == 1.0f);
  assert(f->operand(1)->approximate(globalContext) == 2.0f);
  assert(f->operand(2)->approximate(globalContext) == 3.0f);
  assert(f->operand(3)->approximate(globalContext) == 4.0f);
  assert(f->operand(4)->approximate(globalContext) == 5.0f);
  assert(f->operand(5)->approximate(globalContext) == 6.0f);
  delete f;
#endif
}
