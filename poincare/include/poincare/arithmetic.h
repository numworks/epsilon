#ifndef POINCARE_ARITHMETIC_H
#define POINCARE_ARITHMETIC_H

#include <poincare/integer.h>
namespace Poincare {

class Arithmetic {
public:
  static IntegerReference LCM(const IntegerReference i, const IntegerReference j);
  static IntegerReference GCD(const IntegerReference i, const IntegerReference j);
  /* When outputCoefficients[0] is set to -1, that indicates a special case:
   * i could not be factorized. */
  static void PrimeFactorization(const IntegerReference i, IntegerReference outputFactors[], IntegerReference outputCoefficients[], int outputLength);
  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfPrimeFactors = 32;
private:
  /* When decomposing an integer into primes factors, we look for its prime
   * factors among integer from 2 to 10000. */
  constexpr static int k_biggestPrimeFactor = 10000;
};

}

#endif
