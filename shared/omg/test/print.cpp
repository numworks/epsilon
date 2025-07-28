#include <omg/print.h>
#include <poincare/test/helper.h>
#include <poincare/test/old/helper.h>

using namespace OMG;
using namespace OMG::Print;

void assert_uint32_prints_as(uint32_t integer, const char* result, Base base,
                             OMG::Print::LeadingZeros printLeadingZeros =
                                 OMG::Print::LeadingZeros::Trim) {
  constexpr int bufferSize = OMG::BitHelper::numberOfBitsIn<uint32_t>() + 1;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Print::UInt32(base, integer, printLeadingZeros, buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(result, buffer) == 0, result);
}

QUIZ_CASE(omg_print_integer) {
  assert_uint32_prints_as(1, "1", Base::Binary);
  assert_uint32_prints_as(0, "0", Base::Binary);
  assert_uint32_prints_as(3, "11", Base::Binary);
  assert_uint32_prints_as(1385447424, "01010010100101000100000000000000",
                          Base::Binary, OMG::Print::LeadingZeros::Keep);
  assert_uint32_prints_as(12, "C", Base::Hexadecimal);
  assert_uint32_prints_as(17, "11", Base::Hexadecimal);
}

void assert_int_prints_as(int integer, const char* result, bool left) {
  constexpr int bufferSize = 5;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  bool couldPrint =
      (left ? IntLeft(integer, buffer, bufferSize)
            : IntRight(integer, buffer, bufferSize)) <= bufferSize;
  quiz_assert_print_if_failure(couldPrint, result);
  int i = 0;
  while (result[i] != 0) {
    quiz_assert_print_if_failure(result[i] == buffer[i], result);
    i++;
  }
  while (i < bufferSize) {
    quiz_assert_print_if_failure(buffer[i] == 0, result);
    i++;
  }
}

QUIZ_CASE(omg_print_int_left) {
  assert_int_prints_as(1, "1", true);
  assert_int_prints_as(12, "12", true);
  assert_int_prints_as(15678, "15678", true);
  assert_int_prints_as(99999, "99999", true);
  assert_int_prints_as(0, "0", true);
}

QUIZ_CASE(omg_print_int_right) {
  assert_int_prints_as(1, "00001", false);
  assert_int_prints_as(12, "00012", false);
  assert_int_prints_as(15678, "15678", false);
  assert_int_prints_as(99999, "99999", false);
  assert_int_prints_as(0, "00000", false);
}
