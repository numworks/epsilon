#include <quiz.h>
#include <stddef.h>
#include <assert.h>
#include "helpers.h"

QUIZ_CASE(liba_stddef) {
  quiz_assert(NULL == 0);

  assert_unsigned(size_t);
  quiz_assert(sizeof(size_t) == __SIZEOF_SIZE_T__);
  assert_signed(ssize_t);
  quiz_assert(sizeof(ssize_t) == __SIZEOF_SIZE_T__);
}
