#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_integer) {
  assert(Integer(123) == Integer(123));
}

QUIZ_CASE(poincare_integer_add) {
  assert(Integer(123)+Integer(456) == Integer(579));
}
