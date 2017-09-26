#ifndef POINCARE_ARITHMETIC_H
#define POINCARE_ARITHMETIC_H

#include <poincare/integer.h>
namespace Poincare {

class Arithmetic {
public:
  static Integer GCD(Integer i, Integer j);
  static void PrimeFactorization(Integer i, Integer * outputFactors, Integer * outputCoefficients, int outputLength);
  constexpr static int k_numberOfPrimeFactors = 1000;
};

}

#endif
