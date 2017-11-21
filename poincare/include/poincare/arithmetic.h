#ifndef POINCARE_ARITHMETIC_H
#define POINCARE_ARITHMETIC_H

#include <poincare/integer.h>
namespace Poincare {

class Arithmetic {
public:
  static Integer LCM(const Integer * i, const Integer * j);
  static Integer GCD(const Integer * i, const Integer * j);
  static void PrimeFactorization(const Integer * i, Integer * outputFactors, Integer * outputCoefficients, int outputLength);
  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfPrimeFactors = 32;
  static const Integer k_primorial32;
};

}

#endif
