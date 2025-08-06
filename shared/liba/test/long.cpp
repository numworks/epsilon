#include <assert.h>
#include <quiz.h>
#include <stdint.h>

void assert_int64t_approximatively_equals_int64t(int64_t i, int64_t j) {
  quiz_assert(i - j < 1000000);
  quiz_assert(j - i < 1000000);
}

QUIZ_CASE(liba_long_arithmetic) {
  int64_t i = 123456789101112;
  quiz_assert((float)i == 1.23456789101112e14f);
  quiz_assert((double)i == 1.23456789101112e14);
  float f = 123456789101112.12345f;
  assert_int64t_approximatively_equals_int64t((int64_t)f, 123456789101112);
  double d = 123456789101112.12345f;
  assert_int64t_approximatively_equals_int64t((int64_t)d, 123456789101112);
}
