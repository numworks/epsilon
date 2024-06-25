#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Derivation {
 public:
  // Shallow simplify a derivation Tree* inplace.
  static bool ShallowSimplify(Tree* e);
  TREE_REF_WRAP(ShallowSimplify)

 private:
  /* Push derivand derivation on the pool. If force is true, push a diff tree
   * anyway when we can't derivate. */
  static Tree* Derive(const Tree* derivand, const Tree* symbol, bool force);

  /* Push shallow partial derivate parameterized expression at given index.
   * If unhandled, push nothing and return nullptr. */
  static Tree* ShallowPartialDerivate(const Tree* derivand, int index);
};

}  // namespace Poincare::Internal

#endif
