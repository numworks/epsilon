#ifndef POINCARE_EXPRESSION_PARAMETRIC_H
#define POINCARE_EXPRESSION_PARAMETRIC_H

#include <poincare_junior/src/memory/tree.h>

#include "sign.h"

namespace PoincareJ {

/* In nodes that introduce a variable, the variable is the first child, the
 * child expression last and bounds are in between. The variable is always
 * refered as Variable 0 in the child expression. */

class Parametric {
 public:
  static bool SimplifySumOrProduct(Tree* t);
  static bool ExpandSum(Tree* t);
  static bool ExpandProduct(Tree* t);
  static bool ContractProduct(Tree* t);
  static bool Explicit(Tree* t);
  static bool HasLocalRandom(Tree* t);

  static uint8_t FunctionIndex(const Tree* t);
  static Sign::Sign VariableSign(const Tree* t);

  constexpr static Sign::Sign k_discreteVariableSign = Sign::Integer;
  // TODO: [COMPLEXSIGN] Distinguish real and complex unknown
  constexpr static Sign::Sign k_continuousVariableSign = Sign::Unknown;

  static constexpr uint8_t k_localVariableId = 0;
  static constexpr uint8_t k_variableIndex = 0;

  static constexpr uint8_t k_lowerBoundIndex = 1;
  static constexpr uint8_t k_upperBoundIndex = 2;
  static constexpr uint8_t k_integrandIndex = 3;

  static constexpr uint8_t k_derivandIndex = 2;
};

}  // namespace PoincareJ
#endif
