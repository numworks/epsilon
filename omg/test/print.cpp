#include <omg/print.h>
#include <poincare/test/helper.h>

using namespace OMG;

void assert_int_prints_as(uint32_t integer, const char * result, Base base, bool printLeadingZeros = false) {
  constexpr int bufferSize = OMG::BitHelper::numberOfBitsInType<uint32_t>() + 1;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Print::UInt32(base, integer, printLeadingZeros, buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(result, buffer) == 0, result);
}

QUIZ_CASE(omg_print_integer) {
  assert_int_prints_as(1, "1", Base::Binary);
  assert_int_prints_as(0, "0", Base::Binary);
  assert_int_prints_as(3, "11", Base::Binary);
  assert_int_prints_as(1385447424, "01010010100101000100000000000000", Base::Binary, true);
  assert_int_prints_as(12, "C", Base::Hexadecimal);
  assert_int_prints_as(17, "11", Base::Hexadecimal);
}
