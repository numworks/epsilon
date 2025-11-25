#pragma once

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

Sign GetSign(const Tree* e);
Properties GetProperties(const Tree* e);
ComplexProperties GetComplexProperties(const Tree* e);

/* Properties of e1 - e2 so that e1 < e2 <=> ComplexPropertiesOfDifference(e1,
 * e2) < 0 and so on. Beware that the difference may be real while the trees
 * were complexes. */
ComplexProperties ComplexPropertiesOfDifference(const Tree* e1, const Tree* e2);

// TODO : Sign could be used instead of NonStrictSign and StrictSign

enum class NonStrictSign : int8_t { Positive = 1, Negative = -1 };

enum class StrictSign : int8_t { Positive = 1, Null = 0, Negative = -1 };

inline StrictSign InvertSign(StrictSign sign) {
  return static_cast<StrictSign>(-static_cast<int8_t>(sign));
}

inline NonStrictSign InvertSign(NonStrictSign sign) {
  return static_cast<NonStrictSign>(-static_cast<int8_t>(sign));
}

/* Return the Tree properties if they are known, otherwise return the properties
 * of the approximated value */
ComplexProperties PropertiesOfTreeOrApproximation(const Tree* e);

}  // namespace Poincare::Internal
