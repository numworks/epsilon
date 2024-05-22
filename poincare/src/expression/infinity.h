#ifndef POINCARE_EXPRESSION_INFINITY_H
#define POINCARE_EXPRESSION_INFINITY_H

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Infinity {
 public:
  static bool ShallowBubbleUpInfinity(Tree* u);

  static bool TreeIsPlusOrMinusInfinity(const Tree* u);
  static bool TreeIsMinusInfinity(const Tree* u);
};

}  // namespace Poincare::Internal

#endif
