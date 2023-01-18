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
  /* GCD cannot overflow as GCD(a,b) <=  max(|a|,|b|)
   * we add hasOverflown's parameters anyway to match LCM's signature. */
  static uint32_t GCD(uint32_t i, uint32_t j, bool * hasOverflown = nullptr);
  static uint32_t LCM(uint32_t i, uint32_t j, bool * hasOverflown);
  template<typename T> static Evaluation<T> GCD(const ExpressionNode & expressionNode, const ApproximationContext& approximationContext);
  template<typename T> static Evaluation<T> LCM(const ExpressionNode & expressionNode, const ApproximationContext& approximationContext);

  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfFactors = 32;
  constexpr static int k_maxNumberOfDivisors = 2 * k_maxNumberOfFactors;
  constexpr static int k_errorTooManyFactors = -1;
  constexpr static int k_errorFactorTooLarge = -2;

  /* To save memory on stack, arrays of factor and coefficients for prime
   * factorization are static and shared. When calling PrimeFactorization or
   * PositiveDivisors from an Arithmetic instance, statics elements are locked
   * and only accessible from this instance. */
  Arithmetic() {}
  ~Arithmetic() { resetLock(); }

  /* When output is negative that indicates a special case:
   *  - -1 : too many factors.
   *  - -2 : a prime factor is too big.
   * Before calling PrimeFactorization, we instantiate an Arithmetic object.
   * Outputs are retrieved using (factor|coefficient|divisor)AtIndex(index)
   * methods. */
  int PrimeFactorization(const Integer & i);
  /* Method PositiveDivisors follows the same API as PrimeFactorization, as it
   * uses the same array. */
  int PositiveDivisors(const Integer & i);

  static void resetLock();

  Integer * factorAtIndex(int index) {
    assert(index < k_maxNumberOfFactors);
    assert(this == s_lock);
    return factors() + index;
  }
  Integer * coefficientAtIndex(int index) {
    assert(index < k_maxNumberOfFactors);
    assert(this == s_lock);
    return coefficients() + index;
  }
  Integer * divisorAtIndex(int index) {
    assert(index < k_maxNumberOfDivisors);
    assert(this == s_lock);
    return index < k_maxNumberOfFactors ? factorAtIndex(index) : coefficientAtIndex(index - k_maxNumberOfFactors);
  }

private:
  /* When decomposing an integer into primes factors, we look for its prime
   * factors among integer from 2 to 10000. */
  constexpr static int k_biggestPrimeFactor = 10000;
  static Arithmetic * s_lock;
  /* The following methods are equivalent to a simple static array declaration
   * in the header and an initialization in the source file. However, as Integer
   * itself rely on static objects, such a declaration could cause a static
   * init order fiasco. Here, the object is created on first use only. */
  static Integer * factors() {
    static Integer staticFactors[k_maxNumberOfFactors];
    return staticFactors;
  }

  static Integer * coefficients() {
    static Integer staticCoefficients[k_maxNumberOfFactors];
    return staticCoefficients;
  }
};

}

#endif
