
#include <escher/layout_preferences.h>

namespace Escher {

const LayoutPreferences SharedPreferences;
const LayoutPreferences::Interface* LayoutPreferences::s_preferences = nullptr;

}  // namespace Escher
