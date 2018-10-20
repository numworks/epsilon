#include <quiz.h>
#include <poincare/expression.h>
#include <poincare/rational.h>

using namespace Poincare;

QUIZ_CASE(expression_can_start_uninitialized) {
  Expression e;
  {
    Rational i(1);
    e = i;
  }
}

QUIZ_CASE(expression_can_be_copied_even_if_uninitialized) {
  Expression e;
  Expression f;
  f = e;
}
