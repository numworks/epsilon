#include <omg/string.h>
#include <quiz.h>
#include <string.h>

#include <string_view>

using namespace OMG;

QUIZ_CASE(omg_string) {
  String<10> emptyString;
  quiz_assert(emptyString.length() == 0);

  String<10> abcString{'a', 'b', 'c'};
  quiz_assert(abcString.length() == 3);
  std::string_view abcStringView(abcString);
  quiz_assert(abcStringView.length() == abcString.length());

  char helloCString[] = "hello";
  String<10> helloString(helloCString, strlen(helloCString));
  quiz_assert(helloString.length() == 5);
  quiz_assert(strlen(helloCString) == helloString.length());
  std::string_view helloStringView(helloCString);
  quiz_assert(helloStringView.length() == helloString.length());
}
