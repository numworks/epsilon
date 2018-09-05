#ifndef POINCARE_TEST_PAIR_NODE_H
#define POINCARE_TEST_PAIR_NODE_H

#include <poincare/tree_node.h>
#include <poincare/tree_by_reference.h>

namespace Poincare {

class PairNode : public TreeNode {
public:
  TreeNode * uninitializedStaticNode() const override;
  virtual size_t size() const override { return sizeof(PairNode); }
  virtual int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Pair";
  }
#endif
};

class UninitializedPairNode : public PairNode {
public:
  static UninitializedPairNode * UninitializedPairStaticNode();

  size_t size() const override { return sizeof(UninitializedPairNode); }
  bool isUninitialized() const override { return true; }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedPairNode";
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

}

#endif
