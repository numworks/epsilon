#ifndef POINCARE_EXPRESSION_SIGN_H
#define POINCARE_EXPRESSION_SIGN_H

#include <omgpj/enums.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

namespace Sign {

enum Sign : uint8_t {
  // Organized as a bit field with positive bit = 1 => may be positive...
  Null = 0b001,
  Positive = 0b010,
  Negative = 0b100,
  PositiveOrNull = Positive | Null,
  NegativeOrNull = Negative | Null,
  Unknown = Positive | Negative | Null,
};

inline bool MayBePos(Sign s) { return s & Positive; }
inline bool MayBeNeg(Sign s) { return s & Negative; }
inline bool MayBeNull(Sign s) { return s & Null; }

inline Sign Make(bool mayBeNull, bool mayBePos, bool mayBeNeg) {
  return static_cast<Sign>(mayBeNeg << 2 | mayBePos << 1 | mayBeNull);
}

Sign Add(Sign s1, Sign s2);
Sign Mult(Sign s1, Sign s2);

Sign GetSign(const Tree* t);

}  // namespace Sign
}  // namespace PoincareJ

#endif
