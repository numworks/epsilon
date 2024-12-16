#include <omg/utf8_decoder.h>
#include <poincare/src/expression/arithmetic.h>
#include <poincare/src/expression/integer.h>

#include <utility>

#include "helper.h"
#include "limits.h"

using namespace Poincare::Internal;

void fill_buffer_with(char* buffer, size_t bufferSize, const char* functionName,
                      IntegerHandler* a, int numberOfIntegers) {
  size_t numberOfChar = strlcpy(buffer, functionName, bufferSize);
  for (int i = 0; i < numberOfIntegers; i++) {
    if (i > 0) {
      numberOfChar +=
          strlcpy(buffer + numberOfChar, ", ", bufferSize - numberOfChar);
    }
    numberOfChar +=
        a[i].serialize(buffer + numberOfChar, bufferSize - numberOfChar);
  }
  strlcpy(buffer + numberOfChar, ")", bufferSize - numberOfChar);
}

void assert_gcd_equals_to(IntegerHandler a, IntegerHandler b,
                          IntegerHandler c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  IntegerHandler args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "gcd(", args, 2);
  IntegerHandler gcd = Integer::Handler(IntegerHandler::GCD(a, b));
  quiz_assert_print_if_failure(IntegerHandler::Compare(gcd, c) == 0,
                               failInformationBuffer);
  if (a.is<int>() && b.is<int>()) {
    // Test Arithmetic::GCD(int, int)
    a.setSign(NonStrictSign::Positive);
    b.setSign(NonStrictSign::Positive);
    int extractedGcd = Arithmetic::GCD(a.to<int>(), b.to<int>());
    quiz_assert_print_if_failure(c.is<int>() && extractedGcd == c.to<int>(),
                                 failInformationBuffer);
  }
}

void assert_lcm_equals_to(IntegerHandler a, IntegerHandler b,
                          IntegerHandler c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  IntegerHandler args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "lcm(", args, 2);
  IntegerHandler lcm = Integer::Handler(IntegerHandler::LCM(a, b));
  quiz_assert_print_if_failure(IntegerHandler::Compare(lcm, c) == 0,
                               failInformationBuffer);
  if (a.is<int>() && b.is<int>()) {
    // Test Arithmetic::LCM(int, int) if possible
    bool isUndefined = false;
    a.setSign(NonStrictSign::Positive);
    b.setSign(NonStrictSign::Positive);
    int extractedLcm = Arithmetic::LCM(a.to<int>(), b.to<int>(), &isUndefined);
    quiz_assert_print_if_failure(
        c.is<int>() ? extractedLcm == c.to<int>() : isUndefined,
        failInformationBuffer);
  }
}

void assert_prime_factorization_equals_to(IntegerHandler a, uint32_t* factors,
                                          uint8_t* coefficients, int length) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  fill_buffer_with(failInformationBuffer, bufferSize, "factor(", &a, 1);
  int tempAValue = a.is<int>() ? a.to<int>() : INT_MAX;
  Arithmetic::FactorizedInteger f = Arithmetic::PrimeFactorization(a);
  // a should remain unchanged
  quiz_assert_print_if_failure(
      tempAValue == (a.is<int>() ? a.to<int>() : INT_MAX),
      failInformationBuffer);
  quiz_assert_print_if_failure(f.numberOfFactors == length,
                               failInformationBuffer);
  for (int index = 0; index < length; index++) {
    quiz_assert_print_if_failure(f.factors[index] == factors[index],
                                 failInformationBuffer);
    quiz_assert_print_if_failure(f.coefficients[index] == coefficients[index],
                                 failInformationBuffer);
  }
}

template <unsigned long N>
void assert_divisors_equal_to(uint32_t a,
                              const std::array<uint32_t, N>& expectedList) {
  static_assert(N <= Arithmetic::Divisors::k_maxNumberOfDivisors);
  Arithmetic::Divisors result = Arithmetic::ListPositiveDivisors(a);
  quiz_assert_print_if_failure(
      result.numberOfDivisors != Arithmetic::Divisors::k_divisorListFailed,
      "divisor list computation failed");
  quiz_assert_print_if_failure(result.numberOfDivisors == N,
                               "incorrect number of divisors");
  for (unsigned long i = 0; i < N; i++) {
    quiz_assert_print_if_failure(result.list[i] == expectedList[i],
                                 "incorrect divisor value");
  }
}

IntegerHandler ParseHandler(const char* str) {
  UTF8Decoder decoder(str);
  return Integer::Handler(Integer::Push(decoder, OMG::Base::Decimal));
}

QUIZ_CASE(poincare_arithmetic_gcd) {
  assert_gcd_equals_to(IntegerHandler(11), IntegerHandler(121),
                       IntegerHandler(11));
  assert_gcd_equals_to(IntegerHandler(-256), IntegerHandler(321),
                       IntegerHandler(1));
  assert_gcd_equals_to(IntegerHandler(-8), IntegerHandler(-40),
                       IntegerHandler(8));
  assert_gcd_equals_to(ParseHandler("1234567899876543456"),
                       ParseHandler("234567890098765445678"),
                       IntegerHandler(2));
  assert_gcd_equals_to(ParseHandler("45678998789"),
                       ParseHandler("1461727961248"),
                       ParseHandler("45678998789"));
}

QUIZ_CASE(poincare_arithmetic_lcm) {
  assert_lcm_equals_to(IntegerHandler(11), IntegerHandler(121),
                       IntegerHandler(121));
  assert_lcm_equals_to(IntegerHandler(-31), IntegerHandler(52),
                       IntegerHandler(1612));
  assert_lcm_equals_to(IntegerHandler(-8), IntegerHandler(-40),
                       IntegerHandler(40));
  assert_lcm_equals_to(ParseHandler("1234567899876543456"),
                       ParseHandler("234567890098765445678"),
                       ParseHandler("144794993728852353909143567804987191584"));
  // Inputs are extractable, but not the output.
  assert_lcm_equals_to(IntegerHandler(24278576), IntegerHandler(23334),
                       ParseHandler("283258146192"));
  assert_lcm_equals_to(ParseHandler("45678998789"),
                       ParseHandler("1461727961248"),
                       ParseHandler("1461727961248"));
}

QUIZ_CASE(poincare_arithmetic_factorization) {
  uint32_t factors0[5] = {2, 3, 5, 79, 1319};
  uint8_t coefficients0[5] = {2, 1, 1, 1, 1};
  assert_prime_factorization_equals_to(IntegerHandler(6252060), factors0,
                                       coefficients0, 5);
  uint32_t factors1[3] = {3, 2969, 6907};
  uint8_t coefficients1[3] = {1, 1, 1};
  assert_prime_factorization_equals_to(IntegerHandler(61520649), factors1,
                                       coefficients1, 3);
  uint32_t factors2[3] = {2, 5, 7};
  uint8_t coefficients2[3] = {2, 4, 2};
  assert_prime_factorization_equals_to(IntegerHandler(122500), factors2,
                                       coefficients2, 3);

  uint32_t factors3[7] = {3, 7, 11, 13, 19, 3607, 3803};
  uint8_t coefficients3[7] = {4, 2, 2, 2, 2, 2, 2};
  assert_prime_factorization_equals_to(
      ParseHandler("5513219850886344455940081"), factors3, coefficients3, 7);

  uint32_t factors4[2] = {8017, 8039};
  uint8_t coefficients4[2] = {1, 1};
  assert_prime_factorization_equals_to(IntegerHandler(8017 * 8039), factors4,
                                       coefficients4, 2);
  uint32_t factors5[1] = {10007};
  uint8_t coefficients5[1] = {1};
  assert_prime_factorization_equals_to(IntegerHandler(10007), factors5,
                                       coefficients5, 1);

  uint32_t factors6[0] = {};
  uint8_t coefficients6[0] = {};
  assert_prime_factorization_equals_to(
      IntegerHandler(10007 * 10007), factors6, coefficients6,
      Arithmetic::FactorizedInteger::k_factorizationFailed);

  uint32_t factors7[0] = {};
  uint8_t coefficients7[0] = {};
  assert_prime_factorization_equals_to(IntegerHandler(1), factors7,
                                       coefficients7, 0);

  uint32_t factors8[1] = {101119};
  uint8_t coefficients8[1] = {1};
  assert_prime_factorization_equals_to(IntegerHandler(101119), factors8,
                                       coefficients8, 1);
}

QUIZ_CASE(poincare_arithmetic_divisors) {
  quiz_assert_print_if_failure(
      Arithmetic::ListPositiveDivisors(0).numberOfDivisors ==
          Arithmetic::Divisors::k_divisorListFailed,
      "divisors(0)");

  assert_divisors_equal_to<1>(1, {1});
  assert_divisors_equal_to<2>(2, {1, 2});
  assert_divisors_equal_to<6>(12, {1, 2, 3, 4, 6, 12});
  assert_divisors_equal_to<9>(100, {1, 2, 4, 5, 10, 20, 25, 50, 100});
  assert_divisors_equal_to<9>(225, {1, 3, 5, 9, 15, 25, 45, 75, 225});
  assert_divisors_equal_to<40>(
      1680,
      {1,   2,   3,   4,   5,   6,   7,   8,   10,  12,  14,  15,  16, 20,
       21,  24,  28,  30,  35,  40,  42,  48,  56,  60,  70,  80,  84, 105,
       112, 120, 140, 168, 210, 240, 280, 336, 420, 560, 840, 1680});
  assert_divisors_equal_to<2>(INT_MAX, {1, INT_MAX});

  /* Too many divisors */
  quiz_assert_print_if_failure(
      Arithmetic::ListPositiveDivisors(10080).numberOfDivisors ==
          Arithmetic::Divisors::k_divisorListFailed,
      "divisors(10080)");
}
