#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Derivation {
 public:
  // Shallow simplify a derivation Tree* inplace.
  static bool ShallowSimplify(Tree* ref);

 private:
  // Push derivand derivation on the pool.
  static void Derivate(const Tree* derivand, const Tree* symbol,
                       const Tree* symbolValue);

  EDITION_REF_WRAP(ShallowSimplify)

  // Shallow partial derivate parameterized expression at given index.
  static void ShallowPartialDerivate(const Tree* derivand, const Tree* symbol,
                                     const Tree* symbolValue, int index);
  // Clone expression replacing symbol with symbolValue.
  static Tree* CloneReplacingSymbol(const Tree* expression, const Tree* symbol,
                                    const Tree* symbolValue);
};

}  // namespace PoincareJ

#endif
