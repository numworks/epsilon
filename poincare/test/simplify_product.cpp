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

QUIZ_CASE(poincare_simplify_distributive_reverse) {
  assert(equivalent_to("x+x", "2*x"));
  assert(equivalent_to("2*x+x", "3*x"));
  assert(equivalent_to("x*2+x", "3*x"));
  assert(equivalent_to("2*x+2*x", "4*x"));
  assert(equivalent_to("x*2+2*y", "2*(x+y)"));
  assert(equivalent_to("x+x+y+y", "2*x+2*y"));
  assert(equivalent_to("2*x+2*y", "2*(x+y)"));
  //assert(equivalent_to("x+x+y+y", "2*(x+y)"));
  assert(equivalent_to("x-x-y+y", "0)"));
  assert(equivalent_to("x+y-x-y", "0"));
  assert(equivalent_to("x+x*y", "x*(y+1)"));
  assert(equivalent_to("x-x", "0"));
  assert(equivalent_to("x-x", "0"));
}
