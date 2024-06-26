#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include "projection.h"

namespace Poincare::Internal {

/* TODO: Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
 public:
  static bool SimplifyWithAdaptiveStrategy(
      Tree* node, ProjectionContext* projectionContext);
  TREE_REF_WRAP_1(SimplifyWithAdaptiveStrategy, ProjectionContext*);

  // Simplification steps
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
  static bool ReduceSystem(Tree* e, bool advanced);
  static bool TryApproximationStrategyAgain(
      Tree* e, ProjectionContext projectionContext);
};

}  // namespace Poincare::Internal

#endif
