#include <quiz.h>
#include <stdint.h>
#include <assert.h>

QUIZ_CASE(liba_stdint) {
  int8_t i = -1;
  int j = -1;
  assert(i == -1);
  assert(i == j);
  assert(sizeof(int8_t) == 1);
}

