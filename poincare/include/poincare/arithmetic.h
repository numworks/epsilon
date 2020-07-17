#ifndef POINCARE_ARITHMETIC_H
#define POINCARE_ARITHMETIC_H

#include <poincare/integer.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Arithmetic final {
public:
  static Integer GCD(const Integer & i, const Integer & j);
  static Integer LCM(const Integer & i, const Integer & j);
  static Expression GCD(const Expression & expression);
  static Expression LCM(const Expression & expression);
  static int GCD(int i, int j);
  static int LCM(int i, int j);
  template<typename T> static Evaluation<T> GCD(const ExpressionNode & expressionNode, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template<typename T> static Evaluation<T> LCM(const ExpressionNode & expressionNode, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  /* When outputCoefficients[0] is set to -1, that indicates a special case:
   * i could not be factorized.
   * Before calling PrimeFactorization, we initiate two tables of Integers
   * (outputFactors & outputCoefficients) of length k_maxNumberOfPrimeFactors = 32.
   * As explained in 'integer.h', we guarantee the existence of only 16 Integers
   * simultaneously. However, primeFactors are capped by
   * k_biggestPrimeFactor*k_biggestPrimeFactor and they thus benefit from the
   * small integer optimization. Thereby, the tables will not allocate any
   * Integer on the static Integer table that limit the number of Integer
   * simultaneously alive. */
  static int PrimeFactorization(const Integer & i, Integer outputFactors[], Integer outputCoefficients[], int outputLength);
  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfPrimeFactors = 32;
private:
  /* When decomposing an integer into primes factors, we look for its prime
   * factors among integer from 2 to 10000. */
  constexpr static int k_biggestPrimeFactor = 10000;
};

}

#endif
