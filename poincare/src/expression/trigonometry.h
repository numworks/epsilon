#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

class Trigonometry final {
 public:
  static bool IsDirect(const Tree* e);
  static bool IsInverse(const Tree* e);
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

  /* Detect if expression is of the form a·cos(b·x+c) + k
   * with c between 0 and 2π
   * k can be non-null only if acceptAddition = true
   * TODO: factorize with IsLinearCombinationOfFunction? */
  static bool DetectLinearPatternOfTrig(const Tree* e,
                                        ProjectionContext projectionContext,
                                        const char* symbol, double* a,
                                        double* b, double* c,
                                        bool acceptConstantTerm);

 private:
  // Given n, return the exact expression of sin(n*π/120).
  static const Tree* ExactFormula(uint8_t n, bool isSin, bool* isOpposed);
  static bool ReduceTrigSecondElement(Tree* e, bool* isOpposed);
  TREE_REF_WRAP_1(ReduceTrigSecondElement, bool*);
};

}  // namespace Poincare::Internal

#endif
