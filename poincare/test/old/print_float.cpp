#include <poincare/print_float.h>
#include <stdlib.h>
#include <string.h>

#include <cmath>

#include "../helper.h"

using namespace Poincare;

using enum Preferences::PrintFloatMode;

template <typename T>
void assert_float_prints_to(T a, const char* result,
                            Preferences::PrintFloatMode mode = Scientific,
                            int significantDigits = 7,
                            int bufferSize = PrintFloat::k_maxFloatCharSize) {
  constexpr int tagSize = 8;
  unsigned char tag = 'O';
  char taggedBuffer[250 + 2 * tagSize];
  int taggedAreaSize = bufferSize + 2 * tagSize;
  memset(taggedBuffer, tag, taggedAreaSize);
  char* buffer = taggedBuffer + tagSize;

  PrintFloat::ConvertFloatToText<T>(a, buffer, bufferSize,
                                    PrintFloat::k_maxFloatGlyphLength,
                                    significantDigits, mode);

  for (int i = 0; i < tagSize; i++) {
    quiz_assert_print_if_failure(taggedBuffer[i] == tag, result);
  }
  for (int i = tagSize + strlen(buffer) + 1; i < taggedAreaSize; i++) {
    quiz_assert_print_if_failure(taggedBuffer[i] == tag, result);
  }

  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
}

QUIZ_CASE(poincare_print_float) {
  assert_float_prints_to(123.456f, "1.23456ᴇ2", Scientific, 7);
  assert_float_prints_to(123.456f, "123.456", Decimal, 7);
  assert_float_prints_to(123.456f, "123.456", Engineering, 7);
  assert_float_prints_to(0.0006f, "6ᴇ-4", Decimal, 7);
  assert_float_prints_to(123.456, "1.23456ᴇ2", Scientific, 14);
  assert_float_prints_to(123.456, "123.456", Decimal, 14);
  assert_float_prints_to(123.456, "123.456", Engineering, 14);

  assert_float_prints_to(1.234567891011f, "1.234568", Scientific, 7);
  assert_float_prints_to(1.234567891011f, "1.234568", Decimal, 7);
  assert_float_prints_to(1.234567891011f, "1.234568", Engineering, 7);
  assert_float_prints_to(1.234567891011, "1.234567891011", Scientific, 14);
  assert_float_prints_to(1.234567891011, "1.234567891011", Decimal, 14);
  assert_float_prints_to(1.234567891011, "1.234567891011", Engineering, 14);

  assert_float_prints_to(2.0f, "2", Scientific, 7);
  assert_float_prints_to(2.0f, "2", Decimal, 7);
  assert_float_prints_to(2.0f, "2", Engineering, 7);
  assert_float_prints_to(2.0, "2", Scientific, 14);
  assert_float_prints_to(2.0, "2", Decimal, 14);
  assert_float_prints_to(2.0, "2", Engineering, 14);

  assert_float_prints_to(123456789.0f, "1.234568ᴇ8", Scientific, 7);
  assert_float_prints_to(123456789.0f, "1.234568ᴇ8", Decimal, 7);
  assert_float_prints_to(123456789.0f, "123.4568ᴇ6", Engineering, 7);
  assert_float_prints_to(123456789.0, "1.23456789ᴇ8", Scientific, 14);
  assert_float_prints_to(123456789.0, "123456789", Decimal, 14);
  assert_float_prints_to(123456789.0, "123.456789ᴇ6", Engineering, 14);

  assert_float_prints_to(0.00000123456789f, "1.234568ᴇ-6", Scientific, 7);
  assert_float_prints_to(0.00000123456789f, "1.234568ᴇ-6", Decimal, 7);
  assert_float_prints_to(0.00000123456789f, "1.234568ᴇ-6", Engineering, 7);
  assert_float_prints_to(0.00000123456789, "1.23456789ᴇ-6", Scientific, 14);
  assert_float_prints_to(0.00000123456789, "1.23456789ᴇ-6", Decimal, 14);
  assert_float_prints_to(0.00000123456789, "1.23456789ᴇ-6", Engineering, 14);

  assert_float_prints_to(0.99f, "9.9ᴇ-1", Scientific, 7);
  assert_float_prints_to(0.99f, "0.99", Decimal, 7);
  assert_float_prints_to(0.99f, "990ᴇ-3", Engineering, 7);
  assert_float_prints_to(0.99, "9.9ᴇ-1", Scientific, 14);
  assert_float_prints_to(0.99, "0.99", Decimal, 14);
  assert_float_prints_to(0.99, "990ᴇ-3", Engineering, 14);

  assert_float_prints_to(-123.456789f, "-1.234568ᴇ2", Scientific, 7);
  assert_float_prints_to(-123.456789f, "-123.4568", Decimal, 7);
  assert_float_prints_to(-123.456789f, "-123.4568", Engineering, 7);
  assert_float_prints_to(-123.456789, "-1.23456789ᴇ2", Scientific, 14);
  assert_float_prints_to(-123.456789, "-123.456789", Decimal, 14);
  assert_float_prints_to(-123.456789, "-123.456789", Engineering, 14);

  assert_float_prints_to(-0.000123456789f, "-1.234568ᴇ-4", Scientific, 7);
  assert_float_prints_to(-0.000123456789f, "-1.234568ᴇ-4", Decimal, 7);
  assert_float_prints_to(-0.000123456789f, "-123.4568ᴇ-6", Engineering, 7);
  assert_float_prints_to(-0.000123456789, "-1.23456789ᴇ-4", Scientific, 14);
  assert_float_prints_to(-0.000123456789, "-1.23456789ᴇ-4", Decimal, 14);
  assert_float_prints_to(-0.000123456789, "-123.456789ᴇ-6", Engineering, 14);

  assert_float_prints_to(0.0f, "0", Scientific, 7);
  assert_float_prints_to(0.0f, "0", Decimal, 7);
  assert_float_prints_to(0.0f, "0", Engineering, 7);
  assert_float_prints_to(0.0, "0", Scientific, 14);
  assert_float_prints_to(0.0, "0", Decimal, 14);
  assert_float_prints_to(0.0, "0", Engineering, 14);

  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", Scientific,
                         7);
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case,
   * the display mode is automatically set to scientific. */
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", Decimal, 7);
  assert_float_prints_to(10000000000000000000000000000.0, "10ᴇ27", Engineering,
                         7);
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", Scientific,
                         14);
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", Decimal, 14);
  assert_float_prints_to(10000000000000000000000000000.0, "10ᴇ27", Engineering,
                         14);

  // This used to crash on web platform
  assert_float_prints_to(1000.0, "1000", Decimal, 7);

  assert_float_prints_to(1000000.0f, "1ᴇ6", Scientific, 7);
  assert_float_prints_to(1000000.0f, "1000000", Decimal, 7);
  assert_float_prints_to(1000000.0f, "1ᴇ6", Engineering, 7);
  assert_float_prints_to(1000000.0, "1ᴇ6", Scientific, 14);
  assert_float_prints_to(1000000.0, "1000000", Decimal);
  assert_float_prints_to(1000000.0, "1ᴇ6", Engineering, 14);

  assert_float_prints_to(10000000.0f, "1ᴇ7", Scientific, 7);
  assert_float_prints_to(10000000.0f, "1ᴇ7", Decimal, 7);
  assert_float_prints_to(10000000.0f, "10ᴇ6", Engineering, 7);
  assert_float_prints_to(10000000.0, "1ᴇ7", Scientific, 14);
  assert_float_prints_to(10000000.0, "10000000", Decimal, 14);
  assert_float_prints_to(10000000.0, "10ᴇ6", Engineering, 14);

  assert_float_prints_to(0.0000001, "1ᴇ-7", Scientific, 7);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_float_prints_to(0.0000001f, "1ᴇ-7", Decimal, 7);
  assert_float_prints_to(0.0000001, "100ᴇ-9", Engineering, 7);
  assert_float_prints_to(0.0000001, "1ᴇ-7", Scientific, 14);
  assert_float_prints_to(0.0000001, "1ᴇ-7", Decimal, 14);
  assert_float_prints_to(0.0000001, "100ᴇ-9", Engineering, 14);

  assert_float_prints_to(-0.000000000000000000000000000000009090018f,
                         "-9.090018ᴇ-33", Scientific, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018f,
                         "-9.090018ᴇ-33", Decimal, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018f,
                         "-9.090018ᴇ-33", Engineering, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018,
                         "-9.090018ᴇ-33", Scientific, 14);
  assert_float_prints_to(-0.000000000000000000000000000000009090018,
                         "-9.090018ᴇ-33", Decimal, 14);
  assert_float_prints_to(-0.000000000000000000000000000000009090018,
                         "-9.090018ᴇ-33", Engineering, 14);

  assert_float_prints_to(123.421f, "1.23421ᴇ2", Scientific, 7);
  assert_float_prints_to(123.421f, "123.421", Engineering, 7);
  assert_float_prints_to(123.421f, "123.4", Decimal, 4, 6);
  assert_float_prints_to(123.421f, "1.2ᴇ2", Scientific, 2,
                         8);  // 'ᴇ' uses 3 bytes
  assert_float_prints_to(123.421f, "123.4", Engineering, 4, 8);

  assert_float_prints_to(9.999999f, "1ᴇ1", Scientific, 6);
  assert_float_prints_to(9.999999f, "10", Decimal, 6);
  assert_float_prints_to(9.999999f, "10", Engineering, 6);
  assert_float_prints_to(9.999999f, "9.999999", Scientific, 7);
  assert_float_prints_to(9.999999f, "9.999999", Decimal, 7);
  assert_float_prints_to(9.999999f, "9.999999", Engineering, 7);

  assert_float_prints_to(-9.99999904f, "-1ᴇ1", Scientific, 6);
  assert_float_prints_to(-9.99999904f, "-10", Decimal, 6);
  assert_float_prints_to(-9.99999904f, "-10", Engineering, 6);
  assert_float_prints_to(-9.99999904, "-9.999999", Scientific, 7);
  assert_float_prints_to(-9.99999904, "-9.999999", Decimal, 7);
  assert_float_prints_to(-9.99999904, "-9.999999", Engineering, 7);

  assert_float_prints_to(-0.017452f, "-1.745ᴇ-2", Scientific, 4);
  assert_float_prints_to(-0.017452f, "-0.01745", Decimal, 4);
  assert_float_prints_to(-0.017452f, "-17.45ᴇ-3", Engineering, 4);
  assert_float_prints_to(-0.017452, "-1.7452ᴇ-2", Scientific, 14);
  assert_float_prints_to(-0.017452, "-0.017452", Decimal, 14);
  assert_float_prints_to(-0.017452, "-17.452ᴇ-3", Engineering, 14);

  assert_float_prints_to(1E50, "1ᴇ50", Scientific, 9);
  assert_float_prints_to(1E50, "1ᴇ50", Decimal, 9);
  assert_float_prints_to(1E50, "100ᴇ48", Engineering, 9);
  assert_float_prints_to(1E50, "1ᴇ50", Scientific, 14);
  assert_float_prints_to(1E50, "1ᴇ50", Decimal, 14);
  assert_float_prints_to(1E50, "100ᴇ48", Engineering, 14);

  assert_float_prints_to(100.0, "1ᴇ2", Scientific, 9);
  assert_float_prints_to(100.0, "100", Decimal, 9);
  assert_float_prints_to(100.0, "100", Engineering, 9);

  assert_float_prints_to(12345.678910121314f, "1.234568ᴇ4", Scientific, 7);
  assert_float_prints_to(12345.678910121314f, "12345.68", Decimal, 7);
  assert_float_prints_to(12345.678910121314f, "12.34568ᴇ3", Engineering, 7);
  assert_float_prints_to(12345.678910121314, "1.2345678910121ᴇ4", Scientific,
                         14);
  assert_float_prints_to(12345.678910121314, "12345.678910121", Decimal, 14);
  assert_float_prints_to(12345.678910121314, "12.345678910121ᴇ3", Engineering,
                         14);

  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", Scientific, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", Decimal, 9);
  assert_float_prints_to(9.999999999999999999999E12, "10ᴇ12", Engineering, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", Scientific, 14);
  assert_float_prints_to(9.999999999999999999999E12, "10000000000000", Decimal,
                         14);
  assert_float_prints_to(9.999999999999999999999E12, "10ᴇ12", Engineering, 14);

  assert_float_prints_to(-0.000000099999999f, "-1ᴇ-7", Scientific, 7);
  assert_float_prints_to(-0.000000099999999f, "-1ᴇ-7", Decimal, 7);
  assert_float_prints_to(-0.000000099999999f, "-100ᴇ-9", Engineering, 7);
  assert_float_prints_to(-0.000000099999999, "-9.9999999ᴇ-8", Scientific, 9);
  assert_float_prints_to(-0.000000099999999, "-9.9999999ᴇ-8", Decimal, 9);
  assert_float_prints_to(-0.000000099999999, "-99.999999ᴇ-9", Engineering, 9);

  assert_float_prints_to(999.99999999999977f, "1ᴇ3", Scientific, 5);
  assert_float_prints_to(999.99999999999977f, "1000", Decimal, 5);
  assert_float_prints_to(999.99999999999977f, "1ᴇ3", Engineering, 5);
  assert_float_prints_to(999.99999999999977, "1ᴇ3", Scientific, 14);
  assert_float_prints_to(999.99999999999977, "1000", Decimal, 14);
  assert_float_prints_to(999.99999999999977, "1ᴇ3", Engineering, 14);

  assert_float_prints_to(0.000000999999997, "1ᴇ-6", Scientific, 7);
  assert_float_prints_to(0.000000999999997, "1ᴇ-6", Decimal, 7);
  assert_float_prints_to(0.000000999999997, "1ᴇ-6", Engineering, 7);
  assert_float_prints_to(9999999.97, "1ᴇ7", Decimal, 7);
  assert_float_prints_to(9999999.97, "10000000", Decimal, 8);
  assert_float_prints_to(9999999.97, "10ᴇ6", Engineering, 7);

  // Engineering notation
  assert_float_prints_to(0.0, "0", Engineering, 7);
  assert_float_prints_to(10.0, "10", Engineering, 7);
  assert_float_prints_to(100.0, "100", Engineering, 7);
  assert_float_prints_to(1000.0, "1ᴇ3", Engineering, 7);
  assert_float_prints_to(1234.0, "1.234ᴇ3", Engineering, 7);
  assert_float_prints_to(-0.1, "-100ᴇ-3", Engineering, 7);
  assert_float_prints_to(-0.01, "-10ᴇ-3", Engineering, 7);
  assert_float_prints_to(-0.001, "-1ᴇ-3", Engineering, 7);
}
