#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

ComplexSign GetComplexSign(const Tree* e);

Sign GetSign(const Tree* e);

/* Sign of e1 - e2 so that e1 < e2 <=> SignOfDifference(e1, e2) < 0 and so on.
 * Beware that the difference may be real while the trees were complexes. */
ComplexSign ComplexSignOfDifference(const Tree* e1, const Tree* e2);

// TODO : Sign could be used instead of NonStrictSign and StrictSign

enum class NonStrictSign : int8_t { Positive = 1, Negative = -1 };

enum class StrictSign : int8_t { Positive = 1, Null = 0, Negative = -1 };

inline StrictSign InvertSign(StrictSign sign) {
  return static_cast<StrictSign>(-static_cast<int8_t>(sign));
}

inline NonStrictSign InvertSign(NonStrictSign sign) {
  return static_cast<NonStrictSign>(-static_cast<int8_t>(sign));
}

/* Return the Tree sign if it is known, otherwise return the sign of the
 * approximated value */
ComplexSign SignOfTreeOrApproximation(const Tree* e);

}  // namespace Poincare::Internal

#endif
