#ifndef POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_REDUCTION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicReduction {
  friend class SystematicOperation;

 public:
  static bool DeepSystematicReduce(Tree* u);
  TREE_REF_WRAP(DeepSystematicReduce);
  static bool ShallowSystematicReduce(Tree* u);
  TREE_REF_WRAP(ShallowSystematicReduce);

 private:
  static bool BubbleUpFromChildren(Tree* u);
  static bool SimplifySwitch(Tree* u);
};

}  // namespace Poincare::Internal

#endif
