#include <escher/init.h>
#include <escher/layout_preferences.h>
#include <escher/text_cursor_view.h>
#include <ion/display.h>

namespace Escher {

void Init(LayoutPreferencesInterface* preferences) {
  LayoutPreferences::Init(preferences);
  TextCursorView::InitSharedCursor();
}

}  // namespace Escher
