#ifndef POINCARE_GHOST_NODE_H
#define POINCARE_GHOST_NODE_H

#include "tree_node.h"

namespace Poincare {

class GhostNode : public TreeNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 0; }
  size_t size() const override { return sizeof(GhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Ghost";
  }
#endif

  // Ghost
  bool isGhost() const override { return true; }
  // Allocation Failure
  static GhostNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  // Uninitialized
  TreeNode * uninitializedStaticNode() const override { assert(false); return nullptr; }
};

class AllocationFailedGhostNode : public GhostNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedGhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AllocationFailedGhost";
  }
#endif
  bool isAllocationFailure() const override { return true; }
};

}

#endif
