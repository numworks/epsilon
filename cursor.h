#ifndef CURSOR_H
#define CURSOR_H

#include "tree_node.h"
#include "tree_reference.h"

class Cursor {
  template <typename T>
  friend class TreeReference;
public:
  TreeReference<TreeNode> treeReference() { return m_treeReference; }
  int treeReferenceIdentifier() { return m_treeReference.identifier(); }
  void setTreeReference(TreeReference<TreeNode>  t) {
    m_treeReference = t;
  }
  void setTreeNode(TreeNode * t) {
    m_treeReference = TreeReference<TreeNode>(t);
  }
  bool isDefined() const { return m_treeReference.isDefined(); }
protected:
  Cursor(TreeNode * node) : m_treeReference(node) {}
  TreeReference<TreeNode> m_treeReference;
};

#endif
