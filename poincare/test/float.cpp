#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/print_float.h>
#include <poincare/float.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_float_evaluates_to(Float<T> f, const char * result) {
  Shared::GlobalContext globalContext;
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  char buffer[500];
  f.template approximate<T>(&globalContext, Cartesian, Radian).serialize(buffer, sizeof(buffer), DecimalMode, numberOfDigits);
  quiz_assert(strcmp(buffer, result) == 0);
}

QUIZ_CASE(poincare_float_evaluate) {
  assert_float_evaluates_to<double>(Float<double>::Builder(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_evaluates_to<double>(Float<double>::Builder(1.23456789E30), "1.23456789ᴇ30");
  assert_float_evaluates_to<double>(Float<double>::Builder(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_evaluates_to<double>(Float<double>::Builder(-1.2345E-3), "-0.0012345");
  assert_float_evaluates_to<double>(Float<double>::Builder(1.2345E-3), "0.0012345");
  assert_float_evaluates_to<double>(Float<double>::Builder(1.2345E3), "1234.5");
  assert_float_evaluates_to<double>(Float<double>::Builder(-1.2345E3), "-1234.5");
  assert_float_evaluates_to<double>(Float<double>::Builder(0.99999999999995), "9.9999999999995ᴇ-1");
  assert_float_evaluates_to<double>(Float<double>::Builder(0.00000099999999999995), "9.9999999999995ᴇ-7");
  assert_float_evaluates_to<double>(Float<double>::Builder(0.0000009999999999901200121020102010201201201021099995), "9.9999999999012ᴇ-7");
  assert_float_evaluates_to<float>(Float<float>::Builder(1.2345E-1), "0.12345");
  assert_float_evaluates_to<float>(Float<float>::Builder(1), "1");
  assert_float_evaluates_to<float>(Float<float>::Builder(0.9999999999999995), "1");
  assert_float_evaluates_to<float>(Float<float>::Builder(1.2345E6), "1234500");
  assert_float_evaluates_to<float>(Float<float>::Builder(-1.2345E6), "-1234500");
  assert_float_evaluates_to<float>(Float<float>::Builder(0.0000009999999999999995), "0.000001");
  assert_float_evaluates_to<float>(Float<float>::Builder(-1.2345E-1), "-0.12345");

  assert_float_evaluates_to<double>(Float<double>::Builder(INFINITY), Infinity::Name());
  assert_float_evaluates_to<float>(Float<float>::Builder(0.0f), "0");
  assert_float_evaluates_to<float>(Float<float>::Builder(NAN), Undefined::Name());

}
