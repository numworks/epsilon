#include <assert.h>
#include <limits.h>
#include <quiz.h>
#include <stdint.h>

QUIZ_CASE(liba_limits) {
  quiz_assert(INT_MIN < 0);
  quiz_assert(UINT_MAX > 0);
  int min = INT_MIN;
  int max = INT_MAX;
  quiz_assert(min - 1 == max);
  uint32_t umax = UINT_MAX;
  quiz_assert(umax + 1 == 0);
}
