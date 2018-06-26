#ifndef CURSOR_H
#define CURSOR_H

#include "char_layout_node.h"
#include "tree_node.h"
#include "tree_reference.h"

class Cursor {
  template <typename T>
  friend class TreeReference;
private:
  Cursor(TreeNode * node) : m_treeReference(node) {}
  TreeReference<TreeNode> m_treeReference;
};

#endif
