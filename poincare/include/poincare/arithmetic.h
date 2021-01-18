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
  template<typename T> static Evaluation<T> GCD(const ExpressionNode & expressionNode, ExpressionNode::ApproximationContext approximationContext);
  template<typename T> static Evaluation<T> LCM(const ExpressionNode & expressionNode, ExpressionNode::ApproximationContext approximationContext);
  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfPrimeFactors = 32;

  /* To save memory on stack, arrays of factor and coefficients for prime
   * factorization are static and shared.
   * An error is returned if a second non-destructed instance of Arithmetic
   * calls PrimeFactorization. This situation must be prevented. */
  Arithmetic() {};
  ~Arithmetic();
  /* When output is negative that indicates a special case:
   *  - -1 : too many factors.
   *  - -2 : a prime factor is too big.
   *  - -3 : an other instance is using prime factorization
   * Before calling PrimeFactorization, we instantiate an Arithmetic object.
   * Outputs are retrieved using getFactorization_(index) methods. */
  int PrimeFactorization(const Integer & i);
  Integer * getFactorizationFactor(int index) {
    assert(index < k_maxNumberOfPrimeFactors);
    return s_factorizationFactors + index;
  }
  Integer * getFactorizationCoefficient(int index) {
    assert(index < k_maxNumberOfPrimeFactors);
    return s_factorizationCoefficients + index;
  }

private:
  /* When decomposing an integer into primes factors, we look for its prime
   * factors among integer from 2 to 10000. */
  constexpr static int k_biggestPrimeFactor = 10000;
  static bool s_factorizationLock;
  static Integer s_factorizationFactors[k_maxNumberOfPrimeFactors];
  static Integer s_factorizationCoefficients[k_maxNumberOfPrimeFactors];
};

}

#endif
