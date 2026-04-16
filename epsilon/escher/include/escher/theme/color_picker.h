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
  /* Two Resolve overloads so that Resolve is callable on both raw colors and
   * theme components */
  inline static KDColor Resolve(KDColor rawColor) { return rawColor; }
  inline static KDColor Resolve(ThemeComponent component) {
    return Get(GetCurrentTheme(), component);
  };

 private:
  /* GetCurrentTheme is defined in the .cpp so that the details of how the
   * theme is accessed are hidden. Currently for Epsilon (the only project which
   * uses Escher) the current theme is accessed by reading the theme member of
   * the shared GlobalPreferences instance (which should be a very quick
   * accessor) */
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
