#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_integer) {
  assert(Integer(123) == Integer(123));
}

QUIZ_CASE(poincare_integer_add) {
  //assert(Integer((uint32_t)0) + Integer((uint32_t)0) == Integer((uint32_t)0));
  assert(Integer(123) + Integer(456) == Integer(579));
  assert(Integer::parseInteger("123456789123456789") + Integer(1) == Integer::parseInteger("123456789123456790"));
}

QUIZ_CASE(poincare_integer_multiply) {
  assert(Integer(12) * Integer(34) == Integer(408));
  assert(Integer(999999) * Integer(999999) == Integer::parseInteger("999998000001"));
  assert(Integer::parseInteger("9999999999") * Integer::parseInteger("9999999999") == Integer::parseInteger("99999999980000000001"));
}

QUIZ_CASE(poincare_integer_parse_integer) {
  assert(Integer::parseInteger("123") == Integer(123));
}

QUIZ_CASE(poincare_integer_approximate) {
  assert(Integer(1).approximate() == 1.0f);
  assert(Integer::parseInteger("12345678").approximate() == 12345678.0f);
}
