#include <quiz.h>
#include <stdint.h>
#include <assert.h>
#include "helpers.h"

QUIZ_CASE(liba_stdint_size) {
  quiz_assert(sizeof(uint8_t) == 1);
  quiz_assert(sizeof(uint16_t) == 2);
  quiz_assert(sizeof(uint32_t) == 4);
  quiz_assert(sizeof(uint64_t) == 8);

  quiz_assert(sizeof(int8_t) == 1);
  quiz_assert(sizeof(int16_t) == 2);
  quiz_assert(sizeof(int32_t) == 4);
  quiz_assert(sizeof(int64_t) == 8);

  quiz_assert(sizeof(uintptr_t) == sizeof(void *));
  quiz_assert(sizeof(intptr_t) == sizeof(void *));
}

QUIZ_CASE(liba_stdint_signedness) {
  assert_signed(int8_t);
  assert_signed(int16_t);
  assert_signed(int32_t);
  assert_signed(int64_t);

  assert_unsigned(uint8_t);
  assert_unsigned(uint16_t);
  assert_unsigned(uint32_t);
  assert_unsigned(uint64_t);

  assert_signed(intptr_t);
  assert_unsigned(uintptr_t);
}
