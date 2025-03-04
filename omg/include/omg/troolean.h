#ifndef OMG_TROOLEAN_H
#define OMG_TROOLEAN_H

#include <assert.h>
#include <stdint.h>

namespace OMG {

enum class Troolean : int8_t { False = -1, Unknown = 0, True = 1 };

/* These three-valued booleans are based on Kleene and Priest logics
 * See wikipedia: https://en.wikipedia.org/wiki/Three-valued_logic
 *
 * Truth tables:
 *
 * A | NOT(A)
 * -------
 * F | T
 * U | U
 * T | F
 *
 *       A AND B
 *              A
 *        | T | U | F |
 *        -------------
 *   | T || T | U | F |
 * B | U || U | U | F |
 *   | F || F | F | F |
 *
 *       A OR B
 *              A
 *        | T | U | F |
 *        -------------
 *   | T || T | T | T |
 * B | U || T | U | U |
 *   | F || T | U | F |
 * */

inline Troolean TrooleanNot(Troolean b) {
  if (b == Troolean::True) {
    return Troolean::False;
  }
  if (b == Troolean::False) {
    return Troolean::True;
  }
  assert(b == Troolean::Unknown);
  return Troolean::Unknown;
}

inline Troolean TrooleanAnd(Troolean b1, Troolean b2) {
  if (b1 == Troolean::False || b2 == Troolean::False) {
    return Troolean::False;
  }
  if (b1 == Troolean::Unknown || b2 == Troolean::Unknown) {
    return Troolean::Unknown;
  }
  return Troolean::True;
}

inline Troolean TrooleanOr(Troolean b1, Troolean b2) {
  if (b1 == Troolean::True || b2 == Troolean::True) {
    return Troolean::True;
  }
  if (b1 == Troolean::Unknown || b2 == Troolean::Unknown) {
    return Troolean::Unknown;
  }
  return Troolean::False;
}

inline Troolean BoolToTroolean(bool b) {
  return b ? Troolean::True : Troolean::False;
}

inline bool SafeTrooleanToBool(Troolean b) { return b == Troolean::True; }

inline bool TrooleanToBool(Troolean b) {
  assert(b != Troolean::Unknown);
  return SafeTrooleanToBool(b);
}

}  // namespace OMG

#endif
