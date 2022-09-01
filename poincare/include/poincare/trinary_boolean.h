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
 * A | B | A AND B
 * -----------
 * F | F |  F
 * F | U |  F
 * F | T |  F
 * U | U |  U
 * U | T |  U
 * T | T |  T
 *
 * A | B | A OR B
 * -----------
 * T | T |  T
 * T | U |  T
 * T | F |  T
 * U | U |  U
 * U | F |  U
 * F | F |  F
 * */

inline TrinaryBoolean TrinaryNot(TrinaryBoolean b) {
  return static_cast<TrinaryBoolean>(static_cast<int8_t>(b) * (-1));
}

inline TrinaryBoolean TrinaryAnd(TrinaryBoolean b1, TrinaryBoolean b2) {
  return (b1 == TrinaryBoolean::False || b2 == TrinaryBoolean::False) ? TrinaryBoolean::False : ((b1 == TrinaryBoolean::Unknown || b2 == TrinaryBoolean::Unknown) ? TrinaryBoolean::Unknown : TrinaryBoolean::True);
}

inline TrinaryBoolean TrinaryOr(TrinaryBoolean b1, TrinaryBoolean b2) {
  return (b1 == TrinaryBoolean::True || b2 == TrinaryBoolean::True) ? TrinaryBoolean::True : ((b1 == TrinaryBoolean::Unknown || b2 == TrinaryBoolean::Unknown) ? TrinaryBoolean::Unknown : TrinaryBoolean::False);
}

inline TrinaryBoolean BinaryToTrinaryBool(bool b) {
  return b ? TrinaryBoolean::True : TrinaryBoolean::False;
}

}

#endif