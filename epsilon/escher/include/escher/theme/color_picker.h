#pragma once

#include <kandinsky/color.h>
#include <stdint.h>

#include "components.h"

namespace Escher {

enum class Theme : uint8_t { Light, Dark };

constexpr static Theme k_defaultTheme = Theme::Light;

class ColorPicker {
 public:
  inline static KDColor Resolve(ThemeComponent component) {
    return Get(GetCurrentTheme(), component);
  };

  /* Some custom colors will resolve differently depending on theme.
   * TODO: Use ThemeComponent instead */
  static KDColor ResolveCustomColor(KDColor color);
  static KDColor ResolveCustomColorForHighlight(KDColor color);
  static KDColor ResolveCustomColorForText(KDColor color);

  // Depending on the theme, blending should be more or less intense
  enum class BlendLevel : bool { High, Low };
  static BlendLevel GetBlendLevel();

  /* The thickness of some selection borders (for example in Elements) depends
   * on the theme */
  enum class SelectionThickness : bool { Simple, Double };
  static SelectionThickness GetSelectionThickness();

 private:
  /* GetCurrentTheme is defined in the .cpp so that the details of how the
   * theme is accessed are hidden. Currently for Epsilon (the only project which
   * uses Escher) the current theme is accessed by calling the virtual theme()
   * getter of LayoutPreferences, which is itself defined in the
   * GlobalPreferences concrete class. Accessing the GlobalPreferences theme
   * member should be quick. */
  static Theme GetCurrentTheme();

  static KDColor Get(Theme theme, ThemeComponent component);
};

}  // namespace Escher
