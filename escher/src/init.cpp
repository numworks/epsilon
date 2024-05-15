#include <escher/init.h>
#include <escher/text_cursor_view.h>
#include <ion/display.h>

namespace Escher {

void Init() {
  Ion::Display::Context::SharedContext.init();
  TextCursorView::InitSharedCursor();
}

}  // namespace Escher
