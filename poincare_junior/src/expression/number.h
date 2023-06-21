#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Number {
 public:
  static bool IsZero(const Node* n) { return n.type() == BlockType::Zero; }
  static EditionReference Addition(const Node* i, const Node* j);
  static EditionReference Multiplication(const Node* i, const Node* j);
};

}  // namespace PoincareJ

#endif
