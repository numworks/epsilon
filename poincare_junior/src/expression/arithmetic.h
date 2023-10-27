#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare_junior/src/memory/tree.h>

#include "integer.h"

namespace PoincareJ {

class Arithmetic {
 public:
  static bool SimplifyQuotient(Tree* expr) {
    return SimplifyQuotientOrRemainder(expr);
  }
  static bool SimplifyRemainder(Tree* expr) {
    return SimplifyQuotientOrRemainder(expr);
  }
  static bool SimplifyGCD(Tree* expr);
  static bool SimplifyLCM(Tree* expr);

  static bool BeautifyFactor(Tree* expr);

 private:
  struct FactorizedInteger {
    constexpr static int k_maxNumberOfFactors = 32;
    uint16_t factors[k_maxNumberOfFactors];
    uint8_t coefficients[k_maxNumberOfFactors];
    uint8_t numberOfFactors = 0;
  };

  static bool SimplifyQuotientOrRemainder(Tree* expr);
  static FactorizedInteger PrimeFactorization(IntegerHandler m);
  static Tree* PushPrimeFactorization(IntegerHandler m);
};

}  // namespace PoincareJ
#endif
