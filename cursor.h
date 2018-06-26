#ifndef CURSOR_H
#define CURSOR_H

#include "tree_node.h"
#include "tree_reference.h"

class Cursor {
  template <typename T>
  friend class TreeReference;
public:
  bool isDefined() const { return m_treeReference.isDefined(); }
protected:
  Cursor(TreeNode * node) : m_treeReference(node) {}
  TreeReference<TreeNode> m_treeReference;
};

#endif
