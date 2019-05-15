#include <quiz.h>
#include <poincare/decimal.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <poincare/infinity.h>
#include <assert.h>

#include "tree/helpers.h"
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_number_parser) {
  // Integer
  assert_parsed_expression_is("123456789012345678765434567", Rational::Builder("123456789012345678765434567"));
  assert_parsed_expression_is(MaxIntegerString(), Rational::Builder(MaxIntegerString()));

  // Integer parsed in Decimal because they overflow Integer
  assert_parsed_expression_is(OverflowedIntegerString(), Decimal::Builder(Integer("17976931348623"), 308));
  assert_parsed_expression_is("179769313486235590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216", Decimal::Builder(Integer("17976931348624"), 308));

  // Decimal with rounding when digits are above 14
  assert_parsed_expression_is("0.0000012345678901234", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("0.00000123456789012345", Decimal::Builder(Integer("12345678901235"), -6));
  assert_parsed_expression_is("0.00000123456789012341", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("1234567890123.4", Decimal::Builder(Integer("12345678901234"), 12));
  assert_parsed_expression_is("123456789012345.2", Decimal::Builder(Integer("12345678901235"), 14));
  assert_parsed_expression_is("123456789012341.2", Decimal::Builder(Integer("12345678901234"), 14));
  assert_parsed_expression_is("12.34567", Decimal::Builder(Integer("1234567"), 1));
  assert_parsed_expression_is(".999999999999990", Decimal::Builder(Integer("99999999999999"), -1));
  assert_parsed_expression_is("9.99999999999994", Decimal::Builder(Integer("99999999999999"), 0));
  assert_parsed_expression_is("99.9999999999995", Decimal::Builder(Integer("100000000000000"), 2));
  assert_parsed_expression_is("999.999999999999", Decimal::Builder(Integer("100000000000000"), 3));
  assert_parsed_expression_is("9999.99199999999", Decimal::Builder(Integer("99999920000000"), 3));
  assert_parsed_expression_is("99299.9999999999", Decimal::Builder(Integer("99300000000000"), 4));

  // Infinity
  assert_parsed_expression_is("23ᴇ1000", Infinity::Builder(false));
  assert_parsed_expression_is("2.3ᴇ1000", Decimal::Builder(Integer(23), 1000));

  // Zero
  assert_parsed_expression_is("0.23ᴇ-1000", Decimal::Builder(Integer(0), 0));
  assert_parsed_expression_is("0.23ᴇ-999", Decimal::Builder(Integer(23), -1000));
}
