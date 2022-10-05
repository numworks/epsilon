#include <poincare/layout_cursor.h>
#include <poincare/code_point_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/nth_root_layout.h>

namespace Poincare {

void LayoutCursor::testAddLayoutAndMoveCursor(Context * context) {
  Layout l = HorizontalLayout::Builder(CodePointLayout::Builder('a'), NthRootLayout::Builder(EmptyLayout::Builder()));
  addLayoutAndMoveCursor(l, context, true);
}

}