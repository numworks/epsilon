#pragma once

#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "components.h"

namespace Escher {

/* A ThemeContainer holds one KDColor per themed component. Fixed components are
 * not stored here: their color is resolved directly from the FIXED_COMPONENTS
 * macro and is the same across all themes. */
struct ThemeContainer {
#define X(name) KDColor name;
  THEMED_COMPONENTS(X)
#undef X

  constexpr KDColor get(ThemeComponent component) const {
    // Fixed components resolve directly, without any per-theme storage
    switch (component) {
#define X(name, color)       \
  case ThemeComponent::name: \
    return color;
      FIXED_COMPONENTS(X)
#undef X
      // Themed components are stored in this struct
#define X(name)              \
  case ThemeComponent::name: \
    return name;
      THEMED_COMPONENTS(X)
#undef X
      default:
        OMG_UNREACHABLE
    }
  }
};

}  // namespace Escher
