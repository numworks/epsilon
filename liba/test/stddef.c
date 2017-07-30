#include <quiz.h>
#include <stddef.h>
#include <assert.h>
#include "helpers.h"

QUIZ_CASE(liba_stddef) {
  assert(NULL == 0);

  assert_unsigned(size_t);
  assert(sizeof(size_t) == __SIZEOF_SIZE_T__);
  assert_signed(ssize_t);
  assert(sizeof(ssize_t) == __SIZEOF_SIZE_T__);
}
