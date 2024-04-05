#ifndef POINCARE_EXPRESSION_ALGEBRAIC_H
#define POINCARE_EXPRESSION_ALGEBRAIC_H

#include <poincare_junior/src/memory/tree_ref.h>

namespace PoincareJ {

class Algebraic final {
 public:
  static void Expand(TreeRef node);
  // develop product of sum
  // develop integer power of sum
  // -->be recursive on the results above
  // develop the integer part of power of non-integer?
  // what about subexpressions? cos(x*(x+1))? NO?
  static TreeRef Rationalize(TreeRef node);
  static TreeRef SideRelations(Tree* expression);

  static TreeRef Numerator(TreeRef expression) {
    return NormalFormator(expression, true);
  }
  static TreeRef Denominator(TreeRef expression) {
    return NormalFormator(expression, false);
  }

 private:
  static TreeRef RationalizeAddition(TreeRef expression);
  static TreeRef NormalFormator(TreeRef expression, bool numerator);
};

}  // namespace PoincareJ

#endif
