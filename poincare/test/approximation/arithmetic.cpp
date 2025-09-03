#include <omg/float.h>
#include <poincare/test/float_helper.h>

#include "helper.h"

QUIZ_CASE(pcj_approximation_arithmetic) {
  approximates_to<float>("floor(π)", "3");
  approximates_to<float>("floor(-π)", "-4");
  approximates_to<float>("log(floor(2^64),2)", "64");
  // TODO_PCJ: this test returns nonreal with emscripten
  // approximates_to<float>("floor(1+i)", "undef");
  approximates_to<float>("ceil(3)", "3");
  approximates_to<float>("ceil(π)", "4");
  approximates_to<float>("ceil(-π)", "-3");
  approximates_to<float>("ceil(1+i)", "undef");
  approximates_to<float>("ceil(0.2)", "1");
  approximates_to<double>("ceil(0.2)", "1");
  approximates_to<float>("floor(2.3)", "2");
  approximates_to<double>("floor(2.3)", "2");
  approximates_to<float>("frac(2.3)", "0.3");
  approximates_to<double>("frac(2.3)", "0.3");
  approximates_to<float>("round(2.3246,3)", "2.325");
  approximates_to<double>("round(2.3245,3)", "2.325");
  approximates_to<float>("round(2.3245)", "2");
  approximates_to<double>("round(2.3245)", "2");

  approximates_to<float>("6!", "720");
  approximates_to<double>("6!", "720");

  approximates_to<float>("gcd(234,394)", "2");
  approximates_to<double>("gcd(234,394)", "2");
  approximates_to<float>("gcd(-234,394)", "2");
  approximates_to<double>("gcd(234,-394)", "2");
  approximates_to<float>("gcd(-234,-394)", "2");
  approximates_to<float>("gcd(-234,-394, -16)", "2");
  approximates_to<double>("gcd(-234,-394, -16)", "2");
  approximates_to<float>("gcd(6,15,10)", "1");
  approximates_to<double>("gcd(6,15,10)", "1");
  approximates_to<float>("gcd(30,105,70,42)", "1");
  approximates_to<double>("gcd(30,105,70,42)", "1");

  approximates_to<float>("lcm(234,394)", "46098");
  approximates_to<double>("lcm(234,394)", "46098");
  approximates_to<float>("lcm(-234,394)", "46098");
  approximates_to<double>("lcm(234,-394)", "46098");
  approximates_to<float>("lcm(-234,-394)", "46098");
  approximates_to<float>("lcm(-234,-394, -16)", "368784");
  approximates_to<double>("lcm(-234,-394, -16)", "368784");
  approximates_to<float>("lcm(6,15,10)", "30");
  approximates_to<double>("lcm(6,15,10)", "30");
  approximates_to<float>("lcm(30,105,70,42)", "210");
  approximates_to<double>("lcm(30,105,70,42)", "210");

  /* Testing LCM and GCD integer limits :
   * undef result is expected when manipulating overflowing/inaccurate integers
   * For context :
   * - INT_MAX =            2,147,483,647
   * - UINT32_MAX =         4,294,967,295
   * - Maximal representable integer without loss of precision in :
   *     - float :             16,777,216
   *     - double : 9,007,199,254,740,992
   */
  // Integers that can't be accurately represented as float
  // 1
  approximates_to<float>("gcd(16777219,13)", "undef");
  approximates_to<double>("gcd(16777219,13)", "1");
  // 16777219
  approximates_to<float>("lcm(1549, 10831)", "undef");
  approximates_to<double>("lcm(1549, 10831)", "16777219");
  // Integers overflowing int, but not uint32_t
  // 13
  approximates_to<float>("gcd(2147483650,13)", "undef");
  approximates_to<double>("gcd(2147483650,13)", "13");
  // 2147483650
  approximates_to<float>("lcm(2,25,13,41,61,1321)", "undef");
  approximates_to<double>("lcm(2,25,13,41,61,1321)", "2147483650");
  // Integers overflowing uint32_t
  // 13
  approximates_to<float>("gcd(4294967300,13)", "undef");
  // 13
  approximates_to<double>("gcd(4294967300,13)", "undef");
  // 4294967300
  approximates_to<float>("lcm(4,25,13,41,61,1321)", "undef");
  // 4294967300
  approximates_to<double>("lcm(4,25,13,41,61,1321)", "undef");
  // Integers that can't be accurately represented as double
  approximates_to<float>("gcd(1ᴇ16,10)", "undef");
  approximates_to<double>("gcd(1ᴇ16,10)", "undef");

  approximates_to<float>("quo(29, 10)", "2");
  approximates_to<double>("quo(29, 10)", "2");
  approximates_to<float>("rem(29, 10)", "9");
  approximates_to<double>("rem(29, 10)", "9");

  approximates_to<float>("factor(-23/4)", "-5.75");
  approximates_to<double>("factor(-123/24)", "-5.125");
  approximates_to<float>("factor(i)", "undef");
}

QUIZ_CASE(pcj_approximation_floor_ceil_integer) {
  using namespace Poincare;
  using namespace Poincare::Internal;
  const char* expr1 = "floor(x * (x+1)^(-1) + x^2 * (x+1)^(-1))";
  const char* expr2 = "ceil(x * (x+1)^(-1) + x^2 * (x+1)^(-1))";
  Tree* t1 = parse(expr1);
  Tree* t2 = parse(expr2);
  Variables::ReplaceSymbol(t1, "x"_e, 0, ComplexSign::RealFinite());
  Variables::ReplaceSymbol(t2, "x"_e, 0, ComplexSign::RealFinite());
  Approximation::Context ctx;
  Approximation::Private::LocalContext localCtx =
      Approximation::Private::LocalContext(0.0, nullptr);
  ctx.m_localContext = &localCtx;
  constexpr double upperBound = 1000.0;
  for (double d = 0.0; d < upperBound; d += 1.0) {
    localCtx.setLocalValue(d);
    double result1 =
        Approximation::To<double>(t1, Approximation::Parameters{}, ctx);
    double result2 =
        Approximation::To<double>(t2, Approximation::Parameters{}, ctx);
    quiz_assert_print_if_failure(
        roughly_equal(result1, d, OMG::Float::EpsilonLax<double>(), true),
        expr1);
    quiz_assert_print_if_failure(
        roughly_equal(result2, d, OMG::Float::EpsilonLax<double>(), true),
        expr2);
  }
}
