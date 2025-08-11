#ifndef POINCARE_EXPRESSION_INFINITY_H
#define POINCARE_EXPRESSION_INFINITY_H

#include <stdint.h>

namespace Poincare::Internal {

class Tree;

class Infinity {
 public:
  static bool IsPlusOrMinusInfinity(const Tree* e);
  static bool IsMinusInfinity(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
