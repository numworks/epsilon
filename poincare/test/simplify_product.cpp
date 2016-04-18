#include <assert.h>
#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_simplify_product_by_zero) {
  assert(simplifies_to("3*0", "0"));
  assert(simplifies_to("foo*0", "0"));

  assert(simplifies_to("0*3", "0"));
  assert(simplifies_to("0*foo", "0"));

  assert(simplifies_to("3*5", "15"));
  assert(simplifies_to("8*6", "48"));

  assert(simplifies_to("3*(5+4)", "27"));
}
