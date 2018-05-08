#include <assert.h>
#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_identity_simple_term) {
  assert(identical_to("1", "1"));
  assert(!identical_to("1", "2"));

  assert(identical_to("A", "A"));
  assert(!identical_to("A", "B"));

  assert(identical_to("1+2", "1+2"));
  assert(!identical_to("1+2", "1+3"));

  assert(identical_to("1-2", "1-2"));
  assert(!identical_to("1-2", "1-3"));

  assert(identical_to("1*2", "1*2"));
  assert(!identical_to("1*2", "1*3"));

  assert(identical_to("1/2", "1/2"));
  assert(!identical_to("1/2", "1/3"));

  assert(identical_to("1^2", "1^2"));
  assert(!identical_to("1^2", "1^3"));

  assert(identical_to("cos(1)", "cos(1)"));
  assert(!identical_to("cos(1)", "cos(2)"));
}

QUIZ_CASE(poincare_identity_commutativity) {
  assert(equivalent_to("1+2", "2+1"));
  //assert(identical_to("1*2", "2*1"));
  assert(!equivalent_to("1-2", "2-1"));
  assert(!equivalent_to("1/2", "2/1"));
}
