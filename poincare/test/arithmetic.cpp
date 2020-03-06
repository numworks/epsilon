#include <poincare/arithmetic.h>
#include <utility>
#include "helper.h"

using namespace Poincare;

void fill_buffer_with(char * buffer, size_t bufferSize, const char * functionName, Integer * a, int numberOfIntegers) {
  int numberOfChar = strlcpy(buffer, functionName, bufferSize);
  for (int i = 0; i < numberOfIntegers; i++) {
    if (i > 0) {
      numberOfChar += strlcpy(buffer+numberOfChar, ", ", bufferSize-numberOfChar);
    }
    numberOfChar += a[i].serialize(buffer+numberOfChar, bufferSize-numberOfChar);
  }
  strlcpy(buffer+numberOfChar, ")", bufferSize-numberOfChar);
}

void assert_gcd_equals_to(Integer a, Integer b, Integer c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  Integer args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "gcd(", args, 2);
  Integer gcd = Arithmetic::GCD(a, b);
  quiz_assert_print_if_failure(gcd.isEqualTo(c), failInformationBuffer);
}

void assert_lcm_equals_to(Integer a, Integer b, Integer c) {
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  Integer args[2] = {a, b};
  fill_buffer_with(failInformationBuffer, bufferSize, "lcm(", args, 2);
  Integer lcm = Arithmetic::LCM(a, b);
  quiz_assert_print_if_failure(lcm.isEqualTo(c), failInformationBuffer);
}

void assert_prime_factorization_equals_to(Integer a, int * factors, int * coefficients, int length) {
  Integer outputFactors[100];
  Integer outputCoefficients[100];
  Arithmetic::PrimeFactorization(a, outputFactors, outputCoefficients, 10);
  constexpr size_t bufferSize = 100;
  char failInformationBuffer[bufferSize];
  fill_buffer_with(failInformationBuffer, bufferSize, "factor(", &a, 1);
  for (int index = 0; index < length; index++) {
    /* Cheat: instead of comparing to integers, we compare their approximations
     * (the relation between integers and their approximation is a surjection,
     * however different integers are really likely to have different
     * approximations... */
    quiz_assert_print_if_failure(outputFactors[index].approximate<float>() == Integer(factors[index]).approximate<float>(), failInformationBuffer);
    quiz_assert_print_if_failure(outputCoefficients[index].approximate<float>() == Integer(coefficients[index]).approximate<float>(), failInformationBuffer);
  }
}

QUIZ_CASE(poincare_arithmetic_gcd) {
  assert_gcd_equals_to(Integer(11), Integer(121), Integer(11));
  assert_gcd_equals_to(Integer(-256), Integer(321), Integer(1));
  assert_gcd_equals_to(Integer(-8), Integer(-40), Integer(8));
  assert_gcd_equals_to(Integer("1234567899876543456"), Integer("234567890098765445678"), Integer(2));
  assert_gcd_equals_to(Integer("45678998789"), Integer("1461727961248"), Integer("45678998789"));
}

QUIZ_CASE(poincare_arithmetic_lcm) {
  assert_lcm_equals_to(Integer(11), Integer(121), Integer(121));
  assert_lcm_equals_to(Integer(-31), Integer(52), Integer(1612));
  assert_lcm_equals_to(Integer(-8), Integer(-40), Integer(40));
  assert_lcm_equals_to(Integer("1234567899876543456"), Integer("234567890098765445678"), Integer("144794993728852353909143567804987191584"));
}

QUIZ_CASE(poincare_arithmetic_factorization) {
  assert_lcm_equals_to(Integer("45678998789"), Integer("1461727961248"), Integer("1461727961248"));
  int factors0[5] = {2,3,5,79,1319};
  int coefficients0[5] = {2,1,1,1,1};
  assert_prime_factorization_equals_to(Integer(6252060), factors0, coefficients0, 5);
  int factors1[3] = {3,2969, 6907};
  int coefficients1[3] = {1,1,1};
  assert_prime_factorization_equals_to(Integer(61520649), factors1, coefficients1, 3);
  int factors2[3] = {2,5, 7};
  int coefficients2[3] = {2,4,2};
  assert_prime_factorization_equals_to(Integer(122500), factors2, coefficients2, 3);
  int factors3[7] = {3,7,11, 13, 19, 3607, 3803};
  int coefficients3[7] = {4,2,2,2,2,2,2};
  assert_prime_factorization_equals_to(Integer("5513219850886344455940081"), factors3, coefficients3, 7);
}
