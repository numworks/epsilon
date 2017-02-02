#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <assert.h>

QUIZ_CASE(poincare_complex_to_text) {
  char buffer[14];
  Complex::convertFloatToText(123.456f,buffer, 14, 7);
  assert(strcmp(buffer, "1.23456E2") == 0);
  Complex::convertFloatToText(1.234567891011f,buffer, 14, 7);
  assert(strcmp(buffer, "1.234568E0") == 0);
  Complex::convertFloatToText(2.0f, buffer, 14, 7);
  assert(strcmp(buffer, "2.0E0") == 0);
  Complex::convertFloatToText(123456789.0f, buffer, 14, 7);
  assert(strcmp(buffer, "1.234568E8") == 0);
  Complex::convertFloatToText(0.00000123456789f, buffer, 14, 7);
  assert(strcmp(buffer, "1.234568E-6") == 0);
  Complex::convertFloatToText(0.99f, buffer, 14, 7);
  assert(strcmp(buffer, "9.9E-1") == 0);
  Complex::convertFloatToText(-123.456789f, buffer, 14, 7);
  assert(strcmp(buffer, "-1.234568E2") == 0);
  Complex::convertFloatToText(-0.000123456789f, buffer, 14, 7);
  assert(strcmp(buffer, "-1.234568E-4") == 0);
  Complex::convertFloatToText(0.0f, buffer, 14, 7);
  assert(strcmp(buffer, "0.0E0") == 0);
  Complex::convertFloatToText(10000000000000000000000000000.0f, buffer, 14, 7);
  assert(strcmp(buffer, "1.0E28") == 0);
  Complex::convertFloatToText(10000000000000000000000000000.0f, buffer, 14, 7, Expression::DisplayMode::Auto);
  assert(strcmp(buffer, "1.0E28") == 0);
  Complex::convertFloatToText(1000000.0f, buffer, 14, 7, Expression::DisplayMode::Auto);
  assert(strcmp(buffer, "1000000") == 0);
  Complex::convertFloatToText(10000000.0f, buffer, 14, 7, Expression::DisplayMode::Auto);
  assert(strcmp(buffer, "1.0E7") == 0);
  Complex::convertFloatToText(0.000001f, buffer, 14, 7, Expression::DisplayMode::Auto);
  assert(strcmp(buffer, "0.000001") == 0);
  Complex::convertFloatToText(0.0000001f, buffer, 14, 7, Expression::DisplayMode::Auto);
  assert(strcmp(buffer, "1.0E-7") == 0);
  char buffer2[6];
  Complex::convertFloatToText(123.421f, buffer2, 6, 4, Expression::DisplayMode::Auto);
  assert(strcmp(buffer2, "123.4") == 0);
  char buffer3[6];
  Complex::convertFloatToText(123.421f, buffer3, 6, 5, Expression::DisplayMode::Auto);
  assert(strcmp(buffer3, "1.2E2") == 0);
}

QUIZ_CASE(poincare_complex_approximate) {
  GlobalContext globalContext;
  Expression * a = new Complex(123.456f);
  assert(a->approximate(globalContext) == 123.456f);
  delete a;
}

QUIZ_CASE(poincare_complex_evaluate) {
  GlobalContext globalContext;
  Expression * a = new Complex(123.456f);
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 123.456f);
  delete a;
  delete e;
}
