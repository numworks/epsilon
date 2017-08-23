#include <quiz.h>
#include <setjmp.h>
#include <assert.h>

QUIZ_CASE(set_jmp_long_jmp) {
  jmp_buf env;
  int res = setjmp(env);
  if (res == 1) {
    return;
  }
  int newres = (0 == res) ? res : res + 1;
  longjmp(env, newres);
  assert(false);
}
