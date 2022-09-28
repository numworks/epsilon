#include <poincare/print.h>
#include "helper.h"

using namespace Poincare;

void assert_string_equality(const char * buffer, const char * result) {
  quiz_assert_print_if_failure(strcmp(result, buffer) == 0, result);
}

QUIZ_CASE(poincare_print_custom_print) {
  constexpr int bufferSize = 128;
  char buffer[bufferSize];

  Poincare::Print::CustomPrintf(buffer, bufferSize, "Hello %c", 'A');
  assert_string_equality(buffer, "Hello A");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "%c bar %c", 'a', 'z');
  assert_string_equality(buffer, "a bar z");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "Hello %s", "NumWorks");
  assert_string_equality(buffer, "Hello NumWorks");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "%Cs bar %cs", "foo", "BAz");
  assert_string_equality(buffer, "Foo bar bAz");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "Hello %i", 123);
  assert_string_equality(buffer, "Hello 123");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "A float: %*.*ef!", 0.0123456789f, Preferences::PrintFloatMode::Decimal, 7);
  assert_string_equality(buffer, "A float: 0.01234568!");

  Poincare::Print::CustomPrintf(buffer, bufferSize, "A double: %*.*ed!", 0.0123456789, Preferences::PrintFloatMode::Scientific, 4);
  assert_string_equality(buffer, "A double: 1.235ᴇ-2!");

  constexpr int shortBufferSize = 5;
  char shortBuffer[shortBufferSize];

  Poincare::Print::CustomPrintf(shortBuffer, shortBufferSize, "%s", "1234");
  assert_string_equality(shortBuffer, "1234");

  quiz_assert(Poincare::Print::SafeCustomPrintf(shortBuffer, shortBufferSize, "%s", "12345") >= shortBufferSize);
  assert_string_equality(shortBuffer, "");
}
