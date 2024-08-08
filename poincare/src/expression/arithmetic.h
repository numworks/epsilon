#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare/src/memory/tree.h>

#include "integer.h"

namespace Poincare::Internal {

class Arithmetic {
 public:
  struct FactorizedInteger {
    constexpr static int k_maxNumberOfFactors = 32;
    uint16_t factors[k_maxNumberOfFactors];
    uint8_t coefficients[k_maxNumberOfFactors];
    constexpr static int k_factorizationFailed = -1;
    int8_t numberOfFactors = 0;
  };

  static bool ReduceFactorial(Tree* e);
  static bool ExpandFactorial(Tree* e);
  static bool ReduceQuotientOrRemainder(Tree* e);
  static bool ReduceFloor(Tree* e);
  static bool ReduceRound(Tree* e);
  static bool ReduceFactor(Tree* e);

  // Turn binomial and permute into factorials
  static bool ReducePermute(Tree* e);
  static bool ExpandPermute(Tree* e);
  static bool ReduceBinomial(Tree* e);
  static bool ExpandBinomial(Tree* e);

  static bool ReduceGCD(Tree* e) { return ReduceGCDOrLCM(e, true); }
  static bool ReduceLCM(Tree* e) { return ReduceGCDOrLCM(e, false); }

  static bool BeautifyFactor(Tree* e);

  static FactorizedInteger PrimeFactorization(IntegerHandler m);

  static uint32_t GCD(uint32_t a, uint32_t b);
  static uint32_t LCM(uint32_t a, uint32_t b, bool* hasOverflown);

 private:
  static bool ReduceGCDOrLCM(Tree* e, bool isGCD);
  static Tree* PushPrimeFactorization(IntegerHandler m);
};

}  // namespace Poincare::Internal
#endif
