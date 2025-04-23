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
#if ASSERTIONS
    bool isValid() { return base != nullptr && exponent != nullptr; }
#endif
  };

  /* For expressions which are not power-like, or for PowReal expressions when
   * ignorePowReal is true, the base is the expression itself and the exponent
   * is 1 */
  static const Tree* Base(const Tree* e, bool ignorePowReal);
  static const Tree* Exponent(const Tree* e, bool ignorePowReal);
  static BaseAndExponent GetBaseAndExponent(const Tree* e, bool ignorePowReal);
};

}  // namespace Poincare::Internal

#endif
