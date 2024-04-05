#ifndef POINCARE_EXPRESSION_SIMPLIFICATION_H
#define POINCARE_EXPRESSION_SIMPLIFICATION_H

#include <poincare_junior/src/memory/tree_ref.h>

#include "projection.h"

namespace PoincareJ {

/* TODO: Implement PolynomialInterpretation. Prepare the expression for
 * Polynomial interpretation (expand TranscendentalOnRationals and algebraic
 * trees.) */

class Simplification {
 public:
  static bool Simplify(Tree *node, ProjectionContext *projectionContext);
  EDITION_REF_WRAP_1(Simplify, ProjectionContext *);

  static bool ShallowSystematicReduce(Tree *u);
  EDITION_REF_WRAP(ShallowSystematicReduce);
  static bool DeepSystematicReduce(Tree *u);
  EDITION_REF_WRAP(DeepSystematicReduce);

  static bool SimplifyAddition(Tree *u);
  EDITION_REF_WRAP(SimplifyAddition);
  static bool SimplifyMultiplication(Tree *u);
  EDITION_REF_WRAP(SimplifyMultiplication);
  static bool SimplifyPower(Tree *u);
  EDITION_REF_WRAP(SimplifyPower);

 private:
  /* These private methods should never be called on TreeRefs.
   * TODO: ensure it cannot. */
  static bool SimplifyAbs(Tree *u);
  static bool SimplifyPowerReal(Tree *u);
  static bool SimplifyLnReal(Tree *u);
  static bool SimplifyExp(Tree *u);
  static bool SimplifyComplexArgument(Tree *t);
  static bool SimplifyComplexPart(Tree *t);
  static bool SimplifySign(Tree *t);
  static bool SimplifyDistribution(Tree *t);
  static bool SimplifyDim(Tree *t);

  static bool SimplifyLastTree(Tree *node,
                               ProjectionContext projectionContext = {});
  static bool SimplifySwitch(Tree *u);
  // Return true if child has been merged with next sibling.
  static bool MergeAdditionChildWithNext(Tree *child, Tree *next);
  // Return true if child has been merged with next sibling.
  static bool MergeMultiplicationChildWithNext(Tree *child);
  // Return true if child has been merged with one or more next siblings.
  static bool MergeMultiplicationChildrenFrom(Tree *child, int index,
                                              int *numberOfSiblings,
                                              bool *zero);
  /* Return true if child has been merged with siblings. Recursively merge next
   * siblings. */
  static bool SimplifyMultiplicationChildRec(Tree *child, int index,
                                             int *numberOfSiblings, bool *zero,
                                             bool *multiplicationChanged);
  // Simplify a sorted and sanitized multiplication.
  static bool SimplifySortedMultiplication(Tree *multiplication);
  static void ConvertPowerRealToPower(Tree *u);
};

}  // namespace PoincareJ

#endif
