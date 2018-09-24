#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_float_prints_to(T a, const char * result, Preferences::PrintFloatMode mode = ScientificMode, int significantDigits = 7, int bufferSize = PrintFloat::k_maxFloatBufferLength) {
  quiz_print(result);

  constexpr int tagSize = 8;
  unsigned char tag = 'O';
  char taggedBuffer[250+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  PrintFloat::convertFloatToText<T>(a, buffer, bufferSize, significantDigits, mode);

  for (int i=0; i<tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag);
  }
  translate_in_ASCII_chars(buffer);

  quiz_assert(strcmp(buffer, result) == 0);
}

void assert_expression_prints_to(Expression e, const char * result, Preferences::PrintFloatMode mode = ScientificMode, int numberOfSignificantDigits = 7, int bufferSize = 250) {
  quiz_print(result);

  int tagSize = 8;
  unsigned char tag = 'O';
  char * taggedBuffer = new char[bufferSize+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  e.serialize(buffer, bufferSize, mode, numberOfSignificantDigits);
  translate_in_ASCII_chars(buffer);

  for (int i=0; i<tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag || taggedBuffer[i] == 0);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    quiz_assert(taggedBuffer[i] == tag || taggedBuffer[i] == 0);
  }

  quiz_assert(strcmp(buffer, result) == 0);

  delete[] taggedBuffer;
}

QUIZ_CASE(assert_float_prints_to) {
  assert_float_prints_to(123.456f, "1.23456E2", ScientificMode, 7);
  assert_float_prints_to(123.456f, "123.456", DecimalMode, 7);
  assert_float_prints_to(123.456, "1.23456E2", ScientificMode, 14);
  assert_float_prints_to(123.456, "123.456", DecimalMode, 14);

  assert_float_prints_to(1.234567891011f, "1.234568", ScientificMode, 7);
  assert_float_prints_to(1.234567891011f, "1.234568", DecimalMode, 7);
  assert_float_prints_to(1.234567891011, "1.234567891011", ScientificMode, 14);
  assert_float_prints_to(1.234567891011, "1.234567891011", DecimalMode, 14);

  assert_float_prints_to(2.0f, "2", ScientificMode, 7);
  assert_float_prints_to(2.0f, "2", DecimalMode, 7);
  assert_float_prints_to(2.0, "2", ScientificMode, 14);
  assert_float_prints_to(2.0, "2", DecimalMode, 14);

  assert_float_prints_to(123456789.0f, "1.234568E8", ScientificMode, 7);
  assert_float_prints_to(123456789.0f, "1.234568E8", DecimalMode, 7);
  assert_float_prints_to(123456789.0, "1.23456789E8", ScientificMode, 14);
  assert_float_prints_to(123456789.0, "123456789", DecimalMode, 14);

  assert_float_prints_to(0.00000123456789f, "1.234568E-6", ScientificMode, 7);
  assert_float_prints_to(0.00000123456789f, "0.000001234568", DecimalMode, 7);
  assert_float_prints_to(0.00000123456789, "1.23456789E-6", ScientificMode, 14);
  assert_float_prints_to(0.00000123456789, "0.00000123456789", DecimalMode, 14);

  assert_float_prints_to(0.99f, "9.9E-1", ScientificMode, 7);
  assert_float_prints_to(0.99f, "0.99", DecimalMode, 7);
  assert_float_prints_to(0.99, "9.9E-1", ScientificMode, 14);
  assert_float_prints_to(0.99, "0.99", DecimalMode, 14);

  assert_float_prints_to(-123.456789f, "-1.234568E2", ScientificMode, 7);
  assert_float_prints_to(-123.456789f, "-123.4568", DecimalMode, 7);
  assert_float_prints_to(-123.456789, "-1.23456789E2", ScientificMode, 14);
  assert_float_prints_to(-123.456789, "-123.456789", DecimalMode, 14);

  assert_float_prints_to(-0.000123456789f, "-1.234568E-4", ScientificMode, 7);
  assert_float_prints_to(-0.000123456789f, "-0.0001234568", DecimalMode, 7);
  assert_float_prints_to(-0.000123456789, "-1.23456789E-4", ScientificMode, 14);
  assert_float_prints_to(-0.000123456789, "-0.000123456789", DecimalMode, 14);

  assert_float_prints_to(0.0f, "0", ScientificMode, 7);
  assert_float_prints_to(0.0f, "0", DecimalMode, 7);
  assert_float_prints_to(0.0, "0", ScientificMode, 14);
  assert_float_prints_to(0.0, "0", DecimalMode, 14);

  assert_float_prints_to(10000000000000000000000000000.0, "1E28", ScientificMode, 7);
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case, the
   * display mode is automatically set to scientific. */
  assert_float_prints_to(10000000000000000000000000000.0, "1E28", DecimalMode, 7);
  assert_float_prints_to(10000000000000000000000000000.0, "1E28", ScientificMode, 14);
  assert_float_prints_to(10000000000000000000000000000.0, "1E28", DecimalMode, 14);

  assert_float_prints_to(1000000.0f, "1E6", ScientificMode, 7);
  assert_float_prints_to(1000000.0f, "1000000", DecimalMode, 7);
  assert_float_prints_to(1000000.0, "1E6", ScientificMode, 14);
  assert_float_prints_to(1000000.0, "1000000", DecimalMode);

  assert_float_prints_to(10000000.0f, "1E7", ScientificMode, 7);
  assert_float_prints_to(10000000.0f, "1E7", DecimalMode, 7);
  assert_float_prints_to(10000000.0, "1E7", ScientificMode, 14);
  assert_float_prints_to(10000000.0, "10000000", DecimalMode, 14);

  assert_float_prints_to(0.0000001, "1E-7", ScientificMode, 7);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_float_prints_to(0.0000001f, "0.0000001", DecimalMode, 7);
  assert_float_prints_to(0.0000001, "1E-7", ScientificMode, 14);
  assert_float_prints_to(0.0000001, "0.0000001", DecimalMode, 14);

  assert_float_prints_to(-0.000000000000000000000000000000009090018f, "-9.090018E-33", ScientificMode, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018f, "-9.090018E-33", DecimalMode, 7);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018E-33", ScientificMode, 14);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018E-33", DecimalMode, 14);

  assert_float_prints_to(123.421f, "1.23421E2", ScientificMode, 7);
  assert_float_prints_to(123.421f, "123.4", DecimalMode, 4, 6);
  assert_float_prints_to(123.421f, "1.2E2", ScientificMode, 4, 6);

  assert_float_prints_to(9.999999f, "1E1", ScientificMode, 6);
  assert_float_prints_to(9.999999f, "10", DecimalMode, 6);
  assert_float_prints_to(9.999999f, "9.999999", ScientificMode, 7);
  assert_float_prints_to(9.999999f, "9.999999", DecimalMode, 7);

  assert_float_prints_to(-9.99999904f, "-1E1", ScientificMode, 6);
  assert_float_prints_to(-9.99999904f, "-10", DecimalMode, 6);
  assert_float_prints_to(-9.99999904, "-9.999999", ScientificMode, 7);
  assert_float_prints_to(-9.99999904, "-9.999999", DecimalMode, 7);

  assert_float_prints_to(-0.017452f, "-1.745E-2", ScientificMode, 4);
  assert_float_prints_to(-0.017452f, "-0.01745", DecimalMode, 4);
  assert_float_prints_to(-0.017452, "-1.7452E-2", ScientificMode, 14);
  assert_float_prints_to(-0.017452, "-0.017452", DecimalMode, 14);

  assert_float_prints_to(1E50, "1E50", ScientificMode, 9);
  assert_float_prints_to(1E50, "1E50", DecimalMode, 9);
  assert_float_prints_to(1E50, "1E50", ScientificMode, 14);
  assert_float_prints_to(1E50, "1E50", DecimalMode, 14);

  assert_float_prints_to(100.0, "1E2", ScientificMode, 9);
  assert_float_prints_to(100.0, "100", DecimalMode, 9);

  assert_float_prints_to(12345.678910121314f, "1.234568E4", ScientificMode, 7);
  assert_float_prints_to(12345.678910121314f, "12345.68", DecimalMode, 7);
  assert_float_prints_to(12345.678910121314, "1.2345678910121E4", ScientificMode, 14);
  assert_float_prints_to(12345.678910121314, "12345.678910121", DecimalMode, 14);

  assert_float_prints_to(9.999999999999999999999E12, "1E13", ScientificMode, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1E13", DecimalMode, 9);
  assert_float_prints_to(9.999999999999999999999E12, "1E13", ScientificMode, 14);
  assert_float_prints_to(9.999999999999999999999E12, "10000000000000", DecimalMode, 14);

  assert_float_prints_to(-0.000000099999999f, "-1E-7", ScientificMode, 7);
  assert_float_prints_to(-0.000000099999999f, "-0.0000001", DecimalMode, 7);
  assert_float_prints_to(-0.000000099999999, "-9.9999999E-8", ScientificMode, 9);
  assert_float_prints_to(-0.000000099999999, "-0.000000099999999", DecimalMode, 9);

  assert_float_prints_to(999.99999999999977f, "1E3", ScientificMode, 5);
  assert_float_prints_to(999.99999999999977f, "1000", DecimalMode, 5);
  assert_float_prints_to(999.99999999999977, "1E3", ScientificMode, 14);
  assert_float_prints_to(999.99999999999977, "1000", DecimalMode, 14);

  assert_float_prints_to(0.000000999999997, "1E-6", ScientificMode, 7);
  assert_float_prints_to(0.000000999999997, "0.000001", DecimalMode, 7);
  assert_float_prints_to(9999999.97, "1E7", DecimalMode, 7);
  assert_float_prints_to(9999999.97, "10000000", DecimalMode, 8);
}

QUIZ_CASE(poincare_rational_to_text) {
  assert_expression_prints_to(Rational(2,3), "2/3");
  assert_expression_prints_to(Rational("12345678910111213","123456789101112131"), "12345678910111213/123456789101112131");
  assert_expression_prints_to(Rational("123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789","1"), "123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789");
}

QUIZ_CASE(poincare_decimal_to_text) {
  Decimal d0(Integer("-123456789"),30);
  assert_expression_prints_to(d0, "-1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(d0, "-1.234568E30", DecimalMode, 7);
  Decimal d1(Integer("123456789"),30);
  assert_expression_prints_to(d1, "1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(d1, "1.235E30", DecimalMode, 4);
  Decimal d2(Integer("-123456789"),-30);
  assert_expression_prints_to(d2, "-1.23456789E-30", DecimalMode, 14);
  assert_expression_prints_to(d2, "-1.235E-30", ScientificMode, 4);
  Decimal d3(Integer("-12345"),-3);
  assert_expression_prints_to(d3, "-0.0012345", DecimalMode, 7);
  assert_expression_prints_to(d3, "-0.00123", DecimalMode, 3);
  assert_expression_prints_to(d3, "-0.001235", DecimalMode, 4);
  assert_expression_prints_to(d3, "-1.23E-3", ScientificMode, 3);
  Decimal d4(Integer("12345"),-3);
  assert_expression_prints_to(d4, "0.0012345", DecimalMode, 7);
  assert_expression_prints_to(d4, "1.2E-3", ScientificMode, 2);
  Decimal d5(Integer("12345"),3);
  assert_expression_prints_to(d5, "1234.5", DecimalMode, 7);
  assert_expression_prints_to(d5, "1.23E3", DecimalMode, 3);
  assert_expression_prints_to(d5, "1235", DecimalMode, 4);
  assert_expression_prints_to(d5, "1.235E3", ScientificMode, 4);
  Decimal d6(Integer("-12345"),3);
  assert_expression_prints_to(d6, "-1234.5", DecimalMode, 7);
  assert_expression_prints_to(d6, "-1.2345E3", ScientificMode, 10);
  Decimal d7(Integer("12345"),6);
  assert_expression_prints_to(d7, "1234500", DecimalMode, 7);
  assert_expression_prints_to(d7, "1.2345E6", DecimalMode, 6);
  assert_expression_prints_to(d7, "1.2345E6", ScientificMode);
  Decimal d8(Integer("-12345"),6);
  assert_expression_prints_to(d8, "-1234500", DecimalMode, 7);
  assert_expression_prints_to(d8, "-1.2345E6", DecimalMode, 5);
  assert_expression_prints_to(d7, "1.235E6", ScientificMode, 4);
  Decimal d9(Integer("-12345"),-1);
  assert_expression_prints_to(d9, "-0.12345", DecimalMode, 7);
  assert_expression_prints_to(d9, "-0.1235", DecimalMode, 4);
  assert_expression_prints_to(d9, "-1.235E-1", ScientificMode, 4);
  Decimal d10(Integer("12345"),-1);
  assert_expression_prints_to(d10, "1.2345E-1");
  assert_expression_prints_to(d10, "0.12345", DecimalMode, 7);
  assert_expression_prints_to(d10, "0.1235", DecimalMode, 4);
  assert_expression_prints_to(d10, "1.235E-1", ScientificMode, 4);

  assert_expression_prints_to(Decimal(-1.23456789E30), "-1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(Decimal(1.23456789E30), "1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(Decimal(-1.23456789E-30), "-1.23456789E-30", ScientificMode, 14);
  assert_expression_prints_to(Decimal(-1.2345E-3), "-0.0012345", DecimalMode);
  assert_expression_prints_to(Decimal(1.2345E-3), "0.0012345", DecimalMode);
  assert_expression_prints_to(Decimal(1.2345E3), "1234.5", DecimalMode);
  assert_expression_prints_to(Decimal(-1.2345E3), "-1234.5", DecimalMode);
  assert_expression_prints_to(Decimal(1.2345E6), "1234500", DecimalMode);
  assert_expression_prints_to(Decimal(-1.2345E6), "-1234500", DecimalMode);
  assert_expression_prints_to(Decimal(-1.2345E-1), "-0.12345", DecimalMode);
  assert_expression_prints_to(Decimal(1.2345E-1), "0.12345", DecimalMode);
  assert_expression_prints_to(Decimal(1.0), "1");
  assert_expression_prints_to(Decimal(0.9999999999999996), "1");
  assert_expression_prints_to(Decimal(0.99999999999995), "9.9999999999995E-1", ScientificMode, 14);
  assert_expression_prints_to(Decimal(0.00000099999999999995), "9.9999999999995E-7", ScientificMode, 14);
  assert_expression_prints_to(Decimal(0.000000999999999999995), "0.000001", DecimalMode);
  assert_expression_prints_to(Decimal(0.000000999999999901200121020102010201201201021099995), "9.999999999012E-7", DecimalMode, 14);
  assert_expression_prints_to(Decimal(9999999999999.54), "9999999999999.5", DecimalMode, 14);
  assert_expression_prints_to(Decimal(99999999999999.54), "1E14", DecimalMode, 14);
  assert_expression_prints_to(Decimal(999999999999999.54), "1E15", DecimalMode, 14);
  assert_expression_prints_to(Decimal(9999999999999999.54), "1E16", DecimalMode, 14);
  assert_expression_prints_to(Decimal(-9.702365051313E-297), "-9.702365051313E-297", DecimalMode, 14);
}

QUIZ_CASE(poincare_approximation_to_text) {
  assert_expression_prints_to(Float<double>(-1.23456789E30), "-1.23456789E30", DecimalMode, 14);
  assert_expression_prints_to(Float<double>(1.23456789E30), "1.23456789E30", DecimalMode, 14);
  assert_expression_prints_to(Float<double>(-1.23456789E-30), "-1.23456789E-30", DecimalMode, 14);
  assert_expression_prints_to(Float<double>(-1.2345E-3), "-0.0012345", DecimalMode);
  assert_expression_prints_to(Float<double>(1.2345E-3), "0.0012345", DecimalMode);
  assert_expression_prints_to(Float<double>(1.2345E3), "1234.5", DecimalMode);
  assert_expression_prints_to(Float<double>(-1.2345E3), "-1234.5", DecimalMode);
  assert_expression_prints_to(Float<double>(0.99999999999995), "9.9999999999995E-1", ScientificMode, 14);
  assert_expression_prints_to(Float<double>(0.00000099999999999995), "9.9999999999995E-7", DecimalMode, 14);
  assert_expression_prints_to(Float<double>(0.0000009999999999901200121020102010201201201021099995), "9.9999999999012E-7", DecimalMode, 14);
  assert_expression_prints_to(Float<float>(1.2345E-1), "0.12345", DecimalMode);
  assert_expression_prints_to(Float<float>(1), "1", DecimalMode);
  assert_expression_prints_to(Float<float>(0.9999999999999995), "1", DecimalMode);
  assert_expression_prints_to(Float<float>(1.2345E6), "1234500", DecimalMode);
  assert_expression_prints_to(Float<float>(-1.2345E6), "-1234500", DecimalMode);
  assert_expression_prints_to(Float<float>(0.0000009999999999999995), "0.000001", DecimalMode);
  assert_expression_prints_to(Float<float>(-1.2345E-1), "-0.12345", DecimalMode);

  assert_expression_prints_to(Float<double>(INFINITY), "inf", DecimalMode);
  assert_expression_prints_to(Float<float>(0.0f), "0", DecimalMode);
  assert_expression_prints_to(Float<float>(NAN), "undef", DecimalMode);

}
