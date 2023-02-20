#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare_junior/src/memory/node.h>

namespace PoincareJ {

class Number {
 public:
  static bool IsZero(const Node n) { return n.type() == BlockType::Zero; }
};

}  // namespace PoincareJ

#endif
