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
  template<typename T> static Evaluation<T> GCD(const ExpressionNode & expressionNode, ExpressionNode::ApproximationContext approximationContext);
  template<typename T> static Evaluation<T> LCM(const ExpressionNode & expressionNode, ExpressionNode::ApproximationContext approximationContext);

  constexpr static int k_numberOfPrimeFactors = 1000;
  constexpr static int k_maxNumberOfFactors = 32;
  constexpr static int k_maxNumberOfDivisors = 2 * k_maxNumberOfFactors;
  constexpr static int k_errorTooManyFactors = -1;
  constexpr static int k_errorFactorTooLarge = -2;
  constexpr static int k_errorAlreadyInUse = -3;

  /* To save memory on stack, arrays of factor and coefficients for prime
   * factorization are static and shared.
   * An error is returned if a second non-destructed instance of Arithmetic
   * calls PrimeFactorization. This situation must be prevented. */
  Arithmetic() {}
  ~Arithmetic() { resetLock(); }

  /* When output is negative that indicates a special case:
   *  - -1 : too many factors.
   *  - -2 : a prime factor is too big.
   *  - -3 : an other instance is using prime factorization
   * Before calling PrimeFactorization, we instantiate an Arithmetic object.
   * Outputs are retrieved using getFactorization_(index) methods. */
  int PrimeFactorization(const Integer & i);
  /* Method PositiveDivisors follows the same API as PrimeFactorization, as it
   * uses the same array. */
  int PositiveDivisors(const Integer & i);

  /* Factorization's lock can be compromised if an exception is raised while it
   * is locked. To prevent that, any prime factorization must be enclosed within
   * an intermediary ExceptionCheckpoint. resetPrimeFactorization() shall be
   * called in the associated ErrorHandler and, once the intermediary
   * ExceptionCheckpoint has been destroyed, another exception shall be manually
   * raised to fall back on the intended checkpoint. */
  static void resetLock();

  static Integer * factorAtIndex(int index) {
    assert(index < k_maxNumberOfFactors);
    return factors() + index;
  }
  static Integer * coefficientAtIndex(int index) {
    assert(index < k_maxNumberOfFactors);
    return coefficients() + index;
  }
  static Integer * divisorAtIndex(int index) {
    assert(index < k_maxNumberOfDivisors);
    return index < k_maxNumberOfFactors ? factorAtIndex(index) : coefficientAtIndex(index - k_maxNumberOfFactors);
  }

private:
  /* When decomposing an integer into primes factors, we look for its prime
   * factors among integer from 2 to 10000. */
  constexpr static int k_biggestPrimeFactor = 10000;
  static bool s_lock;
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
