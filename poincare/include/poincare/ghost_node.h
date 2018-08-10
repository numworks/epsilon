#ifndef POINCARE_GHOST_NODE_H
#define POINCARE_GHOST_NODE_H

#include "tree_node.h"
#include "tree_pool.h"
#include "tree_by_reference.h"

namespace Poincare {

class GhostNode : public TreeNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 0; }
  size_t size() const override { return sizeof(GhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const {
    stream << "Ghost";
  }
#endif

  // Ghost
  bool isGhost() const override { return true; }
  // Allocation Failure
  static GhostNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
};

class GhostReference : public TreeByReference {
public:
  GhostReference() : TreeByReference(TreePool::sharedPool()->createTreeNode<GhostNode>()) {}
};

class AllocationFailedGhostNode : public GhostNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedGhostNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailedGhost";  }
#endif
  bool isAllocationFailure() const override { return true; }
};

}

#endif
