#include "cursor_view.h"

void CursorView::drawRect(KDRect rect) const {
  KDFillRect(rect, KDColorRed);
}
