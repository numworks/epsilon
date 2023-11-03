#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare_junior/src/memory/tree.h>

#include "integer.h"

namespace PoincareJ {

class Arithmetic {
 public:
  static bool SimplifyQuotientOrRemainder(Tree* expr);
  static bool SimplifyFloor(Tree* expr);

  // Turn ceil, frac and round into floor
  static bool ExpandDecimals(Tree* expr);
  static bool ContractDecimals(Tree* expr);

  static bool SimplifyGCD(Tree* expr) { return SimplifyGCDOrLCM(expr, true); }
  static bool SimplifyLCM(Tree* expr) { return SimplifyGCDOrLCM(expr, false); }

  static bool BeautifyFactor(Tree* expr);

  // ln(12/5)->2*ln(2)+ln(3)-ln(5)
  static bool ExpandLnOnRational(Tree* expr);

 private:
  struct FactorizedInteger {
    constexpr static int k_maxNumberOfFactors = 32;
    uint16_t factors[k_maxNumberOfFactors];
    uint8_t coefficients[k_maxNumberOfFactors];
    uint8_t numberOfFactors = 0;
  };

  static bool SimplifyGCDOrLCM(Tree* expr, bool isGCD);
  static FactorizedInteger PrimeFactorization(IntegerHandler m);
  static Tree* PushPrimeFactorization(IntegerHandler m);
  // ln(12)->2*ln(2)+ln(3), return nullptr if m is prime and escapeIfPrime true.
  static Tree* ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime);
};

}  // namespace PoincareJ
#endif
