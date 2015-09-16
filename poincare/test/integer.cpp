#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_integer) {
  assert(Integer(123) == Integer(123));
}

QUIZ_CASE(poincare_integer_add) {
  //assert(Integer((uint32_t)0) + Integer((uint32_t)0) == Integer((uint32_t)0));
  assert(Integer(123) + Integer(456) == Integer(579));
  assert(Integer(123) + Integer(123456789) == Integer(123456912));
}

QUIZ_CASE(poincare_integer_multiply) {
  assert(Integer(12) * Integer(34) == Integer(408));
}

QUIZ_CASE(poincare_integer_parse_integer) {
  assert(Integer::parseInteger("123") == Integer(123));
}
