#pragma once

#include <omg/unreachable.h>
#include <stdint.h>

#include "dark_theme.h"
#include "light_theme.h"

namespace Escher::Theme {

enum class Enum : uint8_t { Light, Dark };

class ColorPicker {
 public:
  constexpr static KDColor Get(Enum theme, Component component) {
    switch (theme) {
      case Enum::Light:
        return k_lightTheme.get(component);
      case Enum::Dark:
        return k_darkTheme.get(component);
      default:
        OMG_UNREACHABLE
    }
  }
};

}  // namespace Escher::Theme
