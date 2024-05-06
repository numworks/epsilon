#ifndef POINCARE_EXPRESSION_UNDEFINED_H
#define POINCARE_EXPRESSION_UNDEFINED_H

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  static bool ShallowBubbleUpUndef(Tree* e);
  static bool CanHaveUndefinedChild(const Tree* e, int childIndex);
};

}  // namespace Poincare::Internal

#endif
