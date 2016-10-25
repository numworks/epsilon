#include <quiz.h>
#include <assert.h>
#include <private/ieee754.h>

QUIZ_CASE(liba_ieee754) {
  assert(ieee754man(123.456f) == 7793017);
  assert(ieee754exp(123.456f) == 133);
  assert(ieee754man(555.555f) == 713605);
  assert(ieee754exp(555.555f) == 136);
  assert(ieee754man(0.007f) == 6643778);
  assert(ieee754exp(0.007f) == 119);
}
