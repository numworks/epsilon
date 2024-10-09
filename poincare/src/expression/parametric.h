#ifndef POINCARE_EXPRESSION_PARAMETRIC_H
#define POINCARE_EXPRESSION_PARAMETRIC_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

/* In nodes that introduce a variable, the variable is the first child, the
 * child expression last and bounds are in between. The variable is always
 * refered as Variable 0 in the child expression. */

class Parametric {
 public:
  static bool ReduceSumOrProduct(Tree* e);
  static bool ExpandSum(Tree* e);
  static bool ExpandProduct(Tree* e);
  static bool ContractSum(Tree* e);
  static bool ContractProduct(Tree* e);
  static bool Explicit(Tree* e);
  static bool ExpandExpOfSum(Tree* e);
  static bool ContractProductOfExp(Tree* e);

  // Accepts layout and expressions
  static bool IsFunctionIndex(int i, const Tree* e);
  static uint8_t FunctionIndex(const Tree* e);
  static uint8_t FunctionIndex(TypeBlock type);

  static ComplexSign VariableSign(const Tree* e);

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
