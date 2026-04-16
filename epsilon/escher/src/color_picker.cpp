#include <apps/global_preferences.h>
#include <escher/theme/color_picker.h>

namespace Escher {

Theme ColorPicker::GetCurrentTheme() { return SharedGlobalPrefs().theme(); }

}  // namespace Escher
