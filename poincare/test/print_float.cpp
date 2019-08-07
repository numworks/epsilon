#include <quiz.h>
#include <poincare/preferences.h>
#include <poincare/float.h>
#include <poincare/decimal.h>
#include <poincare/rational.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_float_prints_to(T a, const char * result, Preferences::PrintFloatMode mode = ScientificMode, int significantDigits = 7, int bufferSize = PrintFloat::k_maxFloatBufferLength) {
  constexpr int tagSize = 8;
  unsigned char tag = 'O';
  char taggedBuffer[250+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  PrintFloat::ConvertFloatToText<T>(a, buffer, bufferSize, significantDigits, mode);

  for (int i=0; i<tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag);
  }

  quiz_assert(strcmp(buffer, result) == 0);
}

QUIZ_CASE(assert_print_floats) {
  assert_float_prints_to(123.456f, "1.23456ᴇ2", ScientificMode, 7);
  assert_float_prints_to(123.456f, "123.456", DecimalMode, 7);
  assert_float_prints_to(123.456, "1.23456ᴇ2", ScientificMode, 14);
  assert_float_prints_to(123.456, "123.456", DecimalMode, 14);

  assert_float_prints_to(1.234567891011f, "1.234568", ScientificMode, 7);
  assert_float_prints_to(1.234567891011f, "1.234568", DecimalMode, 7);
  assert_float_prints_to(1.234567891011, "1.234567891011", ScientificMode, 14);
  assert_float_prints_to(1.234567891011, "1.234567891011", DecimalMode, 14);

  assert_float_prints_to(2.0f, "2", ScientificMode, 7);
  assert_float_prints_to(2.0f, "2", DecimalMode, 7);
  assert_float_prints_to(2.0, "2", ScientificMode, 14);
  assert_float_prints_to(2.0, "2", DecimalMode, 14);

  assert_float_prints_to(123456789.0f, "1.234568ᴇ8", ScientificMode, 7);
  assert_float_prints_to(123456789.0f, "1.234568ᴇ8", DecimalMode, 7);
  assert_float_prints_to(123456789.0, "1.23456789ᴇ8", ScientificMode, 14);
  assert_float_prints_to(123456789.0, "123456789", DecimalMode, 14);

  assert_float_prints_to(0.00000123456789f, "1.234568ᴇ-6", ScientificMode, 7);
  assert_float_prints_to(0.00000123456789f, "0.000001234568", DecimalMode, 7);
  assert_float_prints_to(0.00000123456789, "1.23456789ᴇ-6", ScientificMode, 14);
  assert_float_prints_to(0.00000123456789, "0.00000123456789", DecimalMode, 14);

  assert_float_prints_to(0.99f, "9.9ᴇ-1", ScientificMode, 7);
  assert_float_prints_to(0.99f, "0.99", DecimalMode, 7);
  assert_float_prints_to(0.99, "9.9ᴇ-1", ScientificMode, 14);
  assert_float_prints_to(0.99, "0.99", DecimalMode, 14);

  assert_float_prints_to(-123.456789f, "-1.234568ᴇ2", ScientificMode, 7);
  assert_float_prints_to(-123.456789f, "-123.4568", DecimalMode, 7);
  assert_float_prints_to(-123.456789, "-1.23456789ᴇ2", ScientificMode, 14);
  assert_float_prints_to(-123.456789, "-123.456789", DecimalMode, 14);

  assert_float_prints_to(-0.000123456789f, "-1.234568ᴇ-4", ScientificMode, 7);
  assert_float_prints_to(-0.000123456789f, "-0.0001234568", DecimalMode, 7);
  assert_float_prints_to(-0.000123456789, "-1.23456789ᴇ-4", ScientificMode, 14);
  assert_float_prints_to(-0.000123456789, "-0.000123456789", DecimalMode, 14);

  assert_float_prints_to(0.0f, "0", ScientificMode, 7);
  assert_float_prints_to(0.0f, "0", DecimalMode, 7);
  assert_float_prints_to(0.0, "0", ScientificMode, 14);
  assert_float_prints_to(0.0, "0", DecimalMode, 14);

  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", ScientificMode, 7);
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case, the
   * display mode is automatically set to scientific. */
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", DecimalMode, 7);
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", ScientificMode, 14);
  assert_float_prints_to(10000000000000000000000000000.0, "1ᴇ28", DecimalMode, 14);

  assert_float_prints_to(1000000.0f, "1ᴇ6", ScientificMode, 7);
  assert_float_prints_to(1000000.0f, "1000000", DecimalMode, 7);
  assert_float_prints_to(1000000.0, "1ᴇ6", ScientificMode, 14);
  assert_float_prints_to(1000000.0, "1000000", DecimalMode);

  assert_float_prints_to(10000000.0f, "1ᴇ7", ScientificMode, 7);
  assert_float_prints_to(10000000.0f, "1ᴇ7", DecimalMode, 7);
  assert_float_prints_to(10000000.0, "1ᴇ7", ScientificMode, 14);
  assert_float_prints_to(10000000.0, "10000000", DecimalMode, 14);

  assert_float_prints_to(0.0000001, "1ᴇ-7", ScientificMode, 7);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_float_prints_to(0.0000001f, "0.0000001", DecimalMode, 7);
  assert_float_prints_to(0.0000001, "1ᴇ-7", ScientificMode, 14);
  assert_float_prints_to(0.0000001, "0.0000001", DecimalMode, 14);

  assert_float_prints_to(-0.000000000000000000000000000000009090018f, "-9.090018ᴇ-33", ScientificMode, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018f, "-9.090018ᴇ-33", DecimalMode, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018ᴇ-33", ScientificMode, 14);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018ᴇ-33", DecimalMode, 14);

  assert_float_prints_to(123.421f, "1.23421ᴇ2", ScientificMode, 7);
  assert_float_prints_to(123.421f, "123.4", DecimalMode, 4, 6);
  assert_float_prints_to(123.421f, "1.2ᴇ2", ScientificMode, 4, 8); // 'ᴇ' uses 3 bytes

  assert_float_prints_to(9.999999f, "1ᴇ1", ScientificMode, 6);
  assert_float_prints_to(9.999999f, "10", DecimalMode, 6);
  assert_float_prints_to(9.999999f, "9.999999", ScientificMode, 7);
  assert_float_prints_to(9.999999f, "9.999999", DecimalMode, 7);

  assert_float_prints_to(-9.99999904f, "-1ᴇ1", ScientificMode, 6);
  assert_float_prints_to(-9.99999904f, "-10", DecimalMode, 6);
  assert_float_prints_to(-9.99999904, "-9.999999", ScientificMode, 7);
  assert_float_prints_to(-9.99999904, "-9.999999", DecimalMode, 7);

  assert_float_prints_to(-0.017452f, "-1.745ᴇ-2", ScientificMode, 4);
  assert_float_prints_to(-0.017452f, "-0.01745", DecimalMode, 4);
  assert_float_prints_to(-0.017452, "-1.7452ᴇ-2", ScientificMode, 14);
  assert_float_prints_to(-0.017452, "-0.017452", DecimalMode, 14);

  assert_float_prints_to(1E50, "1ᴇ50", ScientificMode, 9);
  assert_float_prints_to(1E50, "1ᴇ50", DecimalMode, 9);
  assert_float_prints_to(1E50, "1ᴇ50", ScientificMode, 14);
  assert_float_prints_to(1E50, "1ᴇ50", DecimalMode, 14);

  assert_float_prints_to(100.0, "1ᴇ2", ScientificMode, 9);
  assert_float_prints_to(100.0, "100", DecimalMode, 9);

  assert_float_prints_to(12345.678910121314f, "1.234568ᴇ4", ScientificMode, 7);
  assert_float_prints_to(12345.678910121314f, "12345.68", DecimalMode, 7);
  assert_float_prints_to(12345.678910121314, "1.2345678910121ᴇ4", ScientificMode, 14);
  assert_float_prints_to(12345.678910121314, "12345.678910121", DecimalMode, 14);

  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", ScientificMode, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", DecimalMode, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1ᴇ13", ScientificMode, 14);
  assert_float_prints_to(9.999999999999999999999E12, "10000000000000", DecimalMode, 14);

  assert_float_prints_to(-0.000000099999999f, "-1ᴇ-7", ScientificMode, 7);
  assert_float_prints_to(-0.000000099999999f, "-0.0000001", DecimalMode, 7);
  assert_float_prints_to(-0.000000099999999, "-9.9999999ᴇ-8", ScientificMode, 9);
  assert_float_prints_to(-0.000000099999999, "-0.000000099999999", DecimalMode, 9);

  assert_float_prints_to(999.99999999999977f, "1ᴇ3", ScientificMode, 5);
  assert_float_prints_to(999.99999999999977f, "1000", DecimalMode, 5);
  assert_float_prints_to(999.99999999999977, "1ᴇ3", ScientificMode, 14);
  assert_float_prints_to(999.99999999999977, "1000", DecimalMode, 14);

  assert_float_prints_to(0.000000999999997, "1ᴇ-6", ScientificMode, 7);
  assert_float_prints_to(0.000000999999997, "0.000001", DecimalMode, 7);
  assert_float_prints_to(9999999.97, "1ᴇ7", DecimalMode, 7);
  assert_float_prints_to(9999999.97, "10000000", DecimalMode, 8);
}
