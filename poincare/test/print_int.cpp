#include <quiz.h>
#include <poincare/print_int.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

void assert_int_prints_as(int integer, const char * result) {
  constexpr int bufferSize = 5;
  char buffer[bufferSize];
  bool couldPrint = PrintInt::PadIntInBuffer(integer, buffer, bufferSize);
  quiz_assert(couldPrint);
  for (int i = 0; i < bufferSize; i++) {
    quiz_assert(result[i] == buffer[i]);
  }
}

QUIZ_CASE(poincare_print_int) {
  assert_int_prints_as(1, "00001");
  assert_int_prints_as(12, "00012");
  assert_int_prints_as(15678, "15678");
  assert_int_prints_as(0, "00000");
}
