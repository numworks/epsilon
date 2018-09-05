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
  size_t size() const override { return sizeof(UninitializedGhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedGhost";
  }
#endif
};

}

#endif
