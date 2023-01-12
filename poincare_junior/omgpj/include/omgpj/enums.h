#ifndef UTILS_ENUMS_H
#define UTILS_ENUMS_H

#include <stdint.h>

enum class ScanDirection {
  Forward,
  Backward
};

enum class NonStrictSign : int8_t {
  Positive = 1,
  Negative = -1
};

enum class StrictSign : int8_t {
  Positive = 1,
  Null = 0,
  Negative = -1
};

inline StrictSign InvertSign(StrictSign sign) {
  return static_cast<StrictSign>(-static_cast<int8_t>(sign));
}

inline NonStrictSign InvertSign(NonStrictSign sign) {
  return static_cast<NonStrictSign>(-static_cast<int8_t>(sign));
}

#endif
