#include <quiz.h>
#include <poincare.h>
#include <poincare/arithmetic.h>
#include <assert.h>
#include <utility>

#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;

void assert_gcd_equals_to(Integer a, Integer b, Integer c) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- GCD ----"  << endl;
  cout << "gcd(" << a.approximate<float>();
  cout << ", " << b.approximate<float>() << ") = ";
#endif
  Integer gcd = Arithmetic::GCD(&a, &b);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << gcd.approximate<float>() << endl;
#endif
  assert(gcd.isEqualTo(c));
}

void assert_lcm_equals_to(Integer a, Integer b, Integer c) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- LCM ----"  << endl;
  cout << "lcm(" << a.approximate<float>();
  cout << ", " << b.approximate<float>() << ") = ";
#endif
  Integer lcm = Arithmetic::LCM(&a, &b);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << lcm.approximate<float>() << endl;
#endif
  assert(lcm.isEqualTo(c));
}

void assert_prime_factorization_equals_to(Integer a, int * factors, int * coefficients, int length) {
  Integer outputFactors[100];
  Integer outputCoefficients[100];
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Primes factorization ----"  << endl;
  cout << "Decomp(" << a.approximate<float>() << ") = ";
#endif
  Arithmetic::PrimeFactorization(&a, outputFactors, outputCoefficients, 10);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  print_prime_factorization(outputFactors, outputCoefficients, 10);
#endif
  for (int index = 0; index < length; index++) {
    if (outputCoefficients[index].isEqualTo(Integer(0))) {
      break;
    }
    /* Cheat: instead of comparing to integers, we compare their approximations
     * (the relation between integers and their approximation is a surjection,
     * however different integers are really likely to have different
     * approximations... */
    assert(outputFactors[index].approximate<float>() == Integer(factors[index]).approximate<float>());
    assert(outputCoefficients[index].approximate<float>() == Integer(coefficients[index]).approximate<float>());
  }
}

QUIZ_CASE(poincare_arithmetic) {
  assert_gcd_equals_to(Integer(11), Integer(121), Integer(11));
  assert_gcd_equals_to(Integer(-256), Integer(321), Integer(1));
  assert_gcd_equals_to(Integer(-8), Integer(-40), Integer(8));
  assert_gcd_equals_to(Integer("1234567899876543456", true), Integer("234567890098765445678"), Integer(2));
  assert_gcd_equals_to(Integer("45678998789"), Integer("1461727961248"), Integer("45678998789"));
  assert_lcm_equals_to(Integer(11), Integer(121), Integer(121));
  assert_lcm_equals_to(Integer(-31), Integer(52), Integer(1612));
  assert_lcm_equals_to(Integer(-8), Integer(-40), Integer(40));
  assert_lcm_equals_to(Integer("1234567899876543456", true), Integer("234567890098765445678"), Integer("144794993728852353909143567804987191584"));
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
