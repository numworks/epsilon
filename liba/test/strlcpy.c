#include <quiz.h>
#include <string.h>
#include <assert.h>

QUIZ_CASE(liba_strlcpy) {
  char * t = "Hello";
  char buffer[16];
  size_t result = strlcpy(buffer, t, 2);
  quiz_assert(result == strlen(t));
  quiz_assert(buffer[0] == 'H');
  quiz_assert(buffer[1] == NULL);
}
