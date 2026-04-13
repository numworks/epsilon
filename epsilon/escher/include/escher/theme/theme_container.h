#pragma once

#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "components.h"

namespace Escher {

/* A ThemeContainer is a struct whose members are KDColors. The colors are the
 * same as in the ThemeComponent enum. It defines which color a theme uses for
 * each ThemeComponent.
 */
struct ThemeContainer {
#define X(name) KDColor name;
  THEME_COMPONENTS(X)
#undef X

  constexpr KDColor get(ThemeComponent component) const {
    switch (component) {
#define X(name)              \
  case ThemeComponent::name: \
    return name;
      THEME_COMPONENTS(X)
#undef X
      default:
        OMG_UNREACHABLE
    }
  }
};

}  // namespace Escher
