#include <escher/layout_preferences.h>
#include <escher/theme/color_picker.h>

namespace Escher {

Theme ColorPicker::GetCurrentTheme() { return SharedPreferences->theme(); }

}  // namespace Escher
