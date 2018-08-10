#ifndef POINCARE_TEST_PAIR_NODE_H
#define POINCARE_TEST_PAIR_NODE_H

#include <poincare/tree_node.h>
#include <poincare/allocation_failure_node.h>
#include <poincare/tree_by_reference.h>
#include <poincare/tree_by_value.h>

namespace Poincare {


class PairNode : public TreeNode {
public:
  static PairNode * FailedAllocationStaticNode() {
    static AllocationFailureNode<PairNode> failureNode;
    TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
    return &failureNode;
  }
  virtual PairNode * failedAllocationStaticNode() override {
    return PairNode::FailedAllocationStaticNode();
  }
  virtual size_t size() const override { return sizeof(PairNode); }
  virtual int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Pair";
  }
#endif
};


class PairByReference : public TreeByReference {
public:
  PairByReference(TreeByReference t1, TreeByReference t2) : TreeByReference(TreePool::sharedPool()->createTreeNode<PairNode>()) {
    replaceChildAtIndexInPlace(0, t1);
    replaceChildAtIndexInPlace(1, t2);
  }
};


class PairByValue : public TreeByValue {
public:
  PairByValue(TreeByValue t1, TreeByValue t2) : TreeByValue(TreePool::sharedPool()->createTreeNode<PairNode>()) {
    replaceChildAtIndexInPlace(0, t1);
    replaceChildAtIndexInPlace(1, t2);
  }
};


}

#endif
