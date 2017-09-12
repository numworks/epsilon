#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "../src/expression_debug.h"

using namespace Poincare;

QUIZ_CASE(poincare_simplify_easy) {
  Expression * e = Expression::parse("1+1+2*3+4+5");
  print_expression(e, 0);
  e->simplify();
  print_expression(e, 0);
  delete e;
}
