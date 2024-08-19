#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class Trigonometry final {
 public:
  static bool ReduceTrig(Tree* e);
  TREE_REF_WRAP(ReduceTrig);
  static bool ReduceTrigDiff(Tree* e);
  TREE_REF_WRAP(ReduceTrigDiff);
  static bool ReduceATrig(Tree* e);
  TREE_REF_WRAP(ReduceATrig);
  static bool ReduceArcTangentRad(Tree* e);
  TREE_REF_WRAP(ReduceArcTangentRad);
  static bool ContractTrigonometric(Tree* e);
  TREE_REF_WRAP(ContractTrigonometric);
  static bool ExpandTrigonometric(Tree* e);
  TREE_REF_WRAP(ExpandTrigonometric);

  static Type GetInverseType(Type type);

 private:
  // Given n, return the exact expression of sin(n*π/120).
  static const Tree* ExactFormula(uint8_t n, bool isSin, bool* isOpposed);
  static bool ReduceTrigSecondElement(Tree* e, bool* isOpposed);
  TREE_REF_WRAP_1(ReduceTrigSecondElement, bool*);
};

}  // namespace Poincare::Internal

#endif
