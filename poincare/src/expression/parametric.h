#pragma once

#include <poincare/properties.h>
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

  static ComplexProperties VariableProperties(const Tree* e);

  constexpr static ComplexProperties k_discreteVariableProperties =
      ComplexProperties::RealInteger();
  /* TODO: Should instead depend on the bounds for integral and symbol value for
   * derivation */
  constexpr static ComplexProperties k_continuousVariableProperties =
      ComplexProperties::Real();

  /* WARNING: For all parametric, the function/integrand node is the last child
   * in the layout tree, but is the first child when the 1D layout is built.
   * This behaviour is scattered around the codebase (mainly in the layoutter
   * and the parser). */

  constexpr static uint8_t k_localVariableId = 0;
  constexpr static uint8_t k_variableIndex = 0;

  constexpr static uint8_t k_lowerBoundIndex = 1;
  constexpr static uint8_t k_upperBoundIndex = 2;
  constexpr static uint8_t k_integrandIndex = 3;
};

}  // namespace Poincare::Internal
