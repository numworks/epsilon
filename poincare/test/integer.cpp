#include "helper.h"
#include <poincare/expression.h>
#include <poincare/integer.h>
#include <poincare/infinity.h>

using namespace Poincare;

static inline Integer MaxInteger() { return Integer(MaxIntegerString()); }
static inline Integer OverflowedInteger() { return Integer(OverflowedIntegerString()); }

QUIZ_CASE(poincare_integer_constructor) {
  Integer zero;
  Integer a("123");
  Integer na("-123");
  Integer b("12345678910111213141516");
  Integer nb("-12345678910111213141516");
  Integer c(12314);
  Integer nc(-12314);
  Integer d((int64_t)1234567891011121314);
  Integer nd((int64_t)(-1234567891011121314));
  Integer e = Integer::Overflow(false);
  Integer f("101011", 6, false, OMG::Base::Binary);
  Integer g("A2B3", 4, false, OMG::Base::Hexadecimal);
  Integer h("123", 3, false, OMG::Base::Decimal);
}

static inline void assert_equal(const Integer i, const Integer j) {
  quiz_assert(Integer::NaturalOrder(i, j) == 0);
}
static inline void assert_not_equal(const Integer i, const Integer j) {
  quiz_assert(Integer::NaturalOrder(i, j) != 0);
}

static inline void assert_lower(const Integer i, const Integer j) {
  quiz_assert(Integer::NaturalOrder(i, j) < 0);
}

static inline void assert_greater(const Integer i, const Integer j) {
  quiz_assert(Integer::NaturalOrder(i, j) > 0);
}

QUIZ_CASE(poincare_integer_compare) {
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
  //FIXME: quiz_assert(Integer("0x2BABE") == Integer(178878));
  //FIXME: quiz_assert(Integer("0b1011") == Integer(11));
}

QUIZ_CASE(poincare_integer_properties) {
  quiz_assert(Integer(0).isZero());
  quiz_assert(!Integer(-1).isZero());
  quiz_assert(!Integer(1).isZero());
  quiz_assert(Integer(1).isOne());
  quiz_assert(!Integer(-1).isOne());
  quiz_assert(!Integer(0).isOne());
  quiz_assert(OverflowedInteger().isOverflow()); // 2^32^k_maxNumberOfDigits
  quiz_assert(!MaxInteger().isOverflow()); // 2^32^k_maxNumberOfDigits-1
  quiz_assert(!Integer(0).isOverflow());
  quiz_assert(Integer(8).isEven());
  quiz_assert(!Integer(7).isEven());
  quiz_assert(Integer(-8).isEven());
  quiz_assert(!Integer(-7).isEven());
  quiz_assert(!Integer(2).isNegative());
  quiz_assert(Integer(-2).isNegative());
  quiz_assert(Integer::NumberOfBase10DigitsWithoutSign(MaxInteger()) == 309);
}

static inline void assert_add_to(const Integer i, const Integer j, const Integer k) {
  quiz_assert(Integer::NaturalOrder(Integer::Addition(i, j), k) == 0);
}

QUIZ_CASE(poincare_integer_addition) {
  assert_add_to(Integer("0"), Integer("0"), Integer(0));
  assert_add_to(Integer(123), Integer(456), Integer(579));
  assert_add_to(Integer(123), Integer(456), Integer(579));
  assert_add_to(Integer("123456789123456789"), Integer(1), Integer("123456789123456790"));
  assert_add_to(Integer("-123456789123456789"), Integer("123456789123456789"), Integer("0"));
  assert_add_to(Integer("234"), Integer(-234), Integer(0));
  assert_add_to(Integer("18446744073709551616"), Integer("18446744073709551368"), Integer("36893488147419102984"));
  //2^64+2^64
  assert_add_to(Integer("18446744073709551616"), Integer("18446744073709551616"), Integer("36893488147419103232"));
  //2^64+2^32
  assert_add_to(Integer("18446744073709551616"), Integer("4294967296"), Integer("18446744078004518912"));
  //2^64+1
  assert_add_to(Integer("18446744073709551616"), Integer("1"), Integer("18446744073709551617"));
  //2^32+2^32
  assert_add_to(Integer("4294967296"), Integer("4294967296"), Integer("8589934592"));
  //2^32+1
  assert_add_to(Integer("4294967296"), Integer("1"), Integer("4294967297"));
  //2^16+1
  assert_add_to(Integer("65537"), Integer("1"), Integer("65538"));
  //2^16+2^16
  assert_add_to(Integer("65537"), Integer("65537"), Integer("131074"));
}

static inline void assert_sub_to(const Integer i, const Integer j, const Integer k) {
  quiz_assert(Integer::NaturalOrder(Integer::Subtraction(i, j), k) == 0);
}

QUIZ_CASE(poincare_integer_subtraction) {
  assert_sub_to(Integer(123), Integer(23), Integer(100));
  assert_sub_to(Integer("123456789123456789"), Integer("9999999999"), Integer("123456779123456790"));
  assert_sub_to(Integer(23), Integer(100), Integer(-77));
  assert_sub_to(Integer(23), Integer(23), Integer(0));
  assert_sub_to(Integer(-23), Integer(-23), Integer(0));
  assert_sub_to(Integer("-123456789123456789"), Integer("-123456789123456789"), Integer(0));
  assert_sub_to(Integer("123456789123456789"), Integer("123456789123456789"), Integer(0));
  assert_sub_to(Integer("18446744073709551616"), Integer("18446744073709551368"), Integer(248));

  //2^64-2^64
  assert_sub_to(Integer("18446744073709551616"), Integer("18446744073709551616"), Integer("0"));
  //2^64-2^32
  assert_sub_to(Integer("18446744073709551616"), Integer("4294967296"), Integer("18446744069414584320"));
  //2^32-2^64
  assert_sub_to(Integer("4294967296"), Integer("18446744073709551616"), Integer("-18446744069414584320"));
  //2^64-1
  assert_sub_to(Integer("18446744073709551616"), Integer("1"), Integer("18446744073709551615"));
  //1-2^64
  assert_sub_to(Integer("1"), Integer("18446744073709551616"), Integer("-18446744073709551615"));
  //2^32-2^32
  assert_sub_to(Integer("4294967296"), Integer("4294967296"), Integer("0"));
  //2^32-1
  assert_sub_to(Integer("4294967296"), Integer("1"), Integer("4294967295"));
  //2^16-1
  assert_sub_to(Integer("65537"), Integer("1"), Integer("65536"));
  //2^16-2^16
  assert_sub_to(Integer("65537"), Integer("65537"), Integer("0"));
}

static inline void assert_mult_to(const Integer i, const Integer j, const Integer k) {
  quiz_assert(Integer::NaturalOrder(Integer::Multiplication(i, j), k) == 0);
}

QUIZ_CASE(poincare_integer_multiplication) {
  assert_mult_to(Integer(12), Integer(34), Integer(408));
  assert_mult_to(Integer(56), Integer(0), Integer(0));
  assert_mult_to(Integer(-12), Integer(34), Integer(-408));
  assert_mult_to(Integer(-12), Integer(-34), Integer(408));
  assert_mult_to(Integer(123456), Integer(654321), Integer("80779853376"));
  assert_mult_to(Integer("9999999999"), Integer("9999999999"), Integer("99999999980000000001"));
  assert_mult_to(Integer("-23"), Integer("0"), Integer("0"));
  assert_mult_to(Integer("-23456787654567765456"), Integer("0"), Integer("0"));
  assert_mult_to(Integer("3293920983030066"), Integer(720), Integer("2371623107781647520"));
  assert_mult_to(Integer("389282362616"), Integer(720), Integer("280283301083520"));
}

static inline void assert_div_to(const Integer i, const Integer j, const Integer q, const Integer r) {
  quiz_assert(Integer::NaturalOrder(Integer::Division(i, j).quotient, q) == 0);
  quiz_assert(Integer::NaturalOrder(Integer::Division(i, j).remainder, r) == 0);
}

QUIZ_CASE(poincare_integer_divide) {
  assert_div_to(Integer("146097313984800"), Integer(720), Integer("202912936090"), Integer(0));
  assert_div_to(Integer(8), Integer(4), Integer(2), Integer(0));
  assert_div_to(Integer("3293920983030066"), Integer(38928), Integer("84615726033"), Integer(17442));
  assert_div_to(Integer("3293920983030066"), Integer("389282362616"), Integer(8461), Integer("202912936090"));
  assert_div_to(Integer("-18940566"), Integer("499030"), Integer(-38), Integer("22574"));
  assert_div_to(Integer("234567909876"), Integer("-234567898"), Integer(-1000), Integer("11876"));
  assert_div_to(Integer("-567"), Integer("-12"), Integer(48), Integer("9"));
  assert_div_to(Integer("-576"), Integer("-12"), Integer(48), Integer("0"));
  assert_div_to(Integer("576"), Integer("-12"), Integer(-48), Integer("0"));
  assert_div_to(Integer("-576"), Integer("12"), Integer(-48), Integer("0"));
  assert_div_to(Integer("12345678910111213141516171819202122232425"), Integer("10"), Integer("1234567891011121314151617181920212223242"), Integer("5"));
  assert_div_to(Integer("1234567891011121314151617181920212223242"), Integer("10"), Integer("123456789101112131415161718192021222324"), Integer("2"));
  assert_div_to(Integer("123456789101112131415161718192021222324"), Integer("10"), Integer("12345678910111213141516171819202122232"), Integer("4"));
  assert_div_to(Integer("12345678910111213141516171819202122232"), Integer("10"), Integer("1234567891011121314151617181920212223"), Integer("2"));
  assert_div_to(Integer("1234567891011121314151617181920212223"), Integer("10"), Integer("123456789101112131415161718192021222"), Integer("3"));
  assert_div_to(Integer("123456789101112131415161718192021222"), Integer("10"), Integer("12345678910111213141516171819202122"), Integer("2"));
  assert_div_to(Integer("12345678910111213141516171819202122"), Integer("10"), Integer("1234567891011121314151617181920212"), Integer("2"));
  assert_div_to(Integer("0"), Integer("-10"), Integer("0"), Integer("0"));
  assert_div_to(Integer("0"), Integer("-123456789098760"), Integer("0"), Integer("0"));
  assert_div_to(Integer("2305843009213693952"), Integer("2305843009213693921"), Integer("1"), Integer("31"));
  assert_div_to(MaxInteger(), MaxInteger(), Integer(1), Integer(0));
  assert_div_to(Integer("18446744073709551615"), Integer(10), Integer("1844674407370955161"), Integer(5));
  assert_div_to(MaxInteger(), Integer(10), Integer("17976931348623159077293051907890247336179769789423065727343008115773267580550096313270847732240753602112011387987139335765878976881441662249284743063947412437776789342486548527630221960124609411945308295208500576883815068234246288147391311054082723716335051068458629823994724593847971630483535632962422413721"), Integer(5));
}

static inline void assert_pow_to(const Integer i, const Integer j, const Integer k) {
  quiz_assert(Integer::NaturalOrder(Integer::Power(i, j), k) == 0);
}

QUIZ_CASE(poincare_integer_pow) {
  assert_pow_to(Integer(0), Integer(14), Integer(0));
  assert_pow_to(Integer(14), Integer(0), Integer(1));
  assert_pow_to(Integer(2), Integer(2), Integer(4));
  assert_pow_to(Integer("12345678910111213141516171819202122232425"), Integer(2), Integer("152415787751564791571474464067365843004067618915106260955633159458990465721380625"));
  assert_pow_to(Integer(14), Integer(14), Integer("11112006825558016"));
}

static inline void assert_factorial_to(const Integer i, const Integer j) {
  quiz_assert(Integer::NaturalOrder(Integer::Factorial(i), j) == 0);
}

QUIZ_CASE(poincare_integer_factorial) {
  assert_factorial_to(Integer(5), Integer(120));
  assert_factorial_to(Integer(123), Integer("12146304367025329675766243241881295855454217088483382315328918161829235892362167668831156960612640202170735835221294047782591091570411651472186029519906261646730733907419814952960000000000000000000000000000"));
}

// Simplify

template<typename T>
void assert_integer_evals_to(const char * i, T result, OMG::Base base = OMG::Base::Decimal) {
  quiz_assert(Integer(i, strlen(i), false, base).approximate<T>() == result);
}

QUIZ_CASE(poincare_integer_evaluate) {
  assert_integer_evals_to("1", 1.0f);
  assert_integer_evals_to("1", 1.0);
  assert_integer_evals_to("12345678", 12345678.0f);
  assert_integer_evals_to("12345678", 12345678.0);
  assert_integer_evals_to("0", 0.0f);
  assert_integer_evals_to("0", 0.0);
  assert_integer_evals_to("-0", 0.0f);
  assert_integer_evals_to("-0", 0.0);
  assert_integer_evals_to("-1", -1.0f);
  assert_integer_evals_to("-1", -1.0);
  assert_integer_evals_to("12345678", 12345678.0f);
  assert_integer_evals_to("12345678", 12345678.0);
  assert_integer_evals_to("4294967295", 4294967295.0f);
  assert_integer_evals_to("4294967295", 4294967295.0);
  assert_integer_evals_to("-4294967296", -4294967296.0f);
  assert_integer_evals_to("-4294967296", -4294967296.0);
  assert_integer_evals_to("2147483648", 2147483648.0f);
  assert_integer_evals_to("2147483648", 2147483648.0);
  assert_integer_evals_to("-18446744073709551615", -18446744073709551615.0f);
  assert_integer_evals_to("-18446744073709551615", -18446744073709551615.0);
  assert_integer_evals_to("-18446744073709551616", -18446744073709551616.0f);
  assert_integer_evals_to("-18446744073709551616", -18446744073709551616.0);
  assert_integer_evals_to("92233720372036854775808", 92233720372036854775808.0f);
  assert_integer_evals_to("92233720372036854775808", 92233720372036854775808.0);
  assert_integer_evals_to("134217720", 134217720.0f);
  assert_integer_evals_to("134217720", 134217720.0);
  assert_integer_evals_to("67108864", 67108864.0f);
  assert_integer_evals_to("67108864", 67108864.0);
  assert_integer_evals_to("33554432", 33554432.0f);
  assert_integer_evals_to("33554432", 33554432.0);
  assert_integer_evals_to("2", 2.0f);
  assert_integer_evals_to("2", 2.0);
  assert_integer_evals_to("4", 4.0f);
  assert_integer_evals_to("4", 4.0);
  assert_integer_evals_to("179769313486230000002930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215", 1.7976931348622999E+308);
#pragma GCC diagnostic push
  /* Options are compiler-dependent. We silent warnings about unknown options
   * in pragmas. */
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-range"
  /* Silent known warnings: we're aware that the magnitude of the floating-point
   * constant is too large for 'double' type but we're actually checking that
   * both are casted down to the same float. */
  assert_integer_evals_to(OverflowedIntegerString(), 179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216.0);
  assert_integer_evals_to(MaxIntegerString(), 179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215.0);
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

  // Based Integer
  assert_integer_evals_to("1011", 11.0f, OMG::Base::Binary);
  assert_integer_evals_to("2A", 42.0f, OMG::Base::Hexadecimal);
  assert_integer_evals_to("42", 42.0f, OMG::Base::Decimal);
}

//Serialize

static inline void assert_integer_serializes_to(const Integer i, const char * serialization, OMG::Base base = OMG::Base::Decimal) {
  char buffer[500];
  i.serialize(buffer, 500, base);
  quiz_assert(strcmp(buffer, serialization) == 0);
}

QUIZ_CASE(poincare_integer_serialize) {
  assert_integer_serializes_to(Integer(-2), "-2");
  assert_integer_serializes_to(Integer("2345678909876"), "2345678909876");
  assert_integer_serializes_to(Integer("-2345678909876"), "-2345678909876");
  assert_integer_serializes_to(Integer(9), "0b1001", OMG::Base::Binary);
  assert_integer_serializes_to(Integer((double_native_int_t)4000000000000), "0b111010001101010010100101000100000000000000", OMG::Base::Binary);
  assert_integer_serializes_to(Integer(9131), "0x23AB", OMG::Base::Hexadecimal);
  assert_integer_serializes_to(Integer(123), "123", OMG::Base::Decimal);
  assert_integer_serializes_to(Integer("-2345678909876"), "-2345678909876");
  assert_integer_serializes_to(MaxInteger(), MaxIntegerString());
  assert_integer_serializes_to(OverflowedInteger(), Infinity::Name());
}

// Euclidian Division

void assert_division_computes_to(int n, int m, const char * div) {
  assert_expression_serialize_to(Integer::CreateEuclideanDivision(Integer(n), Integer(m)), div);
}

QUIZ_CASE(poincare_integer_euclidian_division) {
  assert_division_computes_to(47, 8, "47=8×5+7");
  assert_division_computes_to(1, 5, "1=5×0+1");
  assert_division_computes_to(12, 4, "12=4×3+0");
  assert_division_computes_to(-33, 7, "-33=7×(-5)+2");
  assert_division_computes_to(-28, 101, "-28=101×(-1)+73");
  assert_division_computes_to(-40, 2, "-40=2×(-20)+0");
}

void assert_mixed_fraction_computes_to(int n, int m, const char * frac, bool mixedFractionsEnabled = true) {
  Preferences::sharedPreferences()->enableMixedFractions(mixedFractionsEnabled ? Preferences::MixedFractions::Enabled : Preferences::MixedFractions::Disabled);
  assert_expression_serialize_to(Integer::CreateMixedFraction(Integer(n), Integer(m)), frac);
}

QUIZ_CASE(poincare_integer_mixed_fraction) {
  assert_mixed_fraction_computes_to(47, 8, "5 7/8");
  assert_mixed_fraction_computes_to(1, 5, "0 1/5");
  assert_mixed_fraction_computes_to(-33, 7, "-4 5/7");
  assert_mixed_fraction_computes_to(-28, 101, "-0 28/101");
  assert_mixed_fraction_computes_to(16, 12, "1 1/3");

  assert_mixed_fraction_computes_to(47, 8, "5+7/8", false);
  assert_mixed_fraction_computes_to(1, 5, "0+1/5", false);
  assert_mixed_fraction_computes_to(-33, 7, "-4-5/7", false);
  assert_mixed_fraction_computes_to(-28, 101, "0-28/101", false);
  assert_mixed_fraction_computes_to(16, 12, "1+1/3", false);
}
