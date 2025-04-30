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
  /* [hasList] is an optim, if in doubt, leave to [false] */
  static bool BubbleUpFromChildren(Tree* e, bool* hasList);
  static bool Switch(Tree* e);
  static bool DeepReduceAux(Tree* e, bool* hasList);
  static bool ShallowReduceAux(Tree* e, bool* hasList);
};

}  // namespace Poincare::Internal

#endif
