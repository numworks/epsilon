#pragma once

#include <stdint.h>

namespace Escher::Theme {

// List of all theme components
#define COMPONENTS(X) \
  X(Background)       \
  X(Graph)            \
  X(Text)

enum class Component : uint8_t {
#define X(name) name,
  COMPONENTS(X)
#undef X
};

}  // namespace Escher::Theme
