#include "tree_reference.h"
#include "char_layout_node.h"
#include "cursor.h"

template <typename T>
Cursor TreeReference<T>::treeCursor() {
  return Cursor(node());
}

template Cursor TreeReference<TreeNode>::treeCursor();
template Cursor TreeReference<CharLayoutNode>::treeCursor();
