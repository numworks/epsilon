#ifndef POINCARE_TEST_PAIR_NODE_H
#define POINCARE_TEST_PAIR_NODE_H

#include <poincare/tree_node.h>
#include <poincare/tree_handle.h>

namespace Poincare {

class PairNode : public TreeNode {
public:
  virtual size_t size() const override { return sizeof(PairNode); }
  virtual int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Pair";
  }
#endif
};

class PairByReference : public TreeHandle {
public:
  PairByReference(TreeHandle t1, TreeHandle t2) : TreeHandle(TreePool::sharedPool()->createTreeNode<PairNode>()) {
    replaceChildAtIndexInPlace(0, t1);
    replaceChildAtIndexInPlace(1, t2);
  }
};

}

#endif
