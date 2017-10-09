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
  GlobalContext context;
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- GCD ----"  << endl;
  cout << "gcd(" << a.approximate<float>(context);
  cout << ", " << b.approximate<float>(context) << ") = ";
#endif
  Integer gcd = Arithmetic::GCD(&a, &b);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << gcd.approximate<float>(context) << endl;
#endif
  assert(gcd.compareTo(&c) == 0);
}

void assert_prime_factorization_equals_to(Integer a, int * factors, int * coefficients, int length) {
  GlobalContext context;
  Integer outputFactors[1000];
  Integer outputCoefficients[1000];
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Primes factorization ----"  << endl;
  cout << "Decomp(" << a.approximate<float>(context) << ") = ";
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
    assert(outputFactors[index].approximate<float>(context) == Integer(factors[index]).approximate<float>(context));
    assert(outputCoefficients[index].approximate<float>(context) == Integer(coefficients[index]).approximate<float>(context));
  }
}

QUIZ_CASE(poincare_arithmetic) {
  assert_gcd_equals_to(Integer(11), Integer(121), Integer(11));
  assert_gcd_equals_to(Integer(-256), Integer(321), Integer(1));
  assert_gcd_equals_to(Integer(-8), Integer(-40), Integer(8));
  assert_gcd_equals_to(Integer("1234567899876543456", true), Integer("234567890098765445678"), Integer(2));
  assert_gcd_equals_to(Integer("45678998789"), Integer("1461727961248"), Integer("45678998789"));
  int factors0[5] = {2,3,5,79,1319};
  int coefficients0[5] = {2,1,1,1,1};
  assert_prime_factorization_equals_to(Integer(6252060), factors0, coefficients0, 5);
  int factors1[3] = {3,2969, 6907};
  int coefficients1[3] = {1,1,1};
  assert_prime_factorization_equals_to(Integer(61520649), factors1, coefficients1, 3);
  int factors2[3] = {2,5, 7};
  int coefficients2[3] = {2,4,2};
  assert_prime_factorization_equals_to(Integer(122500), factors2, coefficients2, 3);
  int factors3[8] = {3,7,11, 13, 19, 3607, 3803, 52579};
  int coefficients3[8] = {4,2,2,2,2,2,2,2};
  assert_prime_factorization_equals_to(Integer("15241578780673678515622620750190521"), factors3, coefficients3, 8);
}
