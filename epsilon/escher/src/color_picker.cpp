#include <escher/layout_preferences.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/dark_theme.h>
#include <escher/theme/light_theme.h>
#include <omg/unreachable.h>

namespace Escher {

Theme ColorPicker::GetCurrentTheme() { return SharedPreferences->theme(); }

KDColor ColorPicker::Get(Theme theme, ThemeComponent component) {
  switch (theme) {
    case Theme::Light:
      return k_lightTheme.get(component);
    case Theme::Dark:
      return k_darkTheme.get(component);
    default:
      OMG_UNREACHABLE
  }
}

}  // namespace Escher
