#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_simplify_addition_integer) {
  assert_simplifies_to("1", "1");
  assert_simplifies_to("1+2", "3");
  assert_simplifies_to("1+a", "1+a");
  assert_simplifies_to("1+2+3+4+5+6+7", "28");
  assert_simplifies_to("1+2+3+4+5+a+6+7", "28+a");
  assert_simplifies_to("a*(0+0)", "0");
}
