#ifndef POINCARE_JUNIOR_LAYOUT_INDICES_H
#define POINCARE_JUNIOR_LAYOUT_INDICES_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

namespace Fraction {
constexpr static int NumeratorIndex = 0;
constexpr static int DenominatorIndex = 1;
}  // namespace Fraction

namespace NthRoot {
constexpr static int RadicandIndex = 0;
constexpr static int IndexIndex = 1;
}  // namespace NthRoot

namespace Parametric {
constexpr static int VariableIndex = 0;
constexpr static int LowerBoundIndex = 1;
constexpr static int UpperBoundIndex = 2;
constexpr static int ArgumentIndex = 3;
}  // namespace Parametric

namespace Derivative {
constexpr static int VariableIndex = 0;
constexpr static int AbscissaIndex = 1;
constexpr static int DerivandIndex = 2;
constexpr static int OrderIndex = 3;

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

namespace VerticalOffset {
inline bool IsSuperscript(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return !node->nodeValueBlock(0)->getBit(0);
}

inline bool IsSuffix(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return !node->nodeValueBlock(0)->getBit(1);
}

inline void SetSuperscript(Tree* node, bool superscript) {
  assert(node->isVerticalOffsetLayout());
  node->nodeValueBlock(0)->setBit(0, !superscript);
}

inline void SetSuffix(Tree* node, bool suffix) {
  assert(node->isVerticalOffsetLayout());
  node->nodeValueBlock(0)->setBit(1, !suffix);
}

inline bool IsSuffixSuperscript(const Tree* node) {
  assert(node->isVerticalOffsetLayout());
  return IsSuffix(node) && IsSuperscript(node);
}
}  // namespace VerticalOffset

namespace Integral {
constexpr static int DifferentialIndex = 0;
constexpr static int LowerBoundIndex = 1;
constexpr static int UpperBoundIndex = 2;
constexpr static int IntegrandIndex = 3;
}  // namespace Integral

namespace PtCombinatorics {
constexpr static int nIndex = 0;
constexpr static int kIndex = 1;
}  // namespace PtCombinatorics

namespace Binomial {
constexpr static int nIndex = 0;
constexpr static int kIndex = 1;
}  // namespace Binomial

namespace ListSequence {
constexpr static int FunctionIndex = 0;
constexpr static int VariableIndex = 1;
constexpr static int UpperBoundIndex = 2;
}  // namespace ListSequence

}  // namespace PoincareJ

#endif
