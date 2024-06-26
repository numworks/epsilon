#ifndef POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicReduction {
  friend class SystematicOperation;

 public:
  static bool DeepReduce(Tree* u);
  TREE_REF_WRAP(DeepReduce);
  static bool ShallowReduce(Tree* u);
  TREE_REF_WRAP(ShallowReduce);

 private:
  static bool BubbleUpFromChildren(Tree* u);
  static bool SimplifySwitch(Tree* u);
};

}  // namespace Poincare::Internal

#endif
