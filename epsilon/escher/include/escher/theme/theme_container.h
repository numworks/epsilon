#pragma once

#include <kandinsky/color.h>
#include <omg/unreachable.h>

#include "components.h"

namespace Escher::Theme {

/* A ThemeContainer is a struct whose members are KDColors. The colors are the
 * same as in the Component enum. It defines which color a theme uses for each
 * Component.
 */
struct ThemeContainer {
#define X(name) KDColor name;
  COMPONENTS(X)
#undef X

  constexpr KDColor get(Component component) const {
    switch (component) {
#define X(name)         \
  case Component::name: \
    return name;
      COMPONENTS(X)
#undef X
      default:
        OMG_UNREACHABLE
    }
  }
};

}  // namespace Escher::Theme
