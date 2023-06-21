#ifndef POINCARE_EXPRESSION_ALGEBRAIC_H
#define POINCARE_EXPRESSION_ALGEBRAIC_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Algebraic final {
 public:
  static void Expand(EditionReference node);
  // develop product of sum
  // develop integer power of sum
  // -->be recursive on the results above
  // develop the integer part of power of non-integer?
  // what about subexpressions? cos(x*(x+1))? NO?
  static EditionReference Rationalize(EditionReference node);
  static EditionReference SideRelations(Node* expression);

  static EditionReference Numerator(EditionReference expression) {
    return NormalFormator(expression, true);
  }
  static EditionReference Denominator(EditionReference expression) {
    return NormalFormator(expression, false);
  }

 private:
  static EditionReference RationalizeAddition(EditionReference expression);
  static EditionReference NormalFormator(EditionReference expression,
                                         bool numerator);
};

}  // namespace PoincareJ

#endif
