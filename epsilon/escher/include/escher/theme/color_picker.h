#pragma once

#include <omg/unreachable.h>
#include <stdint.h>

#include "dark_theme.h"
#include "light_theme.h"

namespace Escher {

enum class Theme : uint8_t { Light, Dark };

class ColorPicker {
 public:
  constexpr static KDColor Get(Theme theme, ThemeComponent component) {
    switch (theme) {
      case Theme::Light:
        return k_lightTheme.get(component);
      case Theme::Dark:
        return k_darkTheme.get(component);
      default:
        OMG_UNREACHABLE
    }
  }
};

}  // namespace Escher
