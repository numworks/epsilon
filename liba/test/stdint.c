#include <quiz.h>
#include <stdint.h>
#include <assert.h>

QUIZ_CASE(liba_stdint_size) {
  assert(sizeof(uint8_t) == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);

  assert(sizeof(int8_t) == 1);
  assert(sizeof(int16_t) == 2);
  assert(sizeof(int32_t) == 4);
  assert(sizeof(int64_t) == 8);
}

QUIZ_CASE(liba_stdint_signedness) {
  int8_t i8 = -1;
  assert(i8 == -1);

  int16_t i16 = -1;
  assert(i16 == -1);

  int32_t i32 = -1;
  assert(i32 == -1);

  int64_t i64 = -1;
  assert(i64 == -1);
}
