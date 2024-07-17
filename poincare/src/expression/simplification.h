#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

/* TODO: Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
 public:
  static bool SimplifyWithAdaptiveStrategy(
      Tree* e, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(SimplifyWithAdaptiveStrategy, ProjectionContext*);

  // Simplification steps
  static bool ProjectAndReduce(Tree* e, ProjectionContext* projectionContext,
                               bool advanced);
  static bool BeautifyReduced(Tree* e, ProjectionContext* projectionContext);
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(ToSystem, ProjectionContext*);
  static bool ReduceSystem(Tree* e, bool advanced);

 private:
  static bool HandleUnits(Tree* e, ProjectionContext* projectionContext);
  static bool TryApproximationStrategyAgain(
      Tree* e, ProjectionContext projectionContext);
};

}  // namespace Poincare::Internal

#endif
