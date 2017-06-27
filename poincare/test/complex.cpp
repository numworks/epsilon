#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>

using namespace Poincare;
constexpr Expression::FloatDisplayMode Decimal = Expression::FloatDisplayMode::Decimal;
constexpr Expression::FloatDisplayMode Scientific = Expression::FloatDisplayMode::Scientific;
constexpr Expression::ComplexFormat Cartesian = Expression::ComplexFormat::Cartesian;
constexpr Expression::ComplexFormat Polar = Expression::ComplexFormat::Polar;

void assert_cartesian_complex_converts_to(float a, float b, const char * result, Expression::FloatDisplayMode mode = Scientific, Expression::ComplexFormat format = Cartesian, int significantDigits = 7, int bufferSize = 13+13+7+1) {
  int tagSize = 8;
  unsigned char tag = 'X';
  char * taggedBuffer = (char *)malloc(bufferSize+2*tagSize);
  memset(taggedBuffer, tag, bufferSize+2*tagSize);
  char * buffer = taggedBuffer + tagSize;

  if (b == 0.0f) {
    Complex::convertFloatToText(a, buffer, bufferSize, significantDigits, mode);
  } else {
    Preferences::sharedPreferences()->setComplexFormat(format);
    Preferences::sharedPreferences()->setDisplayMode(mode);
    Complex::Cartesian(a, b).writeTextInBuffer(buffer, bufferSize);
  }

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
    if (buffer[i] == Ion::Charset::Exponential) {
      buffer[i] = 'e';
    }
    if (buffer[i] ==  Ion::Charset::IComplex) {
      buffer[i] = 'i';
    }
  }

  assert(strcmp(buffer, result) == 0);

  free(taggedBuffer);
}

QUIZ_CASE(poincare_complex_to_text) {
  /* We expect 7 significative numbers but do not display 0 */
  assert_cartesian_complex_converts_to(123.456f, 0.0f, "1.23456E2");
  assert_cartesian_complex_converts_to(1.234567891011f, 0.0f, "1.234568");
  assert_cartesian_complex_converts_to(2.0f, 0.0f, "2");
  assert_cartesian_complex_converts_to(123456789.0f, 0.0f, "1.234568E8");
  assert_cartesian_complex_converts_to(0.00000123456789f, 0.0f, "1.234568E-6");
  assert_cartesian_complex_converts_to(0.99f, 0.0f, "9.9E-1");
  assert_cartesian_complex_converts_to(-123.456789f, 0.0f, "-1.234568E2");
  assert_cartesian_complex_converts_to(-0.000123456789f, 0.0f, "-1.234568E-4");
  assert_cartesian_complex_converts_to(0.0f, 0.0f, "0");
  assert_cartesian_complex_converts_to(10000000000000000000000000000.0f, 0.0f, "1E28");
  /* Converting 10000000000000000000000000000.0f into a decimal display would
   * overflow the number of significant digits set to 7. When this is the case, the
   * display mode is automatically set to scientific. */
  assert_cartesian_complex_converts_to(10000000000000000000000000000.0f, 0.0f, "1E28", Decimal);
  assert_cartesian_complex_converts_to(1000000.0f, 0.0f, "1000000", Decimal);
  assert_cartesian_complex_converts_to(10000000.0f, 0.0f, "1E7", Decimal);
  assert_cartesian_complex_converts_to(0.000001f, 0.0f, "0.000001", Decimal);
  /* Converting 0.00000001f into a decimal display would also overflow the
   * number of significant digits set to 7. */
  assert_cartesian_complex_converts_to(0.0000001f, 0.0f, "1E-7", Decimal);
  /* The conversion of -0.00000000000000000000000000000000909090964f does not
   * give the exact right number because of float represention. The closest
   * exact representation is -9.090897E-33. */
  assert_cartesian_complex_converts_to(-0.00000000000000000000000000000000909090964f, 0.0f, "-9.090897E-33");
  assert_cartesian_complex_converts_to(123.421f, 0.0f, "123.4", Decimal, Cartesian, 4, 6);
  assert_cartesian_complex_converts_to(123.421f, 0.0f, "1.2E2", Decimal, Cartesian, 5, 6);
  assert_cartesian_complex_converts_to(9.999999f, 0.0f, "10", Decimal, Cartesian, 6);
}

QUIZ_CASE(poincare_complex_cartesian_to_text) {
  assert_cartesian_complex_converts_to(1.0f, 2.0f, "1+2*i", Decimal, Cartesian);
  assert_cartesian_complex_converts_to(1.0f, 2.0f, "2.236068*e^(1.107149*i)", Decimal, Polar);
  assert_cartesian_complex_converts_to(-1.3f, 2.444f, "-1.3+2.444*i", Decimal, Cartesian);
  assert_cartesian_complex_converts_to(-1.3f, 2.444f, "2.768237*e^(2.059649*i)", Decimal, Polar);
  assert_cartesian_complex_converts_to(-1.3f, -2.444f, "-1.3-2.444*i", Decimal, Cartesian);
  assert_cartesian_complex_converts_to(64078208.0f, 119229408.0f, "6.407821E7+1.192294E8*i", Decimal, Cartesian);
  assert_cartesian_complex_converts_to(64078208.0f, 119229408.0f, "1.353576E8*e^(1.07765*i)", Decimal, Polar);
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
