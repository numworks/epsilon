#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "../src/expression_debug.h"

using namespace Poincare;

QUIZ_CASE(poincare_simplify_easy) {
  Expression * e = Expression::parse("1+1+ln(2)+(5+3*2)/9-4/7+1/98");
  print_expression(e, 0);
  Expression::simplify(&e);
  print_expression(e, 0);
  delete e;
}
