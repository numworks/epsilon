#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_procuct_approximate) {
  Context context;
  Expression * p = Expression::parse("1*2");
  assert(p->approximate(context) == 2.0f);
  delete p;
}
