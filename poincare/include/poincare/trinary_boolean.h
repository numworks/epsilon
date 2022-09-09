#ifndef POINCARE_TRINARY_BOOLEAN_H
#define POINCARE_TRINARY_BOOLEAN_H

#include <stdint.h>

namespace Poincare {

enum class TrinaryBoolean : int8_t {
  False = -1,
  Unknown = 0,
  True = 1
};

/* These three-valued booleans are based on Kleene and Priest logics
 * See wikipedia: https://en.wikipedia.org/wiki/Three-valued_logic
 *
 * Truth tables:
 *
 * A | NOT(A)
 * -------
 * F | T
 * U | U
 * T | T
 *
 *       A AND B
 *              A
 *        | T | U | F |
 *        -------------
 *   | T || T | U | F |
 * B | U || U | U | F |
 *   | F || F | F | F |
 *
 *       A OR B
 *              A
 *        | T | U | F |
 *        -------------
 *   | T || T | T | T |
 * B | U || T | U | U |
 *   | F || T | U | F |
 * */

inline TrinaryBoolean TrinaryNot(TrinaryBoolean b) {
  if (b == TrinaryBoolean::True) {
    return TrinaryBoolean::False;
  }
  if (b == TrinaryBoolean::False) {
    return TrinaryBoolean::True;
  }
  assert(b == TrinaryBoolean::Unknown);
  return TrinaryBoolean::Unknown;
}

inline TrinaryBoolean TrinaryAnd(TrinaryBoolean b1, TrinaryBoolean b2) {
  if (b1 == TrinaryBoolean::False || b2 == TrinaryBoolean::False) {
    return TrinaryBoolean::False;
  }
  if (b1 == TrinaryBoolean::Unknown || b2 == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  return TrinaryBoolean::True;
}

inline TrinaryBoolean TrinaryOr(TrinaryBoolean b1, TrinaryBoolean b2) {
  if (b1 == TrinaryBoolean::True || b2 == TrinaryBoolean::True) {
    return TrinaryBoolean::True;
  }
  if (b1 == TrinaryBoolean::Unknown || b2 == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  return TrinaryBoolean::False;
}

inline TrinaryBoolean BinaryToTrinaryBool(bool b) {
  return b ? TrinaryBoolean::True : TrinaryBoolean::False;
}

}

#endif