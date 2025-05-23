#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Derivation {
 public:
  // Shallow simplify a derivation Tree* inplace.
  static bool Reduce(Tree* e);

  constexpr static CodePoint k_firstOrderSymbol = '\'';
  constexpr static CodePoint k_secondOrderSymbol = '"';
  /* Used in layouter/parser to preserve f'(x) form, without using
   * KUnknownSymbol which might interfere with other behaviors. */
  constexpr static KTree k_functionDerivativeVariable = KTemporaryUnknownSymbol;

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
