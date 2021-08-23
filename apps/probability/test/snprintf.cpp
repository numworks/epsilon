#include <quiz.h>
#include <string.h>

#include "probability/text_helpers.h"

namespace Probability {

QUIZ_CASE(snprintf) {
  constexpr int buffer1Size = 20;
  char buffer1[buffer1Size];
  snprintf(buffer1, buffer1Size, "%s bar %s", "foo", "baz");
  quiz_assert(strcmp(buffer1, "foo bar baz") == 0);

  constexpr int buffer2Size = 20;
  char buffer2[buffer2Size];
  snprintf(buffer2, buffer2Size, "%c bar %c", 'a', 'z');
  quiz_assert(strcmp(buffer2, "a bar z") == 0);
}

}  // namespace Probability