#ifndef POINCARE_EXPRESSION_PARAMETRIC_H
#define POINCARE_EXPRESSION_PARAMETRIC_H

#include <poincare/src/memory/tree.h>

#include "sign.h"

namespace Poincare::Internal {

/* In nodes that introduce a variable, the variable is the first child, the
 * child expression last and bounds are in between. The variable is always
 * refered as Variable 0 in the child expression. */

class Parametric {
 public:
  static bool SimplifySumOrProduct(Tree* t);
  static bool ExpandSum(Tree* t);
  static bool ExpandProduct(Tree* t);
  static bool ContractSum(Tree* t);
  static bool ContractProduct(Tree* t);
  static bool Explicit(Tree* t);
  static bool HasLocalRandom(const Tree* t);
  static bool ExpandExpOfSum(Tree* t);
  static bool ContractProductOfExp(Tree* t);

  // Accepts layout and expressions
  static bool IsFunctionIndex(int i, const Tree* t);
  static uint8_t FunctionIndex(const Tree* t);
  static uint8_t FunctionIndex(TypeBlock type);

  static ComplexSign VariableSign(const Tree* t);

  constexpr static ComplexSign k_discreteVariableSign =
      ComplexSign::RealInteger();
  /* TODO: Should instead depend on the bounds for integral and symbol value for
   * derivation */
  constexpr static ComplexSign k_continuousVariableSign =
      ComplexSign::RealUnknown();

  static constexpr uint8_t k_localVariableId = 0;
  static constexpr uint8_t k_variableIndex = 0;

  static constexpr uint8_t k_lowerBoundIndex = 1;
  static constexpr uint8_t k_upperBoundIndex = 2;
  static constexpr uint8_t k_integrandIndex = 3;

  static constexpr uint8_t k_derivandIndex = 2;
};

}  // namespace Poincare::Internal
#endif
