#include <assert.h>
#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_simplify_addition_integer) {
  assert(simplifies_to("1", "1"));
  assert(simplifies_to("1+2", "3"));
  assert(simplifies_to("1+a", "1+a"));
  assert(simplifies_to("1+2+3+4+5+6+7", "28"));
  assert(simplifies_to("1+2+3+4+5+a+6+7", "28+a"));
  assert(simplifies_to("a*(0+0)", "0"));
}
