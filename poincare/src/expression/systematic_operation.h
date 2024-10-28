#ifndef POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicOperation {
  friend class SystematicReduction;

 public:
  static bool ReducePower(Tree* e);
  TREE_REF_WRAP(ReducePower);

 private:
  /* These private methods should never be called on TreeRefs.
   * TODO: ensure it cannot. */
  static bool ReduceAbs(Tree* e);
  static bool ReduceAddOrMult(Tree* e);
  static bool ReducePowerReal(Tree* e);
  static bool ReduceExp(Tree* e);
  static bool ReduceComplexArgument(Tree* e);
  static bool ReduceComplexPart(Tree* e);
  static bool ReduceSign(Tree* e);
  static bool ReduceDistribution(Tree* e);
  static bool ReduceDim(Tree* e);

  static void ConvertPowerRealToPower(Tree* e);
  static bool CanApproximateTree(Tree* e, bool* changed);
  static bool SimplifySortedAddition(Tree* e);
  static bool SimplifySortedMultiplication(Tree* e);
};

}  // namespace Poincare::Internal

#endif
