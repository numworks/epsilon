#include "print.h"
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/memory/tree_constructor.h>
#include <quiz.h>

using namespace PoincareJ;

static inline EditionReference CreateInteger(const char * digits) {
  size_t length = strlen(digits);
  OMG::Base base = OMG::Base::Decimal;
  size_t prefixLength = strlen("0b");
  assert(prefixLength == strlen("0x"));
  if (length > prefixLength && digits[0] == '0' && (digits[1] == 'b' || digits[1] == 'x')) {
    base = digits[1] == 'b' ? OMG::Base::Binary : OMG::Base::Hexadecimal;
    digits = digits + prefixLength;
    length -= prefixLength;
  }
  return Integer::Push(digits, length, base);
}

static inline IntegerHandler CreateIntegerHandler(const char * digits) {
  return Integer::Handler(CreateInteger(digits));
}

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
  IntegerHandler a = CreateIntegerHandler("254");
  IntegerHandler b = CreateIntegerHandler("-13");

  assert(a.strictSign() == StrictSign::Positive);
  assert(b.strictSign() == StrictSign::Negative);
  assert(zero.strictSign() == StrictSign::Null);
  assert(!a.isZero() && zero.isZero());
  assert(!a.isOne() && one.isOne());
  assert(!a.isMinusOne() && minusOne.isMinusOne());
  assert(!a.isTwo() && two.isTwo());
  assert(a.isEven() && !b.isEven());
  assert(!a.isSignedType<int8_t>() && b.isSignedType<int8_t>() && static_cast<int8_t>(b) == -13);
  assert(!b.isUnsignedType<uint8_t>() && a.isUnsignedType<uint8_t>() && static_cast<uint8_t>(a) == 254);
}

static inline void assert_equal(const char * a, const char * b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a), CreateIntegerHandler(b)) == 0);
}
static inline void assert_not_equal(const char * a, const char * b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a), CreateIntegerHandler(b)) != 0);
}

static inline void assert_lower(const char * a, const char * b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a), CreateIntegerHandler(b)) < 0);
}

static inline void assert_greater(const char * a, const char * b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a), CreateIntegerHandler(b)) > 0);
}

QUIZ_CASE(pcj_integer_compare) {
  assert_equal("123", "123");
  assert_equal("-123", "-123");
  assert_not_equal("-123", "123");
  assert_equal("1234567891011121314", "1234567891011121314");
  assert_equal("1234567891011121314", "1234567891011121314");
  assert_not_equal("-1234567891011121314", "1234567891011121314");
  assert_lower("123", "456");
  assert_greater("456", "123");
  assert_lower("-100", "2");
  assert_lower("-200", "-100");
  assert_lower("123", "123456789123456789");
  assert_lower("-123", "123456789123456789");
  assert_lower("123456789123456788", "123456789123456789");
  assert_lower("-1234567891234567892109209109", "123456789123456789");
  assert_greater("123456789123456789", "123456789123456788");
  assert_equal("0x2BABE", "178878");
  assert_equal("0b1011", "11");
}

static inline void assert_add_to(const char * a, const char * b, const char * c) {
  // TODO: remove static_cast<Node> when Hugo's PR is merged
  quiz_assert(static_cast<Node>(IntegerHandler::Addition(CreateIntegerHandler(a), CreateIntegerHandler(b))).treeIsIdenticalTo(CreateInteger(c)));
}

QUIZ_CASE(pcj_integer_addition) {
  assert_add_to("0", "0", "0");
  assert_add_to("123", "456", "579");
  assert_add_to("123", "456", "579");
  assert_add_to("123456789123456789", "1", "123456789123456790");
  assert_add_to("-123456789123456789", "123456789123456789", "0");
  assert_add_to("234", "-234", "0");
  assert_add_to("18446744073709551616", "18446744073709551368", "36893488147419102984");
  //2^64+2^64
  assert_add_to("18446744073709551616", "18446744073709551616", "36893488147419103232");
  //2^64+2^32
  assert_add_to("18446744073709551616", "4294967296", "18446744078004518912");
  //2^64+1
  assert_add_to("18446744073709551616", "1", "18446744073709551617");
  //2^32+2^32
  assert_add_to("4294967296", "4294967296", "8589934592");
  //2^32+1
  assert_add_to("4294967296", "1", "4294967297");
  //2^16+1
  assert_add_to("65537", "1", "65538");
  //2^16+2^16
  assert_add_to("65537", "65537", "131074");
}
