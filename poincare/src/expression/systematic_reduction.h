#ifndef POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicReduction {
  friend class SystematicOperation;

 public:
  static bool DeepReduce(Tree* e);
  static bool ShallowReduce(Tree* e);

 private:
  /* [canBeList] is an optimisation, if in doubt, leave to [false].
   * See comment in .cpp for detail */
  static bool BubbleUpFromChildren(Tree* e, bool* canBeList);
  static bool Switch(Tree* e);
  static bool DeepReduceAux(Tree* e, bool* canBeList);
  static bool ShallowReduceAux(Tree* e, bool* canBeList);
};

}  // namespace Poincare::Internal

#endif
