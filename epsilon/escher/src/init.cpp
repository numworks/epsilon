#include <escher/init.h>
#include <escher/layout_preferences.h>
#include <escher/text_cursor_view.h>
#include <ion/display.h>

namespace Escher {

void Init(const LayoutPreferences::Interface* preferences) {
  assert(preferences);
  SharedPreferences = preferences;
  TextCursorView::InitSharedCursor();
}

}  // namespace Escher
