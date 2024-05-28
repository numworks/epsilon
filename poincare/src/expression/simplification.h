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
  EDITION_REF_WRAP_1(SimplifyWithAdaptiveStrategy, ProjectionContext*);

  static bool ShallowSystematicReduce(Tree* u);
  EDITION_REF_WRAP(ShallowSystematicReduce);
  static bool DeepSystematicReduce(Tree* u);
  EDITION_REF_WRAP(DeepSystematicReduce);

  static bool SimplifyPower(Tree* u);
  EDITION_REF_WRAP(SimplifyPower);

  // Simplification steps
  static bool PrepareForProjection(Tree* e,
                                   ProjectionContext* projectionContext);
  static bool ToSystem(Tree* e, ProjectionContext* projectionContext);
  static bool SimplifySystem(Tree* e, bool advanced);
  static bool TryApproximationStrategyAgain(
      Tree* e, ProjectionContext projectionContext);

 private:
  /* These private methods should never be called on TreeRefs.
   * TODO: ensure it cannot. */
  static bool SimplifyAbs(Tree* u);
  static bool SimplifyPowerReal(Tree* u);
  static bool SimplifyLnReal(Tree* u);
  static bool SimplifyExp(Tree* u);
  static bool SimplifyComplexArgument(Tree* t);
  static bool SimplifyComplexPart(Tree* t);
  static bool SimplifySign(Tree* t);
  static bool SimplifyDistribution(Tree* t);
  static bool SimplifyDim(Tree* t);

  static bool BubbleUpFromChildren(Tree* u);
  static bool SimplifySwitch(Tree* u);
  static void ConvertPowerRealToPower(Tree* u);
  static bool CanApproximateTree(Tree* u, bool* changed);
};

}  // namespace Poincare::Internal

#endif
