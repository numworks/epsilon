#pragma once

#include <stdint.h>

namespace Escher {

// List of all theme components
#define THEME_COMPONENTS(X) \
  X(Background)             \
  X(Graph)                  \
  X(Text)

enum class ThemeComponent : uint8_t {
#define X(name) name,
  THEME_COMPONENTS(X)
#undef X
};

}  // namespace Escher
