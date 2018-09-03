#ifndef POINCARE_UNINITIALIZED_GHOST_NODE_H
#define POINCARE_UNINITIALIZED_GHOST_NODE_H

#include <poincare/exception_node.h>
#include <poincare/ghost_node.h>
#include <stdio.h>

namespace Poincare {

class UninitializedGhostNode : public ExceptionNode<GhostNode> {
public:
  static UninitializedGhostNode * UninitializedGhostStaticNode();

  // TreeNode
  bool isUninitialized() const override { return true; }
  /* There is only one static node, that should never be inserted in the pool,
   * so no need for an allocation failure. */
  TreeNode * failedAllocationStaticNode() override { assert(false); return nullptr; }
  size_t size() const override { return sizeof(UninitializedGhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedGhost";
  }
#endif
};

}

#endif
