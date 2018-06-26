#include "layout_reference.h"
#include "layout_cursor.h"
#include "char_layout_node.h"

template <typename T>
LayoutCursor LayoutReference<T>::cursor() const {
  return LayoutCursor(this->node());
}

template LayoutCursor LayoutReference<CharLayoutNode>::cursor() const;
