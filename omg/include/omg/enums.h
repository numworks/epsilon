#ifndef OMG_ENUMS_H
#define OMG_ENUMS_H

#include <stdint.h>

namespace OMG {

enum class Base : uint8_t { Binary = 2, Decimal = 10, Hexadecimal = 16 };

enum class NonStrictSign : int8_t { Positive = 1, Negative = -1 };

enum class StrictSign : int8_t { Positive = 1, Null = 0, Negative = -1 };

inline StrictSign InvertSign(StrictSign sign) {
  return static_cast<StrictSign>(-static_cast<int8_t>(sign));
}

inline NonStrictSign InvertSign(NonStrictSign sign) {
  return static_cast<NonStrictSign>(-static_cast<int8_t>(sign));
}

}  // namespace OMG

#endif
