#include <quiz.h>
#include <stdint.h>
#include <assert.h>
#include "helpers.h"

QUIZ_CASE(liba_stdint_size) {
  assert(sizeof(uint8_t) == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);

  assert(sizeof(int8_t) == 1);
  assert(sizeof(int16_t) == 2);
  assert(sizeof(int32_t) == 4);
  assert(sizeof(int64_t) == 8);

  assert(sizeof(uintptr_t) == sizeof(void *));
  assert(sizeof(intptr_t) == sizeof(void *));
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
