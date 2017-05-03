#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>

using namespace Poincare;
constexpr Expression::FloatDisplayMode Decimal = Expression::FloatDisplayMode::Decimal;
constexpr Expression::FloatDisplayMode Scientific = Expression::FloatDisplayMode::Scientific;

void assert_float_converts_to(float f, const char * result, Expression::FloatDisplayMode mode = Scientific, int significantDigits = 7, int bufferSize = 14) {
  int tagSize = 8;
  unsigned char tag = 'X';
  char * taggedBuffer = (char *)malloc(bufferSize+2*tagSize);
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  Complex::convertFloatToText(f, buffer, bufferSize, significantDigits, mode);

  for (int i=0; i<tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }
  for (int i=tagSize+strlen(buffer)+1; i<bufferSize+2*tagSize; i++) {
    assert(taggedBuffer[i] == tag);
  }

  for (int i=0; i<bufferSize; i++) {
    if (buffer[i] == Ion::Charset::Exponent) {
      buffer[i] = 'E';
    }
  }

  assert(strcmp(buffer, result) == 0);

  free(taggedBuffer);
}

QUIZ_CASE(poincare_complex_to_text) {
  assert_float_converts_to(123.456f, "1.23456E2"); //FIXME: Only 6 significant digits?
  assert_float_converts_to(1.234567891011f, "1.234568");
  assert_float_converts_to(2.0f, "2");
  assert_float_converts_to(123456789.0f, "1.234568E8");
  assert_float_converts_to(0.00000123456789f, "1.234568E-6");
  assert_float_converts_to(0.99f, "9.9E-1");
  assert_float_converts_to(-123.456789f, "-1.234568E2");
  assert_float_converts_to(-0.000123456789f, "-1.234568E-4");
  assert_float_converts_to(0.0f, "0");
  assert_float_converts_to(10000000000000000000000000000.0f, "1E28");
  assert_float_converts_to(10000000000000000000000000000.0f, "1E28", Decimal); // FIXME: Explain. Would overflow?
  assert_float_converts_to(1000000.0f, "1000000", Decimal);
  assert_float_converts_to(10000000.0f, "1E7", Decimal);
  assert_float_converts_to(0.000001f, "0.000001", Decimal);
  assert_float_converts_to(0.0000001f, "1E-7", Decimal); // FIXME: Explain. Would overflow?
  // CRASH!! assert_float_converts_to(-0.00000000000000000000000000000000909090964f, "WHATEVER", Decimal);
  assert_float_converts_to(123.421f, "123.4", Decimal, 4, 6);
  assert_float_converts_to(123.421f, "1.2E2", Decimal, 5, 6);
}

QUIZ_CASE(poincare_complex_cartesian_to_text) {
  constexpr int bufferSize = 14;
  char buffer[bufferSize] = {0};
  Complex::Cartesian(1.0f, 2.0f).writeTextInBuffer(buffer, bufferSize);
  char text1[bufferSize] = {'1','+', '2', '*', Ion::Charset::IComplex, 0};
  assert(strcmp(buffer, text1) == 0);
  Complex::Cartesian(-1.3f, 2.444f).writeTextInBuffer(buffer, bufferSize);
  char text2[14] = {'-','1','.','3','+', '2','.','4','4','4', '*', Ion::Charset::IComplex, 0};
  assert(strcmp(buffer, text2) == 0);
  Complex::Cartesian(-1.3f, -2.444f).writeTextInBuffer(buffer, 14);
  char text3[14] = {'-','1','.','3','-', '2','.','4','4','4', '*', Ion::Charset::IComplex, 0};
  assert(strcmp(buffer, text3) == 0);
}

QUIZ_CASE(poincare_complex_approximate) {
  GlobalContext globalContext;
  Expression * a = new Complex(Complex::Float(123.456f));
  assert(a->approximate(globalContext) == 123.456f);
  delete a;
}

QUIZ_CASE(poincare_complex_evaluate) {
  GlobalContext globalContext;
  Expression * a = new Complex(Complex::Float(123.456f));
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 123.456f);
  delete a;
  delete e;
}

QUIZ_CASE(poincare_complex_constructor) {
  Complex * a = new Complex(Complex::Cartesian(2.0f, 3.0f));
  assert(fabsf(a->a() - 2.0f) < 0.00001f && fabsf(a->b()-3.0f) < 0.00001f);
  assert(a->r() == 3.60555124f && a->th() == 0.982793748f);
  delete a;

  a = new Complex(Complex::Polar(3.60555124f, 0.982793748f));
  assert(fabsf(a->a() - 2.0f) < 0.00001f && fabsf(a->b()-3.0f) < 0.00001f);
  delete a;
}
