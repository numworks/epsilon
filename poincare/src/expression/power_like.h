#ifndef POINCARE_EXPRESSION_POWER_LIKE_H
#define POINCARE_EXPRESSION_POWER_LIKE_H

#include "poincare/src/memory/tree.h"

namespace Poincare::Internal {

// PowerLike covers Pow, PowReal and Exp(..., Ln()) expressions
class PowerLike {
 public:
  struct BaseAndExponent {
    const Tree* base = nullptr;
    const Tree* exponent = nullptr;
    bool isValid() { return base != nullptr && exponent != nullptr; }
  };

  static const Tree* Base(const Tree* e);
  static const Tree* Exponent(const Tree* e);
  static BaseAndExponent GetBaseAndExponent(const Tree* e);
};

}  // namespace Poincare::Internal

#endif
