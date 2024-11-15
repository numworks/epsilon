#include <escher/init.h>
#include <escher/text_cursor_view.h>
#include <ion/display.h>

namespace Escher {

void Init() { TextCursorView::InitSharedCursor(); }

}  // namespace Escher
