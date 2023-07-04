#ifndef POINCARE_EXPRESSION_DERIVATION_H
#define POINCARE_EXPRESSION_DERIVATION_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Derivation {
 public:
  // Reduce a derivation Node* inplace.
  static bool Reduce(EditionReference& ref);
  // Push derivand derivation on the pool.
  static void Derivate(const Node* derivand, const Node* symbol,
                       const Node* symbolValue);

 private:
  // Shallow partial derivate parameterized expression at given index.
  static void ShallowPartialDerivate(const Node* derivand, const Node* symbol,
                                     const Node* symbolValue, int index);
  // Clone expression replacing symbol with symbolValue.
  static Node* CloneReplacingSymbol(const Node* expression, const Node* symbol,
                                    const Node* symbolValue);
  static void CloneReplacingSymbolRec(const Node* expression,
                                      const Node* symbol,
                                      const Node* symbolValue);
};

}  // namespace PoincareJ

#endif
