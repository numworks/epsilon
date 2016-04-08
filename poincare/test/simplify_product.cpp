#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_simplify_product_by_zero) {
  assert_simplifies_to("3*0", "0");
  assert_simplifies_to("foo*0", "0");
  assert_simplifies_to("0*3", "0");
  assert_simplifies_to("0*foo", "0");
}
