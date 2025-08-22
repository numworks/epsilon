#include <omg/string.h>
#include <quiz.h>
#include <string.h>

using namespace OMG;

QUIZ_CASE(omg_string) {
  String<10> string1;
  quiz_assert(string1.length() == 0);

  String<10> abcString{'a', 'b', 'c'};
  quiz_assert(abcString.length() == 3);

  char helloCString[] = "hello";
  String<10> helloString(helloCString, strlen(helloCString));
  quiz_assert(helloString.length() == 5);
  quiz_assert(strlen(helloCString) == helloString.length());
}
