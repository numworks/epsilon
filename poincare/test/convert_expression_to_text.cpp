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
void assert_float_prints_to(T a, const char * result, PrintFloat::Mode mode = ScientificMode, int significantDigits = 7, int bufferSize = 250) {
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

void assert_expression_prints_to(Expression * e, const char * result, PrintFloat::Mode mode = ScientificMode, int numberOfSignificantDigits = 7, int bufferSize = 250) {
  quiz_print(result);

  int tagSize = 8;
  unsigned char tag = 'O';
  char * taggedBuffer = new char[bufferSize+2*tagSize];
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  e->writeTextInBuffer(buffer, bufferSize, mode, numberOfSignificantDigits);
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

template<typename T>
void assert_approximation_prints_to(Approximation<T> * e, const char * result, Expression::ComplexFormat complexFormat = Cartesian) {
  GlobalContext globalContext;
  Expression * approximation = e->template approximate<T>(globalContext, Radian, complexFormat);
  assert_expression_prints_to(approximation, result, DecimalMode, (sizeof(T) == sizeof(float) ? 7 : 14));
  delete approximation;
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
  assert_float_prints_to(10000000000000000000000000000.0, "1E28", DecimalMode);
  assert_float_prints_to(1000000.0, "1000000", DecimalMode);
  assert_float_prints_to(10000000.0f, "1E7", DecimalMode);
  assert_float_prints_to(0.000001, "0.000001", DecimalMode);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_float_prints_to(0.0000001f, "1E-7", DecimalMode);
  assert_float_prints_to(-0.000000000000000000000000000000009090018, "-9.090018E-33");
  assert_float_prints_to(123.421f, "123.4", DecimalMode, 4, 6);
  assert_float_prints_to(123.421, "1.2E2", DecimalMode, 5, 6);
  assert_float_prints_to(9.999999f, "10", DecimalMode, 6);
  assert_float_prints_to(-9.99999904, "-10", DecimalMode, 6);
  assert_float_prints_to(-0.017452, "-0.01745", DecimalMode, 6);
  assert_float_prints_to(1E50, "1E50", DecimalMode, 9);
  assert_float_prints_to(100.0, "100", DecimalMode, 9);
  assert_float_prints_to(12345.678910121314, "12345.678910121", DecimalMode, 14);
  assert_float_prints_to(9.999999999999999999999E12, "1E13", DecimalMode, 9);
  assert_float_prints_to(-0.000000099999999, "-0.0000001", DecimalMode, 9);
  assert_float_prints_to(999.99999999999977, "1000", DecimalMode, 5);
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
  assert_expression_prints_to(&d0, "-1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(&d0, "-1.234568E30", DecimalMode, 7);
  Decimal d1(Integer("123456789"),30);
  assert_expression_prints_to(&d1, "1.23456789E30", ScientificMode, 14);
  assert_expression_prints_to(&d1, "1.235E30", DecimalMode, 4);
  Decimal d2(Integer("-123456789"),-30);
  assert_expression_prints_to(&d2, "-1.23456789E-30", DecimalMode, 14);
  assert_expression_prints_to(&d2, "-1.235E-30", ScientificMode, 4);
  Decimal d3(Integer("-12345"),-3);
  assert_expression_prints_to(&d3, "-0.0012345", DecimalMode, 7);
  assert_expression_prints_to(&d3, "-0.00123", DecimalMode, 3);
  assert_expression_prints_to(&d3, "-0.001235", DecimalMode, 4);
  assert_expression_prints_to(&d3, "-1.23E-3", ScientificMode, 3);
  Decimal d4(Integer("12345"),-3);
  assert_expression_prints_to(&d4, "0.0012345", DecimalMode, 7);
  assert_expression_prints_to(&d4, "1.2E-3", ScientificMode, 2);
  Decimal d5(Integer("12345"),3);
  assert_expression_prints_to(&d5, "1234.5", DecimalMode, 7);
  assert_expression_prints_to(&d5, "1.23E3", DecimalMode, 3);
  assert_expression_prints_to(&d5, "1235", DecimalMode, 4);
  assert_expression_prints_to(&d5, "1.235E3", ScientificMode, 4);
  Decimal d6(Integer("-12345"),3);
  assert_expression_prints_to(&d6, "-1234.5", DecimalMode, 7);
  assert_expression_prints_to(&d6, "-1.2345E3", ScientificMode, 10);
  Decimal d7(Integer("12345"),6);
  assert_expression_prints_to(&d7, "1234500", DecimalMode, 7);
  assert_expression_prints_to(&d7, "1.2345E6", DecimalMode, 6);
  assert_expression_prints_to(&d7, "1.2345E6", ScientificMode);
  Decimal d8(Integer("-12345"),6);
  assert_expression_prints_to(&d8, "-1234500", DecimalMode, 7);
  assert_expression_prints_to(&d8, "-1.2345E6", DecimalMode, 5);
  assert_expression_prints_to(&d7, "1.235E6", ScientificMode, 4);
  Decimal d9(Integer("-12345"),-1);
  assert_expression_prints_to(&d9, "-0.12345", DecimalMode, 7);
  assert_expression_prints_to(&d9, "-0.1235", DecimalMode, 4);
  assert_expression_prints_to(&d9, "-1.235E-1", ScientificMode, 4);
  Decimal d10(Integer("12345"),-1);
  assert_expression_prints_to(&d10, "0.12345");
  assert_expression_prints_to(&d10, "0.12345", DecimalMode, 7);
  assert_expression_prints_to(&d10, "0.1235", DecimalMode, 4);
  assert_expression_prints_to(&d10, "1.235E-1", ScientificMode, 4);

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
  Decimal e11(1.0);
  assert_expression_prints_to(&e11, "1");
  Decimal e12(0.9999999999999996);
  assert_expression_prints_to(&e12, "1");
  Decimal e13(0.99999999999995);
  assert_expression_prints_to(&e13, "9.9999999999995E-1");
  Decimal e14(0.00000099999999999995);
  assert_expression_prints_to(&e14, "9.9999999999995E-7");
  Decimal e15(0.000000999999999999995);
  assert_expression_prints_to(&e15, "0.000001");
  Decimal e16(0.000000999999999901200121020102010201201201021099995);
  assert_expression_prints_to(&e16, "9.999999999012E-7");
}

QUIZ_CASE(poincare_approximation_to_text) {
  Approximation<double> e0(-1.23456789E30);
  assert_approximation_prints_to<double>(&e0, "-1.23456789E30");
  Approximation<double> e1(1.23456789E30);
  assert_approximation_prints_to<double>(&e1, "1.23456789E30");
  Approximation<double> e2(-1.23456789E-30);
  assert_approximation_prints_to<double>(&e2, "-1.23456789E-30");
  Approximation<double> e3(-1.2345E-3);
  assert_approximation_prints_to<double>(&e3, "-0.0012345");
  Approximation<double> e4(1.2345E-3);
  assert_approximation_prints_to<double>(&e4, "0.0012345");
  Approximation<double> e5(1.2345E3);
  assert_approximation_prints_to<double>(&e5, "1234.5");
  Approximation<double> e6(-1.2345E3);
  assert_approximation_prints_to<double>(&e6, "-1234.5");
  Approximation<double> e7(0.99999999999995);
  assert_approximation_prints_to<double>(&e7, "9.9999999999995E-1");
  Approximation<double> e8(0.00000099999999999995);
  assert_approximation_prints_to<double>(&e8, "9.9999999999995E-7");
  Approximation<double> e9(0.0000009999999999901200121020102010201201201021099995);
  assert_approximation_prints_to<double>(&e9, "9.9999999999012E-7");
  Approximation<float> e10(1.2345E-1);
  assert_approximation_prints_to<float>(&e10, "0.12345");
  Approximation<float> e11(1);
  assert_approximation_prints_to<float>(&e11, "1");
  Approximation<float> e12(0.9999999999999995);
  assert_approximation_prints_to<float>(&e12, "1");
  Approximation<float> e13(1.2345E6);
  assert_approximation_prints_to<float>(&e13, "1234500");
  Approximation<float> e14(-1.2345E6);
  assert_approximation_prints_to<float>(&e14, "-1234500");
  Approximation<float> e15(0.0000009999999999999995);
  assert_approximation_prints_to<float>(&e15, "0.000001");
  Approximation<float> e16(-1.2345E-1);
  assert_approximation_prints_to<float>(&e16, "-0.12345");

  Approximation<double> c0(1.0, 2.0);
  assert_approximation_prints_to<double>(&c0, "1+2*I", Cartesian);
  Approximation<double> c1(1.0, 2.0);
  assert_approximation_prints_to<double>(&c1, "2.2360679774998*X^(1.1071487177941*I)", Polar);
  Approximation<double> c2(-1.3, 2.444);
  assert_approximation_prints_to<double>(&c2, "(-1.3)+2.444*I", Cartesian); // TODO: get rid of parenthesis
  Approximation<double> c3(-1.3, 2.444);
  assert_approximation_prints_to<double>(&c3, "2.7682369840749*X^(2.0596486811226*I)", Polar);
  Approximation<double> c4(-1.3, -2.444);
  assert_approximation_prints_to<double>(&c4, "(-1.3)-2.444*I", Cartesian); // TODO: get rid of parenthesis
  Approximation<double> c5(64078208.0, 119229408.0);
  assert_approximation_prints_to<double>(&c5, "64078208+119229408*I", Cartesian);
  Approximation<double> c6(64078208.0, 119229408.0);
  assert_approximation_prints_to<double>(&c6, "135357557.86997*X^(1.0776501182461*I)", Polar);
  Approximation<double> c7(INFINITY, 119229408.0);
  assert_approximation_prints_to<double>(&c7, "undef", Polar);
  assert_approximation_prints_to<double>(&c7, "undef", Cartesian);
  Approximation<float> c8(0.0f, 0.0f);
  assert_approximation_prints_to<float>(&c8, "0", Polar);
  assert_approximation_prints_to<float>(&c8, "0", Cartesian);
  Approximation<float> c9(NAN, 0.0f);
  assert_approximation_prints_to<float>(&c9, "undef", Polar);
  assert_approximation_prints_to<float>(&c9, "undef", Cartesian);
  Approximation<float> c10(0.0f, NAN);
  assert_approximation_prints_to<float>(&c10, "undef", Polar);
  assert_approximation_prints_to<float>(&c10, "undef", Cartesian);
  Approximation<double> c11(NAN, NAN);
  assert_approximation_prints_to<double>(&c11, "undef", Polar);
  assert_approximation_prints_to<double>(&c11, "undef", Cartesian);

}
