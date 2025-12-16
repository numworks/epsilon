#pragma once

#include <poincare/properties.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

/* In nodes that introduce a variable, the variable is the first child, the
 * child expression last and bounds are in between. The variable is always
 * referred as Variable 0 in the child expression. */

namespace Parametric {

bool ReduceSumOrProduct(Tree* e);
bool ExpandSum(Tree* e);
bool ExpandProduct(Tree* e);
bool ContractSum(Tree* e);
bool ContractProduct(Tree* e);
bool Explicit(Tree* e);
bool ExpandExpOfSum(Tree* e);
bool ContractProductOfExp(Tree* e);

// Accepts layout and expressions
bool IsFunctionIndex(int i, const Tree* e);
uint8_t FunctionIndex(const Tree* e);
uint8_t FunctionIndex(TypeBlock type);

ComplexProperties VariableProperties(const Tree* e);

constexpr static ComplexProperties k_discreteVariableProperties =
    ComplexProperties::RealInteger();
/* TODO: Should instead depend on the bounds for integral and symbol value for
 * derivation */
constexpr static ComplexProperties k_continuousVariableProperties =
    ComplexProperties::Real();

/* Note: Incidentally, because 2D layout are organised the same, these indexes
 * are the same as in poincare/src/layout/indices.h. They may conflict if both
 * files are included. */
constexpr static uint8_t k_localVariableId = 0;
constexpr static uint8_t k_variableIndex = 0;

/* WARNING: For all parametric, the function/integrand node is the last child in
 * the layout tree, but is the first child when the 1D layout is built. This
 * behaviour is scattered around the codebase (mainly in the layouter and the
 * parser). */

// Sum
constexpr static uint8_t k_sumArgumentIndex = 3;
// Product
constexpr static uint8_t k_prodArgumentIndex = 3;
// Diff
constexpr static uint8_t k_derivandIndex = 3;
// Integral and IntegralWithAlternatives
constexpr static uint8_t k_integrandIndex = 3;
// ListSequence
constexpr static uint8_t k_listSequenceArgumentIndex = 2;

// Sum, Product, Integral, and IntegralWithAlternatives
constexpr static uint8_t k_lowerBoundIndex = 1;
constexpr static uint8_t k_upperBoundIndex = 2;
// Diff and ListSequence
constexpr static uint8_t k_valueIndex = 1;

// Diff
constexpr static uint8_t k_orderIndex = 2;

}  // namespace Parametric

}  // namespace Poincare::Internal
