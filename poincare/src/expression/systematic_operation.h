#ifndef POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicOperation {
  friend class SystematicReduction;

 public:
  static bool ReduceMultiplication(Tree* u);
  TREE_REF_WRAP(ReduceMultiplication);

  static bool ReduceAddition(Tree* u);
  TREE_REF_WRAP(ReduceAddition);

  static bool ReducePower(Tree* u);
  TREE_REF_WRAP(ReducePower);

 private:
  /* These private methods should never be called on TreeRefs.
   * TODO: ensure it cannot. */
  static bool ReduceAbs(Tree* u);
  static bool ReducePowerReal(Tree* u);
  static bool ReduceLnReal(Tree* u);
  static bool ReduceExp(Tree* u);
  static bool ReduceComplexArgument(Tree* t);
  static bool ReduceComplexPart(Tree* t);
  static bool ReduceSign(Tree* t);
  static bool ReduceDistribution(Tree* t);
  static bool ReduceDim(Tree* t);

  static void ConvertPowerRealToPower(Tree* u);
  static bool CanApproximateTree(Tree* u, bool* changed);
};

}  // namespace Poincare::Internal

#endif
