#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;

void assert_expression_prints_to(Expression * e, const char * result, int bufferSize = 100) {
  quiz_print(result);

  char * buffer = new char[bufferSize];
  e->writeTextInBuffer(buffer, bufferSize);

  char * currentChar = buffer;
  while (*currentChar != 0) {
    if (*currentChar == Ion::Charset::Exponent) {
      *currentChar = 'E';
    }
    if (*currentChar == Ion::Charset::Exponential) {
      *currentChar = 'e';
    }
    if (*currentChar ==  Ion::Charset::IComplex) {
      *currentChar = 'i';
    }
    currentChar++;
  }

  assert(strcmp(buffer, result) == 0);

  delete[] buffer;
}

QUIZ_CASE(poincare_rational_to_text) {
  Rational r(2,3);
  assert_expression_prints_to(&r, "2/3");
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
}
