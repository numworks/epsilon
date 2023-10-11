#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

static Tree* CreateInteger(const char* digits) {
  size_t length = strlen(digits);
  OMG::Base base = OMG::Base::Decimal;
  size_t prefixLength = strlen("0b");
  assert(prefixLength == strlen("0x"));
  if (length > prefixLength && digits[0] == '0' &&
      (digits[1] == 'b' || digits[1] == 'x')) {
    base = digits[1] == 'b' ? OMG::Base::Binary : OMG::Base::Hexadecimal;
    digits = digits + prefixLength;
    length -= prefixLength;
  }
  return Integer::Push(digits, length, base);
}

static IntegerHandler CreateIntegerHandler(const char* digits) {
  return Integer::Handler(CreateInteger(digits));
}

QUIZ_CASE(pcj_integer_constructor) {
  IntegerHandler zero(static_cast<uint8_t>(0));
  IntegerHandler one(1);
  IntegerHandler two(2);
  IntegerHandler minusOne(-1);
  IntegerHandler a(12);
  IntegerHandler b(-23);
  SharedEditionPool->push<BlockType::IntegerPosBig>(
      static_cast<uint64_t>(1232424242));
  SharedEditionPool->push<BlockType::IntegerNegBig>(
      static_cast<uint64_t>(23424));
  Integer::Push("123", sizeof("123") - 1);
  Integer::Push("-123", sizeof("-123") - 1);
  Integer::Push("12345678910111213141516",
                sizeof("12345678910111213141516") - 1);
  Integer::Push("-12345678910111213141516",
                sizeof("-12345678910111213141516") - 1);
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
  assert(!a.isSignedType<int8_t>() && b.isSignedType<int8_t>() &&
         static_cast<int8_t>(b) == -13);
  assert(!b.isUnsignedType<uint8_t>() && a.isUnsignedType<uint8_t>() &&
         static_cast<uint8_t>(a) == 254);
}

static void assert_equal(const char* a, const char* b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a),
                                      CreateIntegerHandler(b)) == 0);
}
static void assert_not_equal(const char* a, const char* b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a),
                                      CreateIntegerHandler(b)) != 0);
}

static void assert_lower(const char* a, const char* b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a),
                                      CreateIntegerHandler(b)) < 0);
}

static void assert_greater(const char* a, const char* b) {
  quiz_assert(IntegerHandler::Compare(CreateIntegerHandler(a),
                                      CreateIntegerHandler(b)) > 0);
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

static void assert_set_sign_to(const Tree* i, NonStrictSign sign,
                               const Tree* r) {
  Tree* t = i->clone();
  Integer::SetSign(t, sign);
  quiz_assert(t->treeIsIdenticalTo(r));
  t->removeTree();
}

QUIZ_CASE(pcj_integer_set_sign) {
  assert_set_sign_to(0_e, NonStrictSign::Negative, 0_e);
  assert_set_sign_to(0_e, NonStrictSign::Positive, 0_e);
  assert_set_sign_to(2_e, NonStrictSign::Negative, -2_e);
  assert_set_sign_to(-3_e, NonStrictSign::Positive, 3_e);
}

static void assert_add_to(const char* a, const char* b, const char* c) {
  quiz_assert(
      IntegerHandler::Addition(CreateIntegerHandler(a), CreateIntegerHandler(b))
          ->treeIsIdenticalTo(CreateInteger(c)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_addition) {
  assert_add_to("0", "0", "0");
  assert_add_to("123", "456", "579");
  assert_add_to("123", "456", "579");
  assert_add_to("123456789123456789", "1", "123456789123456790");
  assert_add_to("-123456789123456789", "123456789123456789", "0");
  assert_add_to("234", "-234", "0");
  assert_add_to("18446744073709551616", "18446744073709551368",
                "36893488147419102984");
  // 2^64+2^64
  assert_add_to("18446744073709551616", "18446744073709551616",
                "36893488147419103232");
  // 2^64+2^32
  assert_add_to("18446744073709551616", "4294967296", "18446744078004518912");
  // 2^64+1
  assert_add_to("18446744073709551616", "1", "18446744073709551617");
  // 2^32+2^32
  assert_add_to("4294967296", "4294967296", "8589934592");
  // 2^32+1
  assert_add_to("4294967296", "1", "4294967297");
  // 2^16+1
  assert_add_to("65537", "1", "65538");
  // 2^16+2^16
  assert_add_to("65537", "65537", "131074");
}

static void assert_sub_to(const char* a, const char* b, const char* c) {
  quiz_assert(IntegerHandler::Subtraction(CreateIntegerHandler(a),
                                          CreateIntegerHandler(b))
                  ->treeIsIdenticalTo(CreateInteger(c)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_subtraction) {
  assert_sub_to("123", "23", "100");
  assert_sub_to("123456789123456789", "9999999999", "123456779123456790");
  assert_sub_to("23", "100", "-77");
  assert_sub_to("23", "23", "0");
  assert_sub_to("-23", "-23", "0");
  assert_sub_to("-123456789123456789", "-123456789123456789", "0");
  assert_sub_to("123456789123456789", "123456789123456789", "0");
  assert_sub_to("18446744073709551616", "18446744073709551368", "248");

  // 2^64-2^64
  assert_sub_to("18446744073709551616", "18446744073709551616", "0");
  // 2^64-2^32
  assert_sub_to("18446744073709551616", "4294967296", "18446744069414584320");
  // 2^32-2^64
  assert_sub_to("4294967296", "18446744073709551616", "-18446744069414584320");
  // 2^64-1
  assert_sub_to("18446744073709551616", "1", "18446744073709551615");
  // 1-2^64
  assert_sub_to("1", "18446744073709551616", "-18446744073709551615");
  // 2^32-2^32
  assert_sub_to("4294967296", "4294967296", "0");
  // 2^32-1
  assert_sub_to("4294967296", "1", "4294967295");
  // 2^16-1
  assert_sub_to("65537", "1", "65536");
  // 2^16-2^16
  assert_sub_to("65537", "65537", "0");
}

static void assert_mult_to(const char* a, const char* b, const char* c) {
  quiz_assert(IntegerHandler::Multiplication(CreateIntegerHandler(a),
                                             CreateIntegerHandler(b))
                  ->treeIsIdenticalTo(CreateInteger(c)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_multiplication) {
  assert_mult_to("12", "34", "408");
  assert_mult_to("56", "0", "0");
  assert_mult_to("-12", "34", "-408");
  assert_mult_to("-12", "-34", "408");
  assert_mult_to("123456", "654321", "80779853376");
  assert_mult_to("9999999999", "9999999999", "99999999980000000001");
  assert_mult_to("-23", "0", "0");
  assert_mult_to("-23456787654567765456", "0", "0");
  assert_mult_to("3293920983030066", "720", "2371623107781647520");
  assert_mult_to("389282362616", "720", "280283301083520");
  assert_mult_to("16013", "773094113280", "12379556035952640");
}

static void assert_div_to(const char* a, const char* b, const char* q,
                          const char* r) {
  auto [quotient, remainder] = IntegerHandler::Division(
      CreateIntegerHandler(a), CreateIntegerHandler(b));
  quiz_assert(quotient->treeIsIdenticalTo(CreateInteger(q)) &&
              remainder->treeIsIdenticalTo(CreateInteger(r)));
  reset_pools();
  quiz_assert(
      IntegerHandler::Quotient(CreateIntegerHandler(a), CreateIntegerHandler(b))
          ->treeIsIdenticalTo(CreateInteger(q)));
  reset_pools();
  quiz_assert(IntegerHandler::Remainder(CreateIntegerHandler(a),
                                        CreateIntegerHandler(b))
                  ->treeIsIdenticalTo(CreateInteger(r)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_divide) {
  assert_div_to("8", "4", "2", "0");
  assert_div_to("146097313984800", "720", "202912936090", "0");
  assert_div_to("3293920983030066", "38928", "84615726033", "17442");
  assert_div_to("3293920983030066", "389282362616", "8461", "202912936090");
  assert_div_to("-18940566", "499030", "-38", "22574");
  assert_div_to("234567909876", "-234567898", "-1000", "11876");
  assert_div_to("-567", "-12", "48", "9");
  assert_div_to("-576", "-12", "48", "0");
  assert_div_to("576", "-12", "-48", "0");
  assert_div_to("-576", "12", "-48", "0");
  assert_div_to("12345678910111213141516171819202122232425", "10",
                "1234567891011121314151617181920212223242", "5");
  assert_div_to("1234567891011121314151617181920212223242", "10",
                "123456789101112131415161718192021222324", "2");
  assert_div_to("123456789101112131415161718192021222324", "10",
                "12345678910111213141516171819202122232", "4");
  assert_div_to("12345678910111213141516171819202122232", "10",
                "1234567891011121314151617181920212223", "2");
  assert_div_to("1234567891011121314151617181920212223", "10",
                "123456789101112131415161718192021222", "3");
  assert_div_to("123456789101112131415161718192021222", "10",
                "12345678910111213141516171819202122", "2");
  assert_div_to("12345678910111213141516171819202122", "10",
                "1234567891011121314151617181920212", "2");
  assert_div_to("0", "-10", "0", "0");
  assert_div_to("0", "-123456789098760", "0", "0");
  assert_div_to("2305843009213693952", "2305843009213693921", "1", "31");
  assert_div_to(MaxIntegerString(), MaxIntegerString(), "1", "0");
  assert_div_to("18446744073709551615", "10", "1844674407370955161", "5");
  assert_div_to(MaxIntegerString(), "10",
                "17976931348623159077293051907890247336179769789423065727343008"
                "11577326758055009631327084773224075360211201138798713933576587"
                "89768814416622492847430639474124377767893424865485276302219601"
                "24609411945308295208500576883815068234246288147391311054082723"
                "716335051068458629823994724593847971630483535632962422413721",
                "5");
}

static void assert_pow_to(const char* a, const char* b, const char* c) {
  quiz_assert(
      IntegerHandler::Power(CreateIntegerHandler(a), CreateIntegerHandler(b))
          ->treeIsIdenticalTo(CreateInteger(c)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_pow) {
  assert_pow_to("0", "14", "0");
  assert_pow_to("14", "0", "1");
  assert_pow_to("2", "2", "4");
  assert_pow_to("12345678910111213141516171819202122232425", "2",
                "15241578775156479157147446406736584300406761891510626095563315"
                "9458990465721380625");
  assert_pow_to("14", "14", "11112006825558016");
}

static void assert_factorial_to(const char* a, const char* b) {
  quiz_assert(IntegerHandler::Factorial(CreateIntegerHandler(a))
                  ->treeIsIdenticalTo(CreateInteger(b)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_factorial) {
  assert_factorial_to("5", "120");
  assert_factorial_to(
      "123",
      "121463043670253296757662432418812958554542170884833823153289181618292358"
      "923621676688311569606126402021707358352212940477825910915704116514721860"
      "29519906261646730733907419814952960000000000000000000000000000");
}

static void assert_gcd_to(const char* a, const char* b, const char* c) {
  quiz_assert(
      IntegerHandler::GCD(CreateIntegerHandler(a), CreateIntegerHandler(b))
          ->treeIsIdenticalTo(CreateInteger(c)));
  reset_pools();
}

QUIZ_CASE(pcj_integer_gcd) {
  assert_gcd_to("42", "18", "6");
  assert_gcd_to("18", "42", "6");
  assert_gcd_to("7", "13", "1");
  assert_gcd_to("7", "7", "7");
  assert_gcd_to("-8", "16", "8");
  assert_gcd_to("1", "-1", "1");
  assert_gcd_to("0", "-3", "3");
  assert_gcd_to("0", "0", "0");
}

static void assert_might_overflow(ActionWithContext action, bool overflow) {
  CachePool* cachePool = CachePool::sharedCachePool();
  ExceptionTry {
    cachePool->nodeForIdentifier(
        SharedEditionPool->executeAndCache(action, nullptr, nullptr));
    quiz_assert(!overflow);
  }
  ExceptionCatch(type) {
    if (type == ExceptionType::IntegerOverflow) {
      quiz_assert(overflow);
    } else {
      ExceptionCheckpoint::Raise(type);
    }
  }
}

static void assert_did_overflow(ActionWithContext action) {
  assert_might_overflow(action, true);
}

static void assert_did_not_overflow(ActionWithContext action) {
  assert_might_overflow(action, false);
}

QUIZ_CASE(pcj_integer_overflows) {
  // Construction
  assert_did_overflow([](void* context, const void* data) {
    CreateIntegerHandler(OverflowedIntegerString());
  });
  assert_did_not_overflow([](void* context, const void* data) {
    CreateIntegerHandler(MaxIntegerString());
  });

  // Operations
  assert_did_overflow([](void* context, const void* data) {
    EditionReference a = CreateInteger(MaxIntegerString());
    EditionReference b = CreateInteger("1");
    IntegerHandler::Addition(Integer::Handler(a), Integer::Handler(b));
    a->removeTree();
    b->removeTree();
  });
  assert_did_not_overflow([](void* context, const void* data) {
    EditionReference a = CreateInteger(MaxIntegerString());
    EditionReference b = CreateInteger("1");
    IntegerHandler::Subtraction(Integer::Handler(a), Integer::Handler(b));
    a->removeTree();
    b->removeTree();
  });
  assert_did_overflow([](void* context, const void* data) {
    EditionReference a = CreateInteger(MaxIntegerString());
    EditionReference b = CreateInteger("2");
    IntegerHandler::Multiplication(Integer::Handler(a), Integer::Handler(b));
    a->removeTree();
    b->removeTree();
  });
  assert_did_not_overflow([](void* context, const void* data) {
    EditionReference a = CreateInteger(MaxIntegerString());
    EditionReference b = CreateInteger("1");
    IntegerHandler::Multiplication(Integer::Handler(a), Integer::Handler(b));
    a->removeTree();
    b->removeTree();
  });
}
