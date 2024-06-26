#ifndef POINCARE_EXPRESSION_ALGEBRAIC_H
#define POINCARE_EXPRESSION_ALGEBRAIC_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Algebraic final {
 public:
  static void Expand(TreeRef node);
  // develop product of sum
  // develop integer power of sum
  // -->be recursive on the results above
  // develop the integer part of power of non-integer?
  // what about subexpressions? cos(x*(x+1))? NO?
  static TreeRef Rationalize(TreeRef node);
  static TreeRef SideRelations(Tree* e);

  static TreeRef Numerator(TreeRef e) { return NormalFormator(e, true); }
  static TreeRef Denominator(TreeRef e) { return NormalFormator(e, false); }

 private:
  static TreeRef RationalizeAddition(TreeRef e);
  static TreeRef NormalFormator(TreeRef e, bool numerator);
};

}  // namespace Poincare::Internal

#endif
