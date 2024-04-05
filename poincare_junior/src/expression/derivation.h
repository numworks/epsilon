#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare_junior/src/memory/tree_ref.h>

namespace PoincareJ {

class Derivation {
 public:
  // Shallow simplify a derivation Tree* inplace.
  static bool ShallowSimplify(Tree* ref);

 private:
  // Push derivand derivation on the pool.
  static Tree* Derivate(const Tree* derivand, const Tree* symbolValue,
                        const Tree* symbol);

  EDITION_REF_WRAP(ShallowSimplify)

  /* Push shallow partial derivate parameterized expression at given index.
   * If unhandled, push nothing and return false. */
  static bool ShallowPartialDerivate(const Tree* derivand,
                                     const Tree* symbolValue, int index);
  // Clone expression replacing symbol with symbolValue.
  static Tree* CloneReplacingSymbol(const Tree* expression,
                                    const Tree* symbolValue,
                                    bool simplify = true);
};

}  // namespace PoincareJ

#endif
