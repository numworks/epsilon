#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Derivation {
 public:
  // Reduce a derivation Tree* inplace.
  static bool Reduce(Tree* ref);
  // Push derivand derivation on the pool.
  static void Derivate(const Tree* derivand, const Tree* symbol,
                       const Tree* symbolValue);

 private:
  // Shallow partial derivate parameterized expression at given index.
  static void ShallowPartialDerivate(const Tree* derivand, const Tree* symbol,
                                     const Tree* symbolValue, int index);
  // Clone expression replacing symbol with symbolValue.
  static Tree* CloneReplacingSymbol(const Tree* expression, const Tree* symbol,
                                    const Tree* symbolValue);
  static void CloneReplacingSymbolRec(const Tree* expression,
                                      const Tree* symbol,
                                      const Tree* symbolValue);
};

}  // namespace PoincareJ

#endif
