#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <assert.h>

QUIZ_CASE(poincare_float_to_text) {
  char buffer [14];
  Float(123.456f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "1.23456e2") == 0);
  Float(1.234567891011f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "1.234568e0") == 0);
  Float(2.0f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "2.e0") == 0);
  Float(123456789.0f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "1.234568e8") == 0);
  Float(0.00000123456789f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "1.234568e-6") == 0);
  Float(0.99f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "9.9e-1") == 0);
  Float(-123.456789f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "-1.234568e2") == 0);
  Float(-0.000123456789f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "-1.234568e-4") == 0);
  Float(0.0f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "0.e0") == 0);
  Float(10000000000000000000000000000.0f).convertFloatToText(buffer, 14, 7);
  assert(strcmp(buffer, "1.e28") == 0);
}

QUIZ_CASE(poincare_float_approximate) {
  Context context;
  Expression * a = new Float(123.456f);
  assert(a->approximate(context) == 123.456f);
  delete a;
}

QUIZ_CASE(poincare_float_evaluate) {
  Context context;
  Expression * a = new Float(123.456f);
  Expression * e = a->evaluate(context);
  assert(e->approximate(context) == 123.456f);
  delete a;
  delete e;
}
