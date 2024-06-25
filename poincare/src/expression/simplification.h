#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare/src/memory/tree_ref.h>

#include "projection.h"

namespace Poincare::Internal {

/* TODO: Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
  friend class SystematicOperation;

 public:
  static bool SimplifyWithAdaptiveStrategy(
      Tree* node, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(SimplifyWithAdaptiveStrategy, ProjectionContext*);

  static bool ShallowSystematicReduce(Tree* u);
  TREE_REF_WRAP(ShallowSystematicReduce);
  static bool DeepSystematicReduce(Tree* u);
  TREE_REF_WRAP(DeepSystematicReduce);

  // Simplification steps
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
  static bool SimplifySystem(Tree* e, bool advanced);
  static bool TryApproximationStrategyAgain(
      Tree* e, ProjectionContext projectionContext);

 private:
  static bool BubbleUpFromChildren(Tree* u);
  static bool SimplifySwitch(Tree* u);
};

}  // namespace Poincare::Internal

#endif
