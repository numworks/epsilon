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

  /* Precondition: the power is a non-integer rational outside of the [0, 1]
   * range.
   * x^(p/q) is expanded to a fraction of powers of x, in which the first
   * term has an integer power and the second term has a power between 0 and 1:
   * x^(p/q) = x^n * x^(r/q) where r is the remainder of p/q. n=(p-r)/q is an
   * integer, r, and q are both positive with r/q between 0 and 1. */
  static bool ExpandRationalPower(Tree* e, const Tree* base, const Tree* power);
  static bool ExpandRationalPower(Tree* e);

  static bool ReducePowerOfZero(Tree* e, const Tree* power);
};

}  // namespace Poincare::Internal

#endif
