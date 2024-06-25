#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Trigonometry final {
 public:
  static bool IsDirect(const Tree* node);
  static bool IsInverse(const Tree* node);
  static bool SimplifyTrig(Tree* u);
  TREE_REF_WRAP(SimplifyTrig);
  static bool SimplifyTrigDiff(Tree* u);
  TREE_REF_WRAP(SimplifyTrigDiff);
  static bool SimplifyATrig(Tree* u);
  TREE_REF_WRAP(SimplifyATrig);
  static bool SimplifyArcTangentRad(Tree* u);
  TREE_REF_WRAP(SimplifyArcTangentRad);
  static bool ContractTrigonometric(Tree* node);
  TREE_REF_WRAP(ContractTrigonometric);
  static bool ExpandTrigonometric(Tree* node);
  TREE_REF_WRAP(ExpandTrigonometric);

 private:
  // Given n, return the exact expression of sin(n*π/120).
  static const Tree* ExactFormula(uint8_t n, bool isSin, bool* isOpposed);
  static bool SimplifyTrigSecondElement(Tree* u, bool* isOpposed);
  TREE_REF_WRAP_1(SimplifyTrigSecondElement, bool*);
};

}  // namespace Poincare::Internal

#endif
