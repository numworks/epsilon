#pragma once

#include <omg/unreachable.h>
#include <stdint.h>

#include "dark_theme.h"
#include "light_theme.h"

namespace Escher {

enum class Theme : uint8_t { Light, Dark };

constexpr static Theme k_defaultTheme = Theme::Light;

class ColorPicker {
 public:
  inline static KDColor Resolve(ThemeComponent component) {
    return Get(GetCurrentTheme(), component);
  };

 private:
  /* GetCurrentTheme is defined in the .cpp so that the details of how the
   * theme is accessed are hidden. Currently for Epsilon (the only project which
   * uses Escher) the current theme is accessed by calling the virtual theme()
   * getter of LayoutPreferences, which is itself defined in the
   * GlobalPreferences concrete class. Accessing the GlobalPreferences theme
   * member should be quick. */
  static Theme GetCurrentTheme();

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
