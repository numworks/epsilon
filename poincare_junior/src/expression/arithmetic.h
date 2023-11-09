#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare_junior/src/memory/tree.h>

#include "integer.h"

namespace PoincareJ {

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

  // Turn ceil, frac and round into floor
  static bool ExpandDecimals(Tree* expr);
  static bool ContractDecimals(Tree* expr);

  // Turn binomial and permute into factorials
  static bool SimplifyPermute(Tree* expr);
  static bool ExpandPermute(Tree* expr);
  static bool SimplifyBinomial(Tree* expr);
  static bool ExpandBinomial(Tree* expr);

  static bool SimplifyGCD(Tree* expr) { return SimplifyGCDOrLCM(expr, true); }
  static bool SimplifyLCM(Tree* expr) { return SimplifyGCDOrLCM(expr, false); }

  static bool BeautifyFactor(Tree* expr);

  static FactorizedInteger PrimeFactorization(IntegerHandler m);

 private:
  static bool SimplifyGCDOrLCM(Tree* expr, bool isGCD);
  static Tree* PushPrimeFactorization(IntegerHandler m);
};

}  // namespace PoincareJ
#endif
