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
    uint8_t numberOfFactors = 0;
  };

  static bool SimplifyFactorial(Tree* expr);
  static bool ExpandFactorial(Tree* expr);
  static bool SimplifyQuotientOrRemainder(Tree* expr);
  static bool SimplifyFloor(Tree* expr);
  static bool SimplifyRound(Tree* expr);
  static bool SimplifyFactor(Tree* expr);

  // Turn binomial and permute into factorials
  static bool SimplifyPermute(Tree* expr);
  static bool ExpandPermute(Tree* expr);
  static bool SimplifyBinomial(Tree* expr);
  static bool ExpandBinomial(Tree* expr);

  static bool SimplifyGCD(Tree* expr) { return SimplifyGCDOrLCM(expr, true); }
  static bool SimplifyLCM(Tree* expr) { return SimplifyGCDOrLCM(expr, false); }

  static bool BeautifyFactor(Tree* expr);

  static FactorizedInteger PrimeFactorization(IntegerHandler m);

  static uint32_t GCD(uint32_t a, uint32_t b);
  static uint32_t LCM(uint32_t a, uint32_t b, bool* hasOverflown);

 private:
  static bool SimplifyGCDOrLCM(Tree* expr, bool isGCD);
  static Tree* PushPrimeFactorization(IntegerHandler m);
};

}  // namespace Poincare::Internal
#endif
