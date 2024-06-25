#ifndef POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicOperation {
  friend class Simplification;

 public:
  static bool SimplifyMultiplication(Tree* u);
  TREE_REF_WRAP(SimplifyMultiplication);

  static bool SimplifyAddition(Tree* u);
  TREE_REF_WRAP(SimplifyAddition);

  static bool SimplifyPower(Tree* u);
  TREE_REF_WRAP(SimplifyPower);

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

  static void ConvertPowerRealToPower(Tree* u);
  static bool CanApproximateTree(Tree* u, bool* changed);
};

}  // namespace Poincare::Internal

#endif
