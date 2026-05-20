#include <escher/layout_preferences.h>
#include <escher/palette.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/dark_theme.h>
#include <escher/theme/light_theme.h>
#include <omg/unreachable.h>

namespace Escher {

KDColor ColorPicker::ResolveCustomColor(KDColor color) {
  switch (GetCurrentTheme()) {
    case Theme::Light:
      return color;
    case Theme::Dark:
      return Palette::Black;
    default:
      OMG_UNREACHABLE
  }
}

KDColor ColorPicker::ResolveCustomColorForHighlight(KDColor color) {
  switch (GetCurrentTheme()) {
    case Theme::Light:
      return color;
    case Theme::Dark:
      return Palette::DARK_VioletPale;
    default:
      OMG_UNREACHABLE
  }
}

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

ColorPicker::BlendLevel ColorPicker::ThemeBlendLevel(Theme theme) {
  switch (theme) {
    case Theme::Light:
      return BlendLevel::High;
    case Theme::Dark:
      return BlendLevel::Low;
    default:
      OMG_UNREACHABLE
  }
}

}  // namespace Escher
