#ifndef POINCARE_JUNIOR_LAYOUT_INDICES_H
#define POINCARE_JUNIOR_LAYOUT_INDICES_H

#include <poincare_junior/src/memory/tree.h>

#include "vertical_offset.h"

namespace PoincareJ {

namespace TwoRows {
// Fraction, point2D and binomial
constexpr static int k_upperIndex = 0;
constexpr static int k_lowerIndex = 1;
}  // namespace TwoRows

namespace Fraction {
constexpr static int k_numeratorIndex = TwoRows::k_upperIndex;
constexpr static int k_denominatorIndex = TwoRows::k_lowerIndex;
}  // namespace Fraction

namespace NthRoot {
constexpr static int k_radicandIndex = 0;
constexpr static int k_indexIndex = 1;
}  // namespace NthRoot

namespace Parametric {
constexpr static int k_variableIndex = 0;
constexpr static int k_lowerBoundIndex = 1;
constexpr static int k_upperBoundIndex = 2;
constexpr static int k_argumentIndex = 3;
}  // namespace Parametric

namespace Derivative {
constexpr static int k_variableIndex = 0;
constexpr static int k_abscissaIndex = 1;
constexpr static int k_derivandIndex = 2;
constexpr static int k_orderIndex = 3;

enum class VariableSlot : bool { Fraction, Assignment };
// Denominator is first for 0 in the mask in Derivative to work out of the box
enum class OrderSlot : bool { Denominator, Numerator };

inline VariableSlot GetVariableSlot(const Tree* node) {
  return static_cast<VariableSlot>(node->nodeValueBlock(0)->getBit(0));
}

inline void SetVariableSlot(Tree* node, VariableSlot slot) {
  return node->nodeValueBlock(0)->setBit(0, static_cast<bool>(slot));
}

inline OrderSlot GetOrderSlot(const Tree* node) {
  return static_cast<OrderSlot>(node->nodeValueBlock(0)->getBit(1));
}

inline void SetOrderSlot(Tree* node, OrderSlot slot) {
  return node->nodeValueBlock(0)->setBit(1, static_cast<bool>(slot));
}
}  // namespace Derivative

namespace Integral {
constexpr static int k_differentialIndex = 0;
constexpr static int k_lowerBoundIndex = 1;
constexpr static int k_upperBoundIndex = 2;
constexpr static int k_integrandIndex = 3;
}  // namespace Integral

namespace PtCombinatorics {
constexpr static int k_nIndex = 0;
constexpr static int k_kIndex = 1;
}  // namespace PtCombinatorics

namespace Binomial {
constexpr static int k_nIndex = TwoRows::k_upperIndex;
constexpr static int k_kIndex = TwoRows::k_lowerIndex;
}  // namespace Binomial

namespace ListSequence {
constexpr static int k_functionIndex = 0;
constexpr static int k_variableIndex = 1;
constexpr static int k_upperBoundIndex = 2;
}  // namespace ListSequence

}  // namespace PoincareJ

#endif
