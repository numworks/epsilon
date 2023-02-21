#include <poincare/arithmetic.h>

#include <utility>

#include "helper.h"
#include "limits.h"

using namespace Poincare;

void fill_buffer_with(char* buffer, size_t bufferSize, const char* functionName,
                      Integer* a, int numberOfIntegers) {
  int numberOfChar = strlcpy(buffer, functionName, bufferSize);
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

void assert_gcd_equals_to(Integer a, Integer b, Integer c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  Integer args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "gcd(", args, 2);
  Integer gcd = Arithmetic::GCD(a, b);
  quiz_assert_print_if_failure(gcd.isEqualTo(c), failInformationBuffer);
  if (a.isExtractable() && b.isExtractable()) {
    // Test Arithmetic::GCD(int, int) if possible
    bool isUndefined = false;
    a.setNegative(false);
    b.setNegative(false);
    int extractedGcd =
        Arithmetic::GCD(a.extractedInt(), b.extractedInt(), &isUndefined);
    quiz_assert_print_if_failure(
        c.isExtractable() ? extractedGcd == c.extractedInt() : isUndefined,
        failInformationBuffer);
  }
}

void assert_lcm_equals_to(Integer a, Integer b, Integer c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  Integer args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "lcm(", args, 2);
  Integer lcm = Arithmetic::LCM(a, b);
  quiz_assert_print_if_failure(lcm.isEqualTo(c), failInformationBuffer);
  if (a.isExtractable() && b.isExtractable()) {
    // Test Arithmetic::LCM(int, int) if possible
    bool isUndefined = false;
    a.setNegative(false);
    b.setNegative(false);
    int extractedLcm =
        Arithmetic::LCM(a.extractedInt(), b.extractedInt(), &isUndefined);
    quiz_assert_print_if_failure(
        c.isExtractable() ? extractedLcm == c.extractedInt() : isUndefined,
        failInformationBuffer);
  }
}

void assert_prime_factorization_equals_to(Integer a, int* factors,
                                          int* coefficients, int length) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  fill_buffer_with(failInformationBuffer, bufferSize, "factor(", &a, 1);
  Arithmetic arithmetic;
  int tempAValue = a.isExtractable() ? a.extractedInt() : INT_MAX;
  int n = arithmetic.PrimeFactorization(a);
  // a should remain unchanged
  quiz_assert_print_if_failure(
      tempAValue == (a.isExtractable() ? a.extractedInt() : INT_MAX),
      failInformationBuffer);
  quiz_assert_print_if_failure(n == length, failInformationBuffer);
  for (int index = 0; index < length; index++) {
    /* Cheat: instead of comparing to integers, we compare their
     * approximations (the relation between integers and their approximation
     * is a surjection, however different integers are really likely to have
     * different approximations... */
    quiz_assert_print_if_failure(
        arithmetic.factorAtIndex(index)->approximate<float>() ==
            Integer(factors[index]).approximate<float>(),
        failInformationBuffer);
    quiz_assert_print_if_failure(
        arithmetic.coefficientAtIndex(index)->approximate<float>() ==
            Integer(coefficients[index]).approximate<float>(),
        failInformationBuffer);
  }
}

template <int N>
void assert_divisors_equal_to(Integer a, int const (&divisors)[N]) {
  Arithmetic arithmetic;
  int tempAValue = a.isExtractable() ? a.extractedInt() : INT_MAX;
  int numberOfDivisors = arithmetic.PositiveDivisors(a);
  if (numberOfDivisors == Arithmetic::k_errorTooManyFactors) {
    numberOfDivisors = Arithmetic::k_maxNumberOfDivisors;
  } else {
    assert(numberOfDivisors >= 0);
  }
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  fill_buffer_with(failInformationBuffer, bufferSize, "divisors(", &a, 1);
  // a should remain unchanged
  quiz_assert_print_if_failure(
      tempAValue == (a.isExtractable() ? a.extractedInt() : INT_MAX),
      failInformationBuffer);
  quiz_assert_print_if_failure(numberOfDivisors == N, failInformationBuffer);
  for (int i = 0; i < N; i++) {
    quiz_assert_print_if_failure(
        arithmetic.divisorAtIndex(i)->approximate<float>() ==
            Integer(divisors[i]).approximate<float>(),
        failInformationBuffer);
  }
}

QUIZ_CASE(poincare_arithmetic_gcd) {
  assert_gcd_equals_to(Integer(11), Integer(121), Integer(11));
  assert_gcd_equals_to(Integer(-256), Integer(321), Integer(1));
  assert_gcd_equals_to(Integer(-8), Integer(-40), Integer(8));
  assert_gcd_equals_to(Integer("1234567899876543456"),
                       Integer("234567890098765445678"), Integer(2));
  assert_gcd_equals_to(Integer("45678998789"), Integer("1461727961248"),
                       Integer("45678998789"));
}

QUIZ_CASE(poincare_arithmetic_lcm) {
  assert_lcm_equals_to(Integer(11), Integer(121), Integer(121));
  assert_lcm_equals_to(Integer(-31), Integer(52), Integer(1612));
  assert_lcm_equals_to(Integer(-8), Integer(-40), Integer(40));
  assert_lcm_equals_to(Integer("1234567899876543456"),
                       Integer("234567890098765445678"),
                       Integer("144794993728852353909143567804987191584"));
  // Inputs are extractable, but not the output.
  assert_lcm_equals_to(Integer(24278576), Integer(23334),
                       Integer("283258146192"));
}

QUIZ_CASE(poincare_arithmetic_factorization) {
  assert_lcm_equals_to(Integer("45678998789"), Integer("1461727961248"),
                       Integer("1461727961248"));
  int factors0[5] = {2, 3, 5, 79, 1319};
  int coefficients0[5] = {2, 1, 1, 1, 1};
  assert_prime_factorization_equals_to(Integer(6252060), factors0,
                                       coefficients0, 5);
  int factors1[3] = {3, 2969, 6907};
  int coefficients1[3] = {1, 1, 1};
  assert_prime_factorization_equals_to(Integer(61520649), factors1,
                                       coefficients1, 3);
  int factors2[3] = {2, 5, 7};
  int coefficients2[3] = {2, 4, 2};
  assert_prime_factorization_equals_to(Integer(122500), factors2, coefficients2,
                                       3);
  int factors3[7] = {3, 7, 11, 13, 19, 3607, 3803};
  int coefficients3[7] = {4, 2, 2, 2, 2, 2, 2};
  assert_prime_factorization_equals_to(Integer("5513219850886344455940081"),
                                       factors3, coefficients3, 7);
  int factors4[2] = {8017, 8039};
  int coefficients4[2] = {1, 1};
  assert_prime_factorization_equals_to(Integer(8017 * 8039), factors4,
                                       coefficients4, 2);
  int factors5[1] = {10007};
  int coefficients5[1] = {1};
  assert_prime_factorization_equals_to(Integer(10007), factors5, coefficients5,
                                       1);
  int factors6[0] = {};
  int coefficients6[0] = {};
  assert_prime_factorization_equals_to(Integer(10007 * 10007), factors6,
                                       coefficients6, -2);
  int factors7[0] = {};
  int coefficients7[0] = {};
  assert_prime_factorization_equals_to(Integer(1), factors7, coefficients7, 0);
}

QUIZ_CASE(poincare_arithmetic_divisors) {
  quiz_assert_print_if_failure(Arithmetic().PositiveDivisors(Integer(0)) ==
                                   Arithmetic::k_errorTooManyFactors,
                               "divisors(0)");
  assert_divisors_equal_to(Integer(1), {1});
  assert_divisors_equal_to(Integer(2), {1, 2});
  assert_divisors_equal_to(Integer(-12), {1, 2, 3, 4, 6, 12});
  assert_divisors_equal_to(Integer(-100), {1, 2, 4, 5, 10, 20, 25, 50, 100});
  assert_divisors_equal_to(Integer(225), {1, 3, 5, 9, 15, 25, 45, 75, 225});
  assert_divisors_equal_to(
      Integer(1680),
      {1,   2,   3,   4,   5,   6,   7,   8,   10,  12,  14,  15,  16, 20,
       21,  24,  28,  30,  35,  40,  42,  48,  56,  60,  70,  80,  84, 105,
       112, 120, 140, 168, 210, 240, 280, 336, 420, 560, 840, 1680});
  assert_divisors_equal_to(Integer(INT_MAX), {1, INT_MAX});
  /* Too many divisors */
  assert_divisors_equal_to(
      Integer(10080),
      {1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  12,   14,  15,
       16,  18,  20,  21,  24,  28,  30,  32,  35,  36,  40,   42,  45,
       48,  56,  60,  63,  70,  72,  80,  84,  90,  96,  105,  112, 120,
       126, 140, 144, 160, 168, 180, 210, 224, 240, 252, 280,  288, 315,
       336, 360, 420, 480, 504, 560, 630, 672, 720, 840, 1008, 1120});
  quiz_assert_print_if_failure(Arithmetic().PositiveDivisors(Integer(10080)) ==
                                   Arithmetic::k_errorTooManyFactors,
                               "divisors(10080)");
  /* Factor too large */
  quiz_assert_print_if_failure(
      Arithmetic().PositiveDivisors(Integer(static_cast<int>(INT_MIN))) ==
          Arithmetic::k_errorFactorTooLarge,
      "divisors(INT_MIN)");
  quiz_assert_print_if_failure(
      Arithmetic().PositiveDivisors(Integer::Addition(
          Integer(INT_MAX), Integer(1))) == Arithmetic::k_errorFactorTooLarge,
      "divisors(INT_MAX+1)");
}
