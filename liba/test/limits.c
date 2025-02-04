#include <assert.h>
#include <limits.h>
#include <quiz.h>
#include <stdint.h>

QUIZ_CASE(liba_limits) {
  quiz_assert(INT_MIN < 0);
  quiz_assert(INT_MAX > 0);
  int min = INT_MIN;
  int max = INT_MAX;
  quiz_assert(min == -2147483648);
  quiz_assert(max == 2147483647);
  quiz_assert(min + max == -1);

  quiz_assert(UINT_MAX > 0);
  uint32_t umax = UINT_MAX;
  quiz_assert(umax == 4294967295);
  quiz_assert(umax + 1 == 0);
}
