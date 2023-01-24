#ifndef POINCARE_MEMORY_PLACEHOLDER_H
#define POINCARE_MEMORY_PLACEHOLDER_H

#include <stdint.h>

namespace PoincareJ {

enum class Placeholder : uint8_t {
  A,
  B,
  C,
};

constexpr int k_numberOfPlaceholder = 3;

}

#endif
