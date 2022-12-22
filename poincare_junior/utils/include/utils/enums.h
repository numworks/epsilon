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
#endif
