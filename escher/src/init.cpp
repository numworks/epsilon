#include <escher/init.h>
#include <escher/text_cursor_view.h>
#include <kandinsky/ion_context.h>

namespace Escher {

void Init() {
  KDIonContext::SharedContext.init();
  TextCursorView::InitSharedCursor();
}

}  // namespace Escher
