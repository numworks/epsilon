#include <quiz.h>
#include <poincare/print_int.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

void assert_int_prints_as(int integer, const char * result, bool left) {
  constexpr int bufferSize = 5;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  bool couldPrint = left ? PrintInt::Left(integer, buffer, bufferSize) : PrintInt::Right(integer, buffer, bufferSize);
  quiz_assert(couldPrint);
  int i = 0;
  while (result[i] != 0) {
    quiz_assert(result[i] == buffer[i]);
    i++;
  }
  while (i < bufferSize) {
    quiz_assert(buffer[i] == 0);
    i++;
  }
}

QUIZ_CASE(poincare_print_int_left) {
  assert_int_prints_as(1, "1", true);
  assert_int_prints_as(12, "12", true);
  assert_int_prints_as(15678, "15678", true);
  assert_int_prints_as(99999, "99999", true);
  assert_int_prints_as(0, "0", true);
}

QUIZ_CASE(poincare_print_int_right) {
  assert_int_prints_as(1, "00001", false);
  assert_int_prints_as(12, "00012", false);
  assert_int_prints_as(15678, "15678", false);
  assert_int_prints_as(99999, "99999", true);
  assert_int_prints_as(0, "00000", false);
}
