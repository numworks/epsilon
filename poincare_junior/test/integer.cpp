#include "print.h"
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_integer_constructor) {
  IntegerHandler zero(static_cast<uint8_t>(0));
  IntegerHandler one(1);
  IntegerHandler two(2);
  IntegerHandler minusOne(-1);
  IntegerHandler a(12);
  IntegerHandler b(-23);
  EditionReference::Push<BlockType::IntegerPosBig>(static_cast<uint64_t>(1232424242));
  EditionReference::Push<BlockType::IntegerNegBig>(static_cast<uint64_t>(23424));
  Integer::Push("123", sizeof("123") - 1);
  Integer::Push("-123", sizeof("-123") - 1);
  Integer::Push("12345678910111213141516", sizeof("12345678910111213141516") - 1);
  Integer::Push("-12345678910111213141516", sizeof("-12345678910111213141516") - 1);
  Integer::Push("101011", sizeof("101011") - 1, OMG::Base::Binary);
  Integer::Push("A2B3", sizeof("A2B3") - 1, OMG::Base::Hexadecimal);
  Integer::Push("123", sizeof("123") - 1, OMG::Base::Decimal);
}

QUIZ_CASE(pcj_integer_properties) {
  IntegerHandler zero(static_cast<uint8_t>(0));
  IntegerHandler one(1);
  IntegerHandler two(2);
  IntegerHandler minusOne(-1);
  IntegerHandler a = Integer::Handler(Integer::Push("254", sizeof("254") - 1));
  IntegerHandler b = Integer::Handler(Integer::Push("-12", sizeof("-12") - 1));
  assert(a.strictSign() == StrictSign::Positive);
  assert(b.strictSign() == StrictSign::Negative);
  assert(zero.strictSign() == StrictSign::Null);
  assert(!a.isZero() && zero.isZero());
  assert(!a.isOne() && one.isOne());
  assert(!a.isMinusOne() && minusOne.isMinusOne());
  assert(!a.isTwo() && two.isTwo());
  assert(!a.isSignedType<int8_t>() && b.isSignedType<int8_t>() && static_cast<int8_t>(b) == -12);
  assert(!b.isUnsignedType<uint8_t>() && a.isUnsignedType<uint8_t>() && static_cast<uint8_t>(a) == 254);
}

QUIZ_CASE(pcj_integer_compare) {}
QUIZ_CASE(pcj_integer_addition) {}
#if 0
static inline void assert_equal(const IntegerHandler i, const IntegerHandler j) {
  quiz_assert(Integer::Compare(i, j) == 0);
}
static inline void assert_not_equal(const IntegerHandler i, const IntegerHandler j) {
  quiz_assert(Integer::Compare(i, j) != 0);
}

static inline void assert_lower(const Integer i, const Integer j) {
  quiz_assert(Integer::Compare(i, j) < 0);
}

static inline void assert_greater(const IntegerHandler i, const IntegerHandler j) {
  quiz_assert(IntegerHandler::Compare(i, j) > 0);
}

void testIntegerCompare() {
  assert_equal(Rational::Denominator(EditionReference::Push<RationalShort>(1, 2), two);
  assert_equal(Integer(123), Integer(123));
  assert_equal(Integer(-123), Integer(-123));
  assert_equal(Integer("123"), Integer(123));
  assert_not_equal(Integer("-123"), Integer(123));
  assert_equal(Integer("-123"), Integer(-123));
  assert_equal(Integer((int64_t)1234567891011121314), Integer((int64_t)1234567891011121314));
  assert_equal(Integer("1234567891011121314"), Integer((int64_t)1234567891011121314));
  assert_not_equal(Integer("-1234567891011121314"), Integer((int64_t)1234567891011121314));
  assert_lower(Integer(123), Integer(456));
  assert_greater(Integer(456), Integer(123));
  assert_lower(Integer(-100), Integer(2));
  assert_lower(Integer(-200), Integer(-100));
  assert_lower(Integer(123), Integer("123456789123456789"));
  assert_lower(Integer(-123), Integer("123456789123456789"));
  assert_lower(Integer("123456789123456788"), Integer("123456789123456789"));
  assert_lower(Integer("-1234567891234567892109209109"), Integer("123456789123456789"));
  assert_greater(Integer("123456789123456789"), Integer("123456789123456788"));
  assert_greater(Integer::Overflow(false), Integer("123456789123456788"));

}

void testIntegerAddition() {
}
#endif
