#include "helper.h"
#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/division.h>
#include <poincare/factorial.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/opposite.h>
#include <poincare/percent.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

using namespace Poincare;

QUIZ_CASE(poincare_serialization_based_integer) {
  assert_expression_serialize_to(BasedInteger::Builder(Integer(23), Integer::Base::Decimal), "23");
  assert_expression_serialize_to(BasedInteger::Builder(Integer(23), Integer::Base::Binary), "0b10111");
  assert_expression_serialize_to(BasedInteger::Builder(Integer(23), Integer::Base::Hexadecimal), "0x17");
}

QUIZ_CASE(poincare_serialization_rational) {
  assert_expression_serialize_to(Rational::Builder(2,3), "2/3");
  assert_expression_serialize_to(Rational::Builder("12345678910111213","123456789101112131"), "12345678910111213/123456789101112131");
  assert_expression_serialize_to(Rational::Builder("123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789","1"), "123456789112345678921234567893123456789412345678951234567896123456789612345678971234567898123456789912345678901234567891123456789212345678931234567894123456789512345678961234567896123456789712345678981234567899123456789");
  assert_expression_serialize_to(Rational::Builder(-2, 3), "-2/3");
  assert_expression_serialize_to(Rational::Builder("2345678909876"), "2345678909876");
  assert_expression_serialize_to(Rational::Builder("-2345678909876", "5"), "-2345678909876/5");
  assert_expression_serialize_to(Rational::Builder(MaxIntegerString()), MaxIntegerString());
  Integer one(1);
  Integer overflow = Integer::Overflow(false);
  assert_expression_serialize_to(Rational::Builder(one, overflow), "1/inf");
  assert_expression_serialize_to(Rational::Builder(overflow), Infinity::Name());
}

QUIZ_CASE(poincare_serialization_decimal) {
  Decimal d0 = Decimal::Builder(Integer("-123456789"),30);
  assert_expression_serialize_to(d0, "-1.23456789ᴇ30", ScientificMode, 14);
  assert_expression_serialize_to(d0, "-1.234568ᴇ30", DecimalMode, 7);
  assert_expression_serialize_to(d0, "-1.23456789ᴇ30", EngineeringMode, 14);
  Decimal d1 = Decimal::Builder(Integer("123456789"),30);
  assert_expression_serialize_to(d1, "1.23456789ᴇ30", ScientificMode, 14);
  assert_expression_serialize_to(d1, "1.235ᴇ30", DecimalMode, 4);
  assert_expression_serialize_to(d1, "1.23456789ᴇ30", EngineeringMode, 14);
  Decimal d2 = Decimal::Builder(Integer("-123456789"),-30);
  assert_expression_serialize_to(d2, "-1.23456789ᴇ-30", DecimalMode, 14);
  assert_expression_serialize_to(d2, "-1.235ᴇ-30", ScientificMode, 4);
  assert_expression_serialize_to(d2, "-1.235ᴇ-30", EngineeringMode, 4);
  Decimal d3 = Decimal::Builder(Integer("-12345"),-3);
  assert_expression_serialize_to(d3, "-0.0012345", DecimalMode, 7);
  assert_expression_serialize_to(d3, "-0.00123", DecimalMode, 3);
  assert_expression_serialize_to(d3, "-0.001235", DecimalMode, 4);
  assert_expression_serialize_to(d3, "-1.23ᴇ-3", ScientificMode, 3);
  assert_expression_serialize_to(d3, "-1.23ᴇ-3", EngineeringMode, 3);
  Decimal d4 = Decimal::Builder(Integer("12345"),-3);
  assert_expression_serialize_to(d4, "0.0012345", DecimalMode, 7);
  assert_expression_serialize_to(d4, "1.2ᴇ-3", ScientificMode, 2);
  assert_expression_serialize_to(d4, "1.23ᴇ-3", EngineeringMode, 3);
  Decimal d5 = Decimal::Builder(Integer("12345"),3);
  assert_expression_serialize_to(d5, "1234.5", DecimalMode, 7);
  assert_expression_serialize_to(d5, "1.23ᴇ3", DecimalMode, 3);
  assert_expression_serialize_to(d5, "1235", DecimalMode, 4);
  assert_expression_serialize_to(d5, "1.235ᴇ3", ScientificMode, 4);
  assert_expression_serialize_to(d5, "1.235ᴇ3", EngineeringMode, 4);
  Decimal d6 = Decimal::Builder(Integer("-12345"),3);
  assert_expression_serialize_to(d6, "-1234.5", DecimalMode, 7);
  assert_expression_serialize_to(d6, "-1.2345ᴇ3", ScientificMode, 10);
  assert_expression_serialize_to(d6, "-1.2345ᴇ3", EngineeringMode, 10);
  Decimal d7 = Decimal::Builder(Integer("12345"),6);
  assert_expression_serialize_to(d7, "1234500", DecimalMode, 7);
  assert_expression_serialize_to(d7, "1.2345ᴇ6", DecimalMode, 6);
  assert_expression_serialize_to(d7, "1.2345ᴇ6", ScientificMode);
  assert_expression_serialize_to(d7, "1.2345ᴇ6", EngineeringMode);
  Decimal d8 = Decimal::Builder(Integer("-12345"),6);
  assert_expression_serialize_to(d8, "-1234500", DecimalMode, 7);
  assert_expression_serialize_to(d8, "-1.2345ᴇ6", DecimalMode, 5);
  assert_expression_serialize_to(d7, "1.235ᴇ6", ScientificMode, 4);
  assert_expression_serialize_to(d7, "1.235ᴇ6", EngineeringMode, 4);
  Decimal d9 = Decimal::Builder(Integer("-12345"),-1);
  assert_expression_serialize_to(d9, "-0.12345", DecimalMode, 7);
  assert_expression_serialize_to(d9, "-0.1235", DecimalMode, 4);
  assert_expression_serialize_to(d9, "-1.235ᴇ-1", ScientificMode, 4);
  assert_expression_serialize_to(d9, "-123.5ᴇ-3", EngineeringMode, 4);
  Decimal d10 = Decimal::Builder(Integer("12345"),-1);
  assert_expression_serialize_to(d10, "1.2345ᴇ-1");
  assert_expression_serialize_to(d10, "0.12345", DecimalMode, 7);
  assert_expression_serialize_to(d10, "0.1235", DecimalMode, 4);
  assert_expression_serialize_to(d10, "1.235ᴇ-1", ScientificMode, 4);
  assert_expression_serialize_to(d10, "123.5ᴇ-3", EngineeringMode, 4);

  assert_expression_serialize_to(Decimal::Builder(0.25), "250ᴇ-3", EngineeringMode);
  assert_expression_serialize_to(Decimal::Builder(-1.23456789E30), "-1.23456789ᴇ30", ScientificMode, 14);
  assert_expression_serialize_to(Decimal::Builder(1.23456789E30), "1.23456789ᴇ30", ScientificMode, 14);
  assert_expression_serialize_to(Decimal::Builder(-1.23456789E-30), "-1.23456789ᴇ-30", ScientificMode, 14);
  assert_expression_serialize_to(Decimal::Builder(-1.2345E-3), "-0.0012345", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(1.2345E-3), "0.0012345", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(1.2345E3), "1234.5", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(-1.2345E3), "-1234.5", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(1.2345E6), "1234500", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(-1.2345E6), "-1234500", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(-1.2345E-1), "-0.12345", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(1.2345E-1), "0.12345", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(1.0), "1");
  assert_expression_serialize_to(Decimal::Builder(0.9999999999999996), "1");
  assert_expression_serialize_to(Decimal::Builder(0.99999999999995), "9.9999999999995ᴇ-1", ScientificMode, 14);
  assert_expression_serialize_to(Decimal::Builder(0.00000099999999999995), "9.9999999999995ᴇ-7", ScientificMode, 14);
  assert_expression_serialize_to(Decimal::Builder(0.000000999999999999995), "1ᴇ-6", DecimalMode);
  assert_expression_serialize_to(Decimal::Builder(0.000000999999999901200121020102010201201201021099995), "9.999999999012ᴇ-7", DecimalMode, 14);
  assert_expression_serialize_to(Decimal::Builder(9999999999999.53), "9999999999999.5", DecimalMode, 14);
  assert_expression_serialize_to(Decimal::Builder(99999999999999.54), "1ᴇ14", DecimalMode, 14);
  assert_expression_serialize_to(Decimal::Builder(999999999999999.54), "1ᴇ15", DecimalMode, 14);
  assert_expression_serialize_to(Decimal::Builder(9999999999999999.54), "1ᴇ16", DecimalMode, 14);
  assert_expression_serialize_to(Decimal::Builder(-9.702365051313E-297), "-9.702365051313ᴇ-297", DecimalMode, 14);

  // Engineering notation
  assert_expression_serialize_to(Decimal::Builder(0.0), "0", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(10.0), "10", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(100.0), "100", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(1000.0), "1ᴇ3", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(1234.0), "1.234ᴇ3", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(-0.1), "-100ᴇ-3", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(-0.01), "-10ᴇ-3", EngineeringMode, 7);
  assert_expression_serialize_to(Decimal::Builder(-0.001), "-1ᴇ-3", EngineeringMode, 7);
}

QUIZ_CASE(poincare_serialization_float) {
  assert_expression_serialize_to(Float<double>::Builder(-1.23456789E30), "-1.23456789ᴇ30", DecimalMode, 14);
  assert_expression_serialize_to(Float<double>::Builder(1.23456789E30), "1.23456789ᴇ30", DecimalMode, 14);
  assert_expression_serialize_to(Float<double>::Builder(-1.23456789E-30), "-1.23456789ᴇ-30", DecimalMode, 14);
  assert_expression_serialize_to(Float<double>::Builder(-1.2345E-3), "-0.0012345", DecimalMode);
  assert_expression_serialize_to(Float<double>::Builder(1.2345E-3), "0.0012345", DecimalMode);
  assert_expression_serialize_to(Float<double>::Builder(1.2345E3), "1234.5", DecimalMode);
  assert_expression_serialize_to(Float<double>::Builder(-1.2345E3), "-1234.5", DecimalMode);
  assert_expression_serialize_to(Float<double>::Builder(0.99999999999995), "9.9999999999995ᴇ-1", ScientificMode, 14);
  assert_expression_serialize_to(Float<double>::Builder(0.00000000099999999999995), "9.9999999999995ᴇ-10", DecimalMode, 14);
  assert_expression_serialize_to(Float<double>::Builder(0.0000000009999999999901200121020102010201201201021099995), "9.9999999999012ᴇ-10", DecimalMode, 14);
  assert_expression_serialize_to(Float<float>::Builder(1.2345E-1), "0.12345", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(1), "1", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(0.9999999999999995), "1", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(1.2345E6), "1234500", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(-1.2345E6), "-1234500", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(0.0000009999999999999995), "1ᴇ-6", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(-1.2345E-1), "-0.12345", DecimalMode);

  assert_expression_serialize_to(Float<double>::Builder(INFINITY), Infinity::Name(), DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(0.0f), "0", DecimalMode);
  assert_expression_serialize_to(Float<float>::Builder(NAN), Undefined::Name(), DecimalMode);
}

QUIZ_CASE(poincare_serialization_division) {
  assert_expression_serialize_to(Division::Builder(Rational::Builder(-2), Constant::Builder("π")), "\u0012-2\u0013/π");
  assert_expression_serialize_to(Division::Builder(Constant::Builder("π"), Rational::Builder(-2)), "π/\u0012-2\u0013");
  assert_expression_serialize_to(Division::Builder(Rational::Builder(2,3), Constant::Builder("π")), "\u00122/3\u0013/π");
  assert_expression_serialize_to(Division::Builder(Addition::Builder(Rational::Builder(2),Rational::Builder(1)), Constant::Builder("π")), "\u00122+1\u0013/π");
  assert_expression_serialize_to(Division::Builder(Subtraction::Builder(Rational::Builder(2),Rational::Builder(1)), Constant::Builder("π")), "\u00122-1\u0013/π");
  assert_expression_serialize_to(Division::Builder(Multiplication::Builder(Rational::Builder(2),Rational::Builder(1)), Constant::Builder("π")), "\u00122×1\u0013/π");
  assert_expression_serialize_to(Division::Builder(Division::Builder(Rational::Builder(2),Rational::Builder(1)), Constant::Builder("π")), "\u00122/1\u0013/π");
  assert_expression_serialize_to(Division::Builder(Opposite::Builder(Rational::Builder(2)), Constant::Builder("π")), "\u0012-2\u0013/π");
}

QUIZ_CASE(poincare_serialization_factorial) {
  assert_expression_serialize_to(Factorial::Builder(Rational::Builder(2,3)), "\u00122/3\u0013!");
  assert_expression_serialize_to(Factorial::Builder(Division::Builder(Constant::Builder("π"),Rational::Builder(3))), "\u0012π/3\u0013!");
  assert_expression_serialize_to(Factorial::Builder(Power::Builder(Constant::Builder("π"),Rational::Builder(3))), "\u0012π^3\u0013!");
}

QUIZ_CASE(poincare_serialization_percent) {
  assert_expression_serialize_to(PercentSimple::Builder(Rational::Builder(2,3)), "\u00122/3\u0013%");
  assert_expression_serialize_to(PercentSimple::Builder(Division::Builder(Constant::Builder("π"),Rational::Builder(3))), "\u0012π/3\u0013%");
  assert_expression_serialize_to(PercentSimple::Builder(Power::Builder(Constant::Builder("π"),Rational::Builder(3))), "\u0012π^3\u0013%");
  assert_expression_serialize_to(PercentAddition::Builder(PercentAddition::Builder(Rational::Builder(100), Rational::Builder(20)), Opposite::Builder(Rational::Builder(30))), "100↗20%↘30%");
}
