#include <assert.h>
#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_simplify_product_by_zero) {
  assert(simplifies_to("3*0", "0"));
  assert(simplifies_to("A*0", "0"));

  assert(simplifies_to("0*3", "0"));
  assert(simplifies_to("0*A", "0"));

  assert(simplifies_to("3*5", "15"));
  assert(simplifies_to("8*6", "48"));

  assert(simplifies_to("3*(5+4)", "27"));
}

QUIZ_CASE(poincare_simplify_distributive_reverse) {
  assert(equivalent_to("x+x", "2*x"));
  assert(equivalent_to("2*x+x", "3*x"));
  assert(equivalent_to("x*2+x", "3*x"));
  assert(equivalent_to("2*x+2*x", "4*x"));
  assert(equivalent_to("x*2+2*t", "2*(x+t)"));
  assert(equivalent_to("x+x+t+t", "2*x+2*t"));
  assert(equivalent_to("2*x+2*t", "2*(x+t)"));
  //assert(equivalent_to("x+x+t+t", "2*(x+t)"));
  assert(equivalent_to("x-x-t+t", "0)"));
  assert(equivalent_to("x+t-x-t", "0"));
  assert(equivalent_to("x+x*t", "x*(t+1)"));
  assert(equivalent_to("x-x", "0"));
  assert(equivalent_to("x-x", "0"));
}
