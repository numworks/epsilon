#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare_junior/src/memory/tree.h>

#include "integer.h"

namespace PoincareJ {

class Arithmetic {
 public:
  static bool SimplifyQuotientOrRemainder(Tree* expr);
  static bool SimplifyFloor(Tree* expr);

  static bool SimplifyGCD(Tree* expr) { return SimplifyGCDOrLCM(expr, true); }
  static bool SimplifyLCM(Tree* expr) { return SimplifyGCDOrLCM(expr, false); }

  static bool BeautifyFactor(Tree* expr);

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
};

}  // namespace PoincareJ
#endif
