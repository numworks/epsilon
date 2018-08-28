#include <assert.h>
#include <quiz.h>
#include "simplify_utils.h"

QUIZ_CASE(poincare_identity_simple_term) {
  quiz_assert(identical_to("1", "1"));
  quiz_assert(!identical_to("1", "2"));

  quiz_assert(identical_to("A", "A"));
  quiz_assert(!identical_to("A", "B"));

  quiz_assert(identical_to("1+2", "1+2"));
  quiz_assert(!identical_to("1+2", "1+3"));

  quiz_assert(identical_to("1-2", "1-2"));
  quiz_assert(!identical_to("1-2", "1-3"));

  quiz_assert(identical_to("1*2", "1*2"));
  quiz_assert(!identical_to("1*2", "1*3"));

  quiz_assert(identical_to("1/2", "1/2"));
  quiz_assert(!identical_to("1/2", "1/3"));

  quiz_assert(identical_to("1^2", "1^2"));
  quiz_assert(!identical_to("1^2", "1^3"));

  quiz_assert(identical_to("cos(1)", "cos(1)"));
  quiz_assert(!identical_to("cos(1)", "cos(2)"));
}

QUIZ_CASE(poincare_identity_commutativity) {
  quiz_assert(equivalent_to("1+2", "2+1"));
  //quiz_assert(identical_to("1*2", "2*1"));
  quiz_assert(!equivalent_to("1-2", "2-1"));
  quiz_assert(!equivalent_to("1/2", "2/1"));
}
