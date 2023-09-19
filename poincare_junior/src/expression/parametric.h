#ifndef POINCARE_EXPRESSION_PARAMETRIC_H
#define POINCARE_EXPRESSION_PARAMETRIC_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Parametric {
 public:
  static bool SimplifySumOrProduct(Tree* t);
  static bool ExpandSumOrProduct(Tree* t);
  static bool ContractSumOrProduct(Tree* t);
  static bool Explicit(Tree* t);

  static constexpr uint8_t k_localVariableId = 0;
  static constexpr uint8_t k_variableIndex = 0;
  static constexpr uint8_t k_lowerBoundIndex = 1;
  static constexpr uint8_t k_upperBoundIndex = 2;
  static constexpr uint8_t k_functionIndex = 3;
};

}  // namespace PoincareJ
#endif
