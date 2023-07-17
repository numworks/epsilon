#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Number {
 public:
  static bool IsZero(const Tree* n) { return n->type() == BlockType::Zero; }
  static EditionReference Addition(const Tree* i, const Tree* j);
  static EditionReference Multiplication(const Tree* i, const Tree* j);
  static NonStrictSign Sign(const Tree* node);
  static StrictSign StrictSign(const Tree* node);
};

}  // namespace PoincareJ

#endif
