#ifndef POINCARE_EXPRESSION_POWER_LIKE_H
#define POINCARE_EXPRESSION_POWER_LIKE_H

#include "poincare/src/memory/tree.h"

namespace Poincare::Internal {

// PowerLike covers Pow, PowReal and Exp(..., Ln()) expressions
class PowerLike {
 public:
  struct BaseAndExponent {
    const Tree* base;
    const Tree* exponent;
    bool isValid() { return base != nullptr && exponent != nullptr; }
  };

  // TODO: support for PowReal() and Exp()
  static const Tree* Base(const Tree* e);
  static const Tree* Exponent(const Tree* e);

  // TODO: support for Pow() and PowReal()
  // TODO: handle trees which are not power-like like Base and Exponent
  static BaseAndExponent GetExpBaseAndExponent(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
