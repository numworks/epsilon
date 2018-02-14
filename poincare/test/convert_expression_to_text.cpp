#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;
constexpr PrintFloat::Mode DecimalDisplay = PrintFloat::Mode::Decimal;
constexpr PrintFloat::Mode ScientificDisplay = PrintFloat::Mode::Scientific;
constexpr Expression::ComplexFormat Cartesian = Expression::ComplexFormat::Cartesian;
constexpr Expression::ComplexFormat Polar = Expression::ComplexFormat::Polar;

template<typename T>
void assert_float_prints_to(T a, const char * result, PrintFloat::Mode mode = ScientificDisplay, int significantDigits = 7, int bufferSize = 250) {
  quiz_print(result);

  int tagSize = 8;
  unsigned char tag = 'O';
  char * taggedBuffer = new char[bufferSize+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  PrintFloat::convertFloatToText<T>(a, buffer, bufferSize, significantDigits, mode);

  for (int i=0; i<tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }
  translate_in_ASCII_chars(buffer);

  assert(strcmp(buffer, result) == 0);

  delete[] taggedBuffer;
}

void assert_expression_prints_to(Expression * e, const char * result, PrintFloat::Mode mode = ScientificDisplay, Expression::ComplexFormat format = Cartesian, int bufferSize = 250) {
  quiz_print(result);

  int tagSize = 8;
  unsigned char tag = 'O';
  char * taggedBuffer = new char[bufferSize+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  Preferences::sharedPreferences()->setComplexFormat(format);
  Preferences::sharedPreferences()->setDisplayMode(mode);
  e->writeTextInBuffer(buffer, bufferSize);
  translate_in_ASCII_chars(buffer);

  for (int i=0; i<tagSize; i++) {
    assert(taggedBuffer[i] == tag || taggedBuffer[i] == 0);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    assert(taggedBuffer[i] == tag || taggedBuffer[i] == 0);
  }

  assert(strcmp(buffer, result) == 0);

  delete[] taggedBuffer;
}

QUIZ_CASE(assert_float_prints_to) {
  /* We expect 7 significative numbers but do not display 0 */
  assert_float_prints_to(123.456f, "1.23456E2");
  assert_float_prints_to(1.234567891011, "1.234568");
  assert_float_prints_to(2.0f, "2");
  assert_float_prints_to(123456789.0, "1.234568E8");
  assert_float_prints_to(0.00000123456789f, "1.234568E-6");
  assert_float_prints_to(0.99, "9.9E-1");
  assert_float_prints_to(-123.456789f, "-1.234568E2");
  assert_float_prints_to(-0.000123456789, "-1.234568E-4");
  assert_float_prints_to(0.0f, "0");
  assert_float_prints_to(10000000000000000000000000000.0, "1E28");
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case, the
   * display mode is automatically set to scientific. */
  assert_float_prints_to(10000000000000000000000000000.0, "1E28", DecimalDisplay);
  assert_float_prints_to(1000000.0, "1000000", DecimalDisplay);
  assert_float_prints_to(10000000.0f, "1E7", DecimalDisplay);
  assert_float_prints_to(0.000001, "0.000001", DecimalDisplay);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_float_prints_to(0.0000001f, "1E-7", DecimalDisplay);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018E-33");
  assert_float_prints_to(123.421f, "123.4", DecimalDisplay, 4, 6);
  assert_float_prints_to(123.421, "1.2E2", DecimalDisplay, 5, 6);
  assert_float_prints_to(9.999999f, "10", DecimalDisplay, 6);
  assert_float_prints_to(-9.99999904, "-10", DecimalDisplay, 6);
  assert_float_prints_to(-0.017452, "-0.01745", DecimalDisplay, 6);
  assert_float_prints_to(1E50, "1E50", DecimalDisplay, 9);
  assert_float_prints_to(100.0, "100", DecimalDisplay, 9);
  assert_float_prints_to(12345.678910121314, "12345.678910121", DecimalDisplay, 14);
  assert_float_prints_to(9.999999999999999999999E12, "1E13", DecimalDisplay, 9);
  assert_float_prints_to(-0.000000099999999, "-0.0000001", DecimalDisplay, 9);
}

QUIZ_CASE(poincare_rational_to_text) {
  Rational r0(2,3);
  assert_expression_prints_to(&r0, "2/3");
  Rational r1("12345678910111213","123456789101112131");
  assert_expression_prints_to(&r1, "12345678910111213/123456789101112131");
  Rational r2("123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789","1");
  assert_expression_prints_to(&r2, "123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789");
}

QUIZ_CASE(poincare_decimal_to_text) {
  Decimal d0(Integer("-123456789"),30);
  assert_expression_prints_to(&d0, "-1.23456789E30");
  Decimal d1(Integer("123456789"),30);
  assert_expression_prints_to(&d1, "1.23456789E30");
  Decimal d2(Integer("-123456789"),-30);
  assert_expression_prints_to(&d2, "-1.23456789E-30");
  Decimal d3(Integer("-12345"),-3);
  assert_expression_prints_to(&d3, "-0.0012345");
  Decimal d4(Integer("12345"),-3);
  assert_expression_prints_to(&d4, "0.0012345");
  Decimal d5(Integer("12345"),3);
  assert_expression_prints_to(&d5, "1234.5");
  Decimal d6(Integer("-12345"),3);
  assert_expression_prints_to(&d6, "-1234.5");
  Decimal d7(Integer("12345"),6);
  assert_expression_prints_to(&d7, "1234500");
  Decimal d8(Integer("-12345"),6);
  assert_expression_prints_to(&d8, "-1234500");
  Decimal d9(Integer("-12345"),-1);
  assert_expression_prints_to(&d9, "-0.12345");
  Decimal d10(Integer("12345"),-1);
  assert_expression_prints_to(&d10, "0.12345");

  Decimal e0(-1.23456789E30);
  assert_expression_prints_to(&e0, "-1.23456789E30");
  Decimal e1(1.23456789E30);
  assert_expression_prints_to(&e1, "1.23456789E30");
  Decimal e2(-1.23456789E-30);
  assert_expression_prints_to(&e2, "-1.23456789E-30");
  Decimal e3(-1.2345E-3);
  assert_expression_prints_to(&e3, "-0.0012345");
  Decimal e4(1.2345E-3);
  assert_expression_prints_to(&e4, "0.0012345");
  Decimal e5(1.2345E3);
  assert_expression_prints_to(&e5, "1234.5");
  Decimal e6(-1.2345E3);
  assert_expression_prints_to(&e6, "-1234.5");
  Decimal e7(1.2345E6);
  assert_expression_prints_to(&e7, "1234500");
  Decimal e8(-1.2345E6);
  assert_expression_prints_to(&e8, "-1234500");
  Decimal e9(-1.2345E-1);
  assert_expression_prints_to(&e9, "-0.12345");
  Decimal e10(1.2345E-1);
  assert_expression_prints_to(&e10, "0.12345");
  Decimal e11(1);
  assert_expression_prints_to(&e11, "1");
  Decimal e12(0.9999999999999995);
  assert_expression_prints_to(&e12, "1");
  Decimal e13(0.999999999999995);
  assert_expression_prints_to(&e13, "9.99999999999995E-1");
  Decimal e14(0.000000999999999999995);
  assert_expression_prints_to(&e14, "9.99999999999995E-7");
  Decimal e15(0.0000009999999999999995);
  assert_expression_prints_to(&e15, "0.000001");
  Decimal e16(0.0000009999999999901200121020102010201201201021099995);
  assert_expression_prints_to(&e16, "9.9999999999012E-7");
}

QUIZ_CASE(poincare_complex_to_text) {
  Complex<double> c0 = Complex<double>::Cartesian(1.0, 2.0);
  assert_expression_prints_to(&c0, "1+2*I", DecimalDisplay, Cartesian);
  Complex<double> c1 = Complex<double>::Cartesian(1.0, 2.0);
  assert_expression_prints_to(&c1, "2.2360679774998*X^(1.1071487177941*I)", DecimalDisplay, Polar);
  Complex<double> c2 = Complex<double>::Cartesian(-1.3, 2.444);
  assert_expression_prints_to(&c2, "-1.3+2.444*I", DecimalDisplay, Cartesian);
  Complex<double> c3 = Complex<double>::Cartesian(-1.3, 2.444);
  assert_expression_prints_to(&c3, "2.7682369840749*X^(2.0596486811226*I)", DecimalDisplay, Polar);
  Complex<double> c4 = Complex<double>::Cartesian(-1.3, -2.444);
  assert_expression_prints_to(&c4, "-1.3-2.444*I", DecimalDisplay, Cartesian);
  Complex<double> c5 = Complex<double>::Cartesian(64078208.0, 119229408.0);
  assert_expression_prints_to(&c5, "64078208+119229408*I", DecimalDisplay, Cartesian);
  Complex<double> c6 = Complex<double>::Cartesian(64078208.0, 119229408.0);
  assert_expression_prints_to(&c6, "135357557.86997*X^(1.0776501182461*I)", DecimalDisplay, Polar);
  Complex<double> c7 = Complex<double>::Cartesian(INFINITY, 119229408.0);
  assert_expression_prints_to(&c7, "undef", DecimalDisplay, Polar);
  Complex<float> c8 = Complex<float>::Cartesian(0.0f, 0.0f);
  assert_expression_prints_to(&c8, "0", DecimalDisplay, Polar);
  Complex<float> c9 = Complex<float>::Cartesian(NAN, 0.0f);
  assert_expression_prints_to(&c9, "undef", DecimalDisplay, Polar);
  Complex<float> c10 = Complex<float>::Cartesian(0.0f, NAN);
  assert_expression_prints_to(&c10, "undef", DecimalDisplay, Polar);
  Complex<double> c11 = Complex<double>::Cartesian(NAN, NAN);
  assert_expression_prints_to(&c11, "undef", DecimalDisplay, Polar);

}
