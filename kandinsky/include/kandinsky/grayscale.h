#pragma once

#include <stdint.h>

enum class KDGrayscale : uint8_t {
  Darkest = 0,
  Dark,
  Bright,
  Brightest,
  NumberOfShades,
};
