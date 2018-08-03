#ifndef POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H

#include "ghost_layout_node.h"

namespace Poincare {

class AllocationFailedLayoutNode : public GhostLayoutNode {
public:
  using GhostLayoutNode::GhostLayoutNode;
  // TreeNode
  bool isAllocationFailure() const override { return true; }
  size_t size() const override { return sizeof(AllocationFailedLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailedLayout";  }
#endif
};

class AllocationFailedLayoutRef : public LayoutReference {
public:
  AllocationFailedLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<AllocationFailedLayoutNode>();
    m_identifier = node->identifier();
  }
  AllocationFailedLayoutRef(TreeNode * n) : LayoutReference(n) {}
};

}

#endif
