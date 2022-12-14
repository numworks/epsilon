#include <omg/print.h>
#include <poincare/test/helper.h>

using namespace OMG;

void assert_int_prints_as(uint32_t integer, const char * result, Print::Base base) {
  constexpr int bufferSize = Ion::BitHelper::numberOfBitsInType<uint32_t>() + 1;
  char buffer[bufferSize];
  for (int i = 0; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Print::Integer(base, integer, buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(result, buffer) == 0, result);
}

QUIZ_CASE(omg_print_integer) {
  assert_int_prints_as(1, "00000000000000000000000000000001", Print::Base::Binary);
  assert_int_prints_as(0, "00000000000000000000000000000000", Print::Base::Binary);
  assert_int_prints_as(3, "00000000000000000000000000000011", Print::Base::Binary);
  assert_int_prints_as(12, "0000000C", Print::Base::Hexadecimal);
  assert_int_prints_as(17, "00000011", Print::Base::Hexadecimal);
}
