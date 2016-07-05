#include "cursor_view.h"

void CursorView::drawRect(KDRect rect) const {
  KDColor cursorColor = KDColorRed;
  KDFillRect(rect, &cursorColor, 1);
}
